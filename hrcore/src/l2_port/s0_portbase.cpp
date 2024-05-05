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
	void PortBase::_DisconnectBetween(const ObjectPointer& from, const ObjectPointer& to)
	{
		auto from_port = std::static_pointer_cast<PortBase>(from);
		auto to_port = std::static_pointer_cast<PortBase>(to);

		DEBUG_ASSERT(from_port != nullptr, "from is null");
		DEBUG_ASSERT(to_port != nullptr, "to is null");

		from_port->_DisconnectFrom(to_port);
		to_port->_DisconnectFrom(from_port);
	}

	PortBase::PortBase(): ObjectBase(), ray_config(nullptr), data()
	{}



	bool PortBase::SetRayType(const char* uname)
	{
		auto res = hr::ray::global_configs.GetConfig(uname);
		if (res == nullptr) return false;
		if (ray_config != nullptr)
		{
			ray_config->free_function(data);
		}
		ray_config = res;
		res->init_function(data);
		return true;
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


	InPort::InPort()
	{

	}
	OutPort::OutPort(): converter()
	{
	}

	sptr<BasicConverter> OutPort::_GetConverterTo(const sptr<InPort>& target) const
	{
		if (target == nullptr || target->IsDestroyed() || this->IsDestroyed()) return nullptr;
		return hr::ray::global_configs.GetConverter(target->GetRayUname(), this->GetRayUname());
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
			_ConnectBetween(GetPointerToSelf(), target);
			this->converter = converter;
			return true;
		}
		return false;
	}

	void OutPort::Send()
	{
		if (converter == nullptr || connected_port == nullptr) return;
		converter->Apply(data, *connected_port->GetDataRawPointer());
	}


	void SingleConnectionPort::_ConnectFrom(const ObjectPointer& other)
	{
		connected_port = std::dynamic_pointer_cast<PortBase>(other);
	}
	void SingleConnectionPort::_DisconnectFrom(const ObjectPointer& other)
	{
		if (other != connected_port) return;
		connected_port = nullptr;
	}
	void SingleConnectionPort::_OnDestroy()
	{
		Disconnect();
	}
	SingleConnectionPort::SingleConnectionPort():connected_port()
	{
	}
	const sptr<PortBase>& SingleConnectionPort::GetConnectedPort() const
	{
		return connected_port;
	}
	void SingleConnectionPort::Disconnect()
	{
		_DisconnectBetween(GetPointerToSelf(), connected_port);
	}
}
