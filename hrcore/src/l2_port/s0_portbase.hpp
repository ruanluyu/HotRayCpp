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

	public:
		PortBase();
		virtual bool SetRayType(const char* uname);
		virtual const char* GetRayUname() const;

		virtual RayData* GetDataRawPointer();
	};

	class OutPort;

	class InPort : public PortBase {
	private:
		wptr<OutPort> source_port;
	protected:
		void _ConnectFrom(const ObjectPointer& other) override;
	public:
		InPort();
		virtual const RayData& ReadData() const;
		virtual const wptr<OutPort>& GetSourcePort() const;
	};

	class OutPort :public PortBase {
	private:
		wptr<InPort> target_port;
		sptr<BasicConverter> converter;
		sptr<BasicConverter> _GetConverterTo(const sptr<InPort>& target) const;
		void _ForceConnectTo(const sptr<InPort>& target);
		void _ConnectFrom(const ObjectPointer& other) override;
	public:
		OutPort();
		virtual bool ConnectableTo(const sptr<InPort>& target) const;
		virtual bool TryConnectTo(const sptr<InPort>& target);
		virtual void Disconnect();

		virtual void Send();
		virtual const wptr<InPort>& GetTargetPort() const;
	};
}