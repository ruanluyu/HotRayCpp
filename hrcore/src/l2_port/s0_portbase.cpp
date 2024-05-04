#include "pch.h"
#include "s0_portbase.hpp"

namespace hr::port{
	void PortBase::_ConnectBetween(const ObjectPointer& from, const ObjectPointer& to)
	{
		auto from_port = std::static_pointer_cast<PortBase>(from);
		auto to_port = std::static_pointer_cast<PortBase>(to);

		DEBUG_ASSERT(from_port != nullptr, "from is null");
		DEBUG_ASSERT(to_port != nullptr, "to is null");

		from_port->_ConnectFrom(to_port);
		to_port->_ConnectFrom(from_port);
	}
	PortBase::PortBase(): ObjectBase(), ray_config(nullptr), data()
	{

	}



	bool PortBase::SetRayType(const char* uname)
	{
		auto res = hr::ray::global_configs.GetConfig(uname);
		ray_config = res;
		return res != nullptr;
	}

	const char* PortBase::GetRayUname() const
	{
		if (ray_config == nullptr) 
			return "";
		return ray_config->unique_name.c_str();
	}

	RayData* PortBase::GetDataRawPointer()
	{
		return &data;
	}


	void InPort::_ConnectFrom(const ObjectPointer& other)
	{
		source_port = std::static_pointer_cast<OutPort>(other);
	}

	InPort::InPort():PortBase()
	{

	}
	const RayData& InPort::ReadData() const
	{
		return data;
	}
	const wptr<OutPort>& InPort::GetSourcePort() const
	{
		return source_port;
	}
	OutPort::OutPort():PortBase(),target_port(), converter()
	{
	}
	sptr<BasicConverter> OutPort::_GetConverterTo(const sptr<InPort>& target) const
	{
		if (target == nullptr || target->IsDestroyed() || this->IsDestroyed()) return nullptr;
		return hr::ray::global_configs.GetConverter(target->GetRayUname(), this->GetRayUname());
	}
	void OutPort::_ForceConnectTo(const sptr<InPort>& target)
	{
		_ConnectBetween(this->GetPointerToSelf(), target);
	}
	void OutPort::_ConnectFrom(const ObjectPointer& other)
	{
		target_port = std::static_pointer_cast<InPort>(other);
	}
	bool OutPort::ConnectableTo(const sptr<InPort>& target) const
	{
		return _GetConverterTo(target) != nullptr;
	}
	bool OutPort::TryConnectTo(const sptr<InPort>& target)
	{
		if (target == nullptr || target->IsDestroyed() || this->IsDestroyed()) return false;
		auto converter = hr::ray::global_configs.GetConverter(this->GetRayUname(), target->GetRayUname());
		if (converter != nullptr)
		{
			_ForceConnectTo(target);
			this->converter = converter;
			return true;
		}
		return false;
	}

	void OutPort::Disconnect()
	{
		this->target_port.reset();
	}
	void OutPort::Send()
	{
		auto active_target = target_port.lock();
		if (converter == nullptr || active_target == nullptr) return;
		converter->Apply(data, *active_target->GetDataRawPointer());
	}
	const wptr<InPort>& OutPort::GetTargetPort() const
	{
		return target_port;
	}
}
