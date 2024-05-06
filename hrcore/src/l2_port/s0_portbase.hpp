#pragma once
#include <string>
#include "l1_ray/s0_raybase.hpp"
#include "l1_ray/s2_configs.hpp"
#include "l0_def/s1_global.hpp"
namespace hr::port {
	using namespace hr::ray;
	using namespace hr::def;
	class PortContainer :public ObjectBase {
	protected:
		void OnDestroy() override;
	public:
		PortContainer();
		virtual void DisconnectAll() = 0;
	};

	using ConfigType = GlobalRayConfigs::RayConfig;

	class SinglePort :public PortContainer {
	private:
	protected:
		RayData data;
		sptr<SinglePort> connected_port;
		sptr<GlobalRayConfigs::RayConfig> ray_config;

		void _ConnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _ConnectFrom(const ObjectPointer& other);

		void _DisconnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _DisconnectFrom(const ObjectPointer& other);

		void OnDestroy() override;
	public:
		virtual void SetRayConfig(const sptr<ConfigType>& config);
		virtual const sptr<ConfigType>& GetRayConfig() const;

		SinglePort();
		virtual const sptr<SinglePort>& GetConnectedPort() const;
		void DisconnectAll() override;


		virtual RayData* GetDataRawPointer();
	};


	class InPort : public SinglePort {
	private:
	protected:
	public:
		InPort();
	};


	class OutPort :public SinglePort {
	private:
		sptr<BasicConverter> converter;
		sptr<BasicConverter> _GetConverterTo(const sptr<InPort>& target) const;
	public:
		OutPort();
		virtual bool ConnectableTo(const sptr<InPort>& target) const;
		virtual bool TryConnectTo(const sptr<InPort>& target);

		virtual void Send();
	};

	class PortCollectionBase : public PortContainer {
	public:
		virtual ui64 GetSize() const = 0;
		virtual void RemoveAt(const ui64& index) = 0;
	};

	class PortArrayBase :public PortCollectionBase {
	public:
		virtual void SetSize(const ui64& size) = 0;
		virtual void SetRayConfig(const sptr<ConfigType>& config) = 0;
	};


	template<class T>
	concept SinglePortConcept = std::derived_from<T, SinglePort>;

	template<SinglePortConcept SingleT>
	class PortArray : public PortArrayBase {
	protected:
		hr_vector<sptr<SingleT>> port_list;
		sptr<GlobalRayConfigs::RayConfig> ray_config;

		sptr<SingleT> _CreatePort()
		{
			auto new_obj = CreateObject<SingleT>();
			auto casted_obj_ptr = std::static_pointer_cast<ObjectBase>(new_obj);
			AddChild(casted_obj_ptr);
			new_obj->SetRayConfig(ray_config);
			return new_obj;
		}

		
	public:
		void SetRayConfig(const sptr<ConfigType>& config) override
		{
			ray_config = config;
		}


		void SetSize(const ui64& size) override{
			if (size == port_list.size()) return;
			auto old_size = port_list.size();
			if (size > port_list.size())
			{
				port_list.reserve(size);
				
				for (ui64 i = 0; i < size - old_size; i++)
				{
					port_list.push_back(_CreatePort());
				}
			}
			else
			{
				for (ui64 i = size; i < old_size; i++)
				{
					DestroyObject(port_list[i]);
				}
				port_list.erase(port_list.begin() + size, port_list.end());
			}
		}

		ui64 GetSize() const override {
			return port_list.size();
		}

		const sptr<SingleT>& GetPortAt(const ui64& index)
		{
			return port_list.at(index);
		}

		void RemoveAt(const ui64& index) override {
			auto target = port_list[index];
			port_list.erase(port_list.begin() + index);
			target->DisconnectAll();
			DestroyObject(target);
		}

		void DisconnectAll() override
		{
			for (auto& port : port_list)
			{
				port->DisconnectAll();
			}
		}
	};

	using InPortArray = PortArray<InPort>;
	using OutPortArray = PortArray<OutPort>;

	template<class T, class SingleT>
	concept PortArrayConcept = SinglePortConcept<SingleT> && std::derived_from<T, PortArray<SingleT>>;



	

	template<SinglePortConcept SingleT>
	class PortCollection;

	template<class T, class SingleT>
	concept PortCollectionConcept = SinglePortConcept<SingleT> && std::derived_from<T, PortCollection<SingleT>>;

	template<class T, class SingleT>
	concept ContainerConcept =
		std::derived_from<T, PortContainer> && (
		SinglePortConcept<T> ||
		PortArrayConcept<T, SingleT> ||
		PortCollectionConcept<T, SingleT>);

	template<SinglePortConcept SingleT>
	class PortCollection : public PortCollectionBase {
		using ArrayT = PortArray<SingleT>;
		using PortListContainer = hr_vector<sptr<PortContainer>>;
	private:
		PortListContainer port_list;
	protected:
	public:
		void DisconnectAll() override {
			for (auto& port : port_list)
			{
				port->DisconnectAll();
			}
		}
		PortCollection():port_list() {}

		template<ContainerConcept<SingleT> ContainerT>
		void Add(const sptr<ContainerT>& port)
		{
			AddChild(port);
			port_list.push_back(std::static_pointer_cast<PortContainer>(port));
		}

		void RemoveAt(const ui64& index) override
		{
			auto target = port_list[index];
			port_list.erase(port_list.begin() + index);
			target->DisconnectAll();
			DestroyObject(target);
		}

		const sptr<PortContainer>& GetPortAt(const ui64& index) const {
			return port_list[index];
		}

		ui64 GetSize() const override { return port_list.size(); }
	};


	using InPortCollection = PortCollection<InPort>;
	using OutPortCollection = PortCollection<OutPort>;

}