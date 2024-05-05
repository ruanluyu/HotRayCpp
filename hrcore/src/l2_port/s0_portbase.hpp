#pragma once
#include <string>
#include "l1_ray/s0_raybase.hpp"
#include "l1_ray/s2_configs.hpp"
#include "l0_def/s1_global.hpp"
namespace hr::port {
	using namespace hr::ray;
	using namespace hr::def;
	class PortBase :public ObjectBase {
	protected:
		sptr<GlobalRayConfigs::RayConfig> ray_config;
		RayData data;

		void _ConnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _ConnectFrom(const ObjectPointer& other) = 0;

		void _DisconnectBetween(const ObjectPointer& from, const ObjectPointer& to);
		virtual void _DisconnectFrom(const ObjectPointer& other) = 0;

	public:
		PortBase();
		virtual bool SetRayType(const char* uname);
		virtual const char* GetRayUname() const;

		virtual RayData* GetDataRawPointer();

	};


	class SingleConnectionPort :public PortBase {
	private:
	protected:
		sptr<PortBase> connected_port;
		void _ConnectFrom(const ObjectPointer& other) override;
		void _DisconnectFrom(const ObjectPointer& other) override;
		void _OnDestroy() override;
	public:
		SingleConnectionPort();
		virtual const sptr<PortBase>& GetConnectedPort() const;
		virtual void Disconnect();
	};


	class InPort : public SingleConnectionPort {
	private:
	protected:
	public:
		InPort();
	};


	class OutPort :public SingleConnectionPort {
	private:
		sptr<BasicConverter> converter;
		sptr<BasicConverter> _GetConverterTo(const sptr<InPort>& target) const;
	public:
		OutPort();
		virtual bool ConnectableTo(const sptr<InPort>& target) const;
		virtual bool TryConnectTo(const sptr<InPort>& target);

		virtual void Send();
	};
}