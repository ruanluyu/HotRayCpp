#include "pch.h"
#include "s0_portbase.hpp"

namespace hr::port{
	
	
	PortBase::PortBase(): ObjectBase(), ray_config(nullptr), data()
	{
	}



	bool PortBase::SetRayType(const hr_string& uname)
	{
		auto res = hr::ray::global_configs.get_config(uname);
		ray_config = res;
		return res != nullptr;
	}
}
