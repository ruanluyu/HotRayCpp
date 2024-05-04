#include "pch.h"
#include "s1_global.hpp"

namespace hr::def {

	namespace mem {

		static void* default_allocate(const ui64& size_in_bytes)
		{
			return malloc(size_in_bytes);
		}

		static void default_free(void* const& pointer_to_be_freed)
		{
			void* copied_pointer = pointer_to_be_freed;
			free(copied_pointer);
		}


		static ManagementConfigs global_strategy{
				.memory_allocate = default_allocate,
				.memory_free = default_free
		};

		void init_strategy(const ManagementConfigs& configs) {
			if (configs.memory_allocate == nullptr) {
				throw std::invalid_argument("memory_allocate should not be nullptr. ");
			}
			if (configs.memory_free == nullptr)
			{
				throw std::invalid_argument("memory_free should not be nullptr. ");
			}
			global_strategy = configs;
		}


		ManagementConfigs get_strategy()
		{
			return global_strategy;
		}

		void* hr_allocate(const ui64& size_in_bytes)
		{
			return global_strategy.memory_allocate(size_in_bytes);
		}

		void hr_free(void* const& pointer_to_be_freed)
		{
			return global_strategy.memory_free(pointer_to_be_freed);
		}

		bool ManagementConfigs::operator==(const ManagementConfigs& other)
		{
			return (this->memory_allocate == other.memory_allocate) &&
				(this->memory_free == other.memory_free);
		}
	}


	LogFunctionConfigs global_configs = {nullptr, nullptr, nullptr};

	void init_log(const LogFunctionConfigs& configs)
	{
		global_configs = configs;
	}
	bool has_info_logger()
	{
		return global_configs.info != nullptr;
	}
	bool has_warning_logger()
	{
		return global_configs.warning != nullptr;
	}
	bool has_error_logger()
	{
		return global_configs.error != nullptr;
	}
	void log_info(const char* info)
	{
		if (global_configs.info) global_configs.info(info);
	}
	void log_warning(const char* warning)
	{
		if (global_configs.warning) global_configs.warning(warning);
	}
	void log_error(const char* error)
	{
		if (global_configs.error) global_configs.error(error);
	}

	void DestroyObject(const ObjectBase::ObjectPointer& object)
	{
		ObjectBase::Destroy(object);
	}


#define ASSERT_OBJECT_NOT_DESTROYED  \
	DEBUG_ASSERT(self != nullptr ,"You are calling a function of a destroyed object. ")


	ObjectBase::ObjectBase() :name(), children(), parent(), self(){}
	ObjectBase::~ObjectBase(){
		BEGIN_DEBUG_CHUNK
		{
			hr_stringstream ss;
			ss << "Deconstruct: " << this;
			DEBUG_FAST_LOG(ss.str());
		}
		END_DEBUG_CHUNK
	}


	void ObjectBase::SetName(const char* new_name)
	{
		ASSERT_OBJECT_NOT_DESTROYED;
		if (name == nullptr)
		{
			name = hr_make_shared<hr_string>(new_name);
		}
		else
		{
			name->assign(new_name);
		}
	}

	const char* ObjectBase::GetName() const
	{
		ASSERT_OBJECT_NOT_DESTROYED;
		if (name == nullptr) return "";
		return name->c_str();
	}

	void ObjectBase::RemoveParent() {
		ASSERT_OBJECT_NOT_DESTROYED;
		auto parent_ptr = parent.lock();
		if (parent_ptr == nullptr) return;
		if (parent_ptr->children == nullptr)
		{
			parent.reset();
			return;
		}
		parent_ptr->children->erase(self);
		parent.reset();
	}

	void ObjectBase::AddChild(ObjectPointer& child)
	{
		ASSERT_OBJECT_NOT_DESTROYED;
		if (child == nullptr) return;
		child->RemoveParent();
		if (children == nullptr)
		{
			children = hr_make_shared<hr_unordered_set<sptr<ObjectBase>>>();
		}
		children->insert(child);
	}

	void ObjectBase::Destroy(const ObjectPointer& object)
	{
		if (object == nullptr)
		{
			return;
		}
		if (object->IsDestroyed())
		{
			hr_stringstream ss;
			ss << "Object at " << object << " has been destroyed more than once. " << std::endl;
			DEBUG_FAST_LOG(ss.str());
			return;
		}
		object->self = nullptr;
		if (object->children != nullptr)
		{
			ChildrenContainer& children = *object->children;
			for (auto& child : children)
			{
				Destroy(child);
			}
			children.clear();
		}
	}
	

	ObjectBase::ObjectPointer ObjectBase::GetPointerToSelf() const
	{
		ASSERT_OBJECT_NOT_DESTROYED;
		return ObjectPointer(self);
	}


	bool ObjectBase::IsDestroyed() const {
		return self == nullptr;
	}

	const ObjectBase::ChildrenContainer& ObjectBase::GetChildren() const {
		return *children;
	}

	

}