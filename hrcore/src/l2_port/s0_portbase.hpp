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
		virtual bool SetRayConfig(const sptr<ConfigType>& config);
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

	template<class SingleT>
	requires std::convertible_to<SingleT, SinglePort>
	class PortArray : public PortContainer {
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
		bool SetRayConfig(const sptr<ConfigType>& config)
		{
			ray_config = config;
		}


		void SetSize(const ui64& size) {
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

		ui64 GetSize() const {
			return port_list.size();
		}

		const sptr<SingleT>& GetPort(const ui64& index)
		{
			return port_list.at(index);
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

}