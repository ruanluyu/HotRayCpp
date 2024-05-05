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


	class SinglePort :public PortContainer {
		using ConfigType = GlobalRayConfigs::RayConfig;
	private:
	protected:
		RayData data;
		sptr<SinglePort> connected_port;
		sptr<GlobalRayConfigs::RayConfig> ray_config;

		void _ConnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _ConnectFrom(const ObjectPointer& other);

		void _DisconnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _DisconnectFrom(const ObjectPointer& other);

		
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
	class PortArray {
	private:
	protected:
		hr_vector<SingleT> port_list;
	public:
		
	};

}