#pragma once

#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "s0_numerical.hpp"

#ifdef HR_DEBUG
#include <source_location>
#endif

namespace hr::def {

	
	namespace mem {
		using AllocateFunction = void* (*)(const ui64& size_in_bytes);
		using FreeFunction = void (*)(void* const& pointer_to_be_freed);
		using ResizeFunction = void* (*)(void* const& pointer_to_be_resized, const ui64& new_size_in_bytes);

		struct ManagementConfigs {
			AllocateFunction memory_allocate; // required
			FreeFunction memory_free; // required

			bool operator==(const ManagementConfigs& other);
		};


		extern void init_strategy(const ManagementConfigs& configs);
		extern ManagementConfigs get_strategy();


		extern void* hr_allocate(const ui64& size_in_bytes);
		extern void hr_free(void* const& pointer_to_be_freed);

		template< class T, class... Args >
		T* hr_new(Args&&... construct_params) {
			return ::new (static_cast<void*>(hr_allocate(sizeof(T)))) T(std::forward<Args>(construct_params)...);
		}

		template< class T >
		void hr_delete(T* pointer) {
			pointer->~T();
			hr_free(pointer);
		}


		template< class T >
		class allocator {
		private:
			ManagementConfigs configs;
		public:
			using value_type = T;

			template<class U>
			friend class allocator;

			allocator(): configs(get_strategy()) {}

			template <class U>
			allocator(const allocator<U>& other): configs(other.configs) {}

			T* allocate(std::size_t n)
			{
				return reinterpret_cast<T*>(configs.memory_allocate(sizeof(T) * n));
			}

			void deallocate(T* p, std::size_t n)
			{
				static_cast<void>(n);
				configs.memory_free(static_cast<void*>(p));
			}

			template <class U>
			bool operator==(const allocator<U>& other)
			{
				return configs == other.configs;
			}

			template <class U>
			bool operator!=(const allocator<U>& other)
			{
				return !(configs == other.configs);
			}
		};

		template<class T>
		class deleter
		{
		public:
			deleter(): configs(get_strategy()) { }
			template<class U>
			friend class deleter;

			template <class U>
			deleter(const deleter<U>& other): configs(other.configs) { }
			
			template <class U>
			deleter(deleter<U>&& other): configs(other.configs) { }


			void operator()(T* p) const
			{
				p->~T();
				configs.memory_free(p);
			}

		private:
			ManagementConfigs configs;
		};
	}


	using hr_string = std::basic_string<char, std::char_traits<char>, mem::allocator<char>>;
	using hr_stringstream = std::basic_stringstream<char, std::char_traits<char>, mem::allocator<char>>;

	template<typename T>
	using hr_vector = std::vector<T, mem::allocator<T>>;

	template<typename T>
	using hr_shared_ptr = std::shared_ptr<T>;

	template<typename T>
	using hr_unique_ptr = std::unique_ptr<T, mem::deleter<T>>;

	template<typename T>
	using hr_weak_ptr = std::weak_ptr<T>;

	template<typename T>
	using hr_deque = std::deque<T, mem::allocator<T>>;

	template<typename T>
	using hr_queue = std::queue<T, hr_deque<T>>;

	template<typename T>
	using hr_stack = std::stack<T, hr_deque<T>>;


	template<typename T>
	using hr_unordered_set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, mem::allocator<T>>;

	template<typename keyT, typename valT>
	using hr_unordered_map = std::unordered_map<keyT, valT, std::hash<keyT>, std::equal_to<keyT>, mem::allocator<std::pair<const keyT, valT>>>;

	template<typename T>
	using sptr = hr_shared_ptr<T>;

	template<typename T>
	using uptr = hr_unique_ptr<T>;

	template<typename T>
	using wptr = hr_weak_ptr<T>;

	template< class T, class... Args >
	hr_shared_ptr<T> hr_make_shared(Args&&... args) {
		auto ptr = mem::hr_new<T>(std::forward<Args>(args)...);
		auto d = mem::deleter<T>();
		return hr_shared_ptr<T>(ptr, d);
	}
	
	template<typename T, typename... Args>
	hr_unique_ptr<T> hr_make_unique(Args&&... args) {
		auto ptr = mem::hr_new<T>(std::forward<Args>(args)...);
		auto d = mem::deleter<T>();
		return std::unique_ptr<T, mem::deleter<T>>(ptr, d);
	}
	

	using LogFunction = void (*)(const char*);

	struct LogFunctionConfigs {
		LogFunction info, warning, error;
	};

	void init_log(const LogFunctionConfigs& configs);

	bool has_info_logger();
	bool has_warning_logger();
	bool has_error_logger();

	void log_info(const char* info);
	void log_warning(const char* warning);
	void log_error(const char* error);

#ifdef HR_DEBUG
	constexpr bool _hrenv_debug = true;
#else
	constexpr bool _hrenv_debug = false;
#endif

#define BEGIN_DEBUG_CHUNK if constexpr (hr::def::_hrenv_debug){

#define END_DEBUG_CHUNK }

	class ObjectBase {
	public:

		using ObjectPointer = sptr<ObjectBase>;
		using ObjectWeakPointer = wptr<ObjectBase>;
		using StringPointer = sptr<hr_string>;
		using ChildrenContainer = hr_unordered_set<ObjectPointer>;

		ObjectBase();
		~ObjectBase();

		virtual void SetName(const hr_string& new_name);
		virtual hr_string GetName() const;

		virtual void RemoveParent();

		virtual void AddChild(ObjectPointer& child);

		const ChildrenContainer& GetChildren() const;

		static void Destroy(const ObjectPointer& object);

		virtual bool IsDestroyed() const;

		template<class T>
		requires (std::convertible_to<T*, ObjectBase*>)
		static sptr<T> Create() {
			auto ptr = hr_make_shared<T>();
			ptr->self = ptr;
			return ptr;
		}

		ObjectPointer GetPointerToSelf() const;

		
	protected:
		StringPointer name;
		ObjectWeakPointer parent;
		sptr<ChildrenContainer> children;
	private:
		ObjectPointer self;
	};



}



#define DEBUG_FAST_LOG(MESSAGE) \
		{BEGIN_DEBUG_CHUNK\
			const std::source_location& _dbg_location = std::source_location::current();\
			hr_stringstream _dbg_ss;\
			_dbg_ss <<"Debug log: "<< std::endl\
				<< "file: "<<_dbg_location.file_name() << std::endl\
				<< "line: "<<_dbg_location.line() << std::endl\
				<< "column: "<<_dbg_location.column() << std::endl\
				<< "function: "<<_dbg_location.function_name() << std::endl\
				<< "message: "<< MESSAGE << std::endl;\
			hr::def::log_info(_dbg_ss.str().c_str());\
		END_DEBUG_CHUNK}\
