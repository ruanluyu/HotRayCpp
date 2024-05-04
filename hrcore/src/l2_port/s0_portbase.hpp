#pragma once
#include <string>
#include "l1_ray/s0_raybase.hpp"
#include "l1_ray/s2_configs.hpp"
#include "l0_def/s1_global.hpp"
namespace hr::port {
	using namespace hr::ray;
	using namespace hr::def;
	class PortBase : ObjectBase {
	protected:
		sptr<GlobalRayConfigs::RayConfig> ray_config;
		RayData data;
	public:
		PortBase();
		virtual void Disconnect() = 0;
		bool SetRayType(const hr_string& uname);
	};

	class InPort : public PortBase {
	private:
	public:
		InPort();
		void Disconnect() override;

	};

	class OutPort :public PortBase {
	private:
		wptr<InPort> target_port;
	public:
		OutPort();
		virtual bool TryConnectTo(InPort* port);
		void Disconnect() override;
	};
}