#include "pch.h"
#include "s0_portbase.hpp"

namespace hr::port{
	void SinglePort::_ConnectBetween(const ObjectPointer& from, const ObjectPointer& to)
	{
		auto from_port = std::static_pointer_cast<SinglePort>(from);
		auto to_port = std::static_pointer_cast<SinglePort>(to);

		DEBUG_ASSERT(from_port != nullptr, "from is null");
		DEBUG_ASSERT(to_port != nullptr, "to is null");

		from_port->_ConnectFrom(to_port);
		to_port->_ConnectFrom(from_port);
	}
	void SinglePort::_DisconnectBetween(const ObjectPointer& from, const ObjectPointer& to)
	{
		auto from_port = std::static_pointer_cast<SinglePort>(from);
		auto to_port = std::static_pointer_cast<SinglePort>(to);

		DEBUG_ASSERT(from_port != nullptr, "from is null");
		DEBUG_ASSERT(to_port != nullptr, "to is null");

		from_port->_DisconnectFrom(to_port);
		to_port->_DisconnectFrom(from_port);
	}

	PortContainer::PortContainer()
	{}

	void SinglePort::SetRayConfig(const sptr<ConfigType>&config)
	{
		if (ray_config != nullptr)
		{
			ray_config->free_function(data);
		}
		ray_config = config;
		if (ray_config != nullptr)
		{
			ray_config->init_function(data);
		}
	}

	const sptr<ConfigType>& SinglePort::GetRayConfig() const
	{
		return ray_config;
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
		return hr::ray::global_ray_configs.GetConverter(target->GetRayConfig(), this->GetRayConfig());
	}

	bool OutPort::ConnectableTo(const sptr<InPort>& target) const
	{
		return _GetConverterTo(target) != nullptr;
	}

	bool OutPort::TryConnectTo(const sptr<InPort>& target)
	{
		if (target == nullptr || target->IsDestroyed() || this->IsDestroyed()) return false;
		auto converter = hr::ray::global_ray_configs.GetConverter(this->GetRayConfig(), target->GetRayConfig());
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


	void SinglePort::_ConnectFrom(const ObjectPointer& other)
	{
		connected_port = std::dynamic_pointer_cast<SinglePort>(other);
	}
	void SinglePort::_DisconnectFrom(const ObjectPointer& other)
	{
		if (other != connected_port) return;
		connected_port = nullptr;
	}
	void SinglePort::OnDestroy()
	{
		if (ray_config != nullptr)
			ray_config->free_function(data);
		PortContainer::OnDestroy();
	}
	void PortContainer::OnDestroy()
	{
		DisconnectAll();
		ObjectBase::OnDestroy();
	}
	RayData* SinglePort::GetDataRawPointer()
	{
		return &data;
	}
	SinglePort::SinglePort():connected_port(), ray_config(nullptr), data()
	{
	}
	const sptr<SinglePort>& SinglePort::GetConnectedPort() const
	{
		return connected_port;
	}
	void SinglePort::DisconnectAll()
	{
		if(connected_port != nullptr) _DisconnectBetween(GetPointerToSelf(), connected_port);
	}
}
