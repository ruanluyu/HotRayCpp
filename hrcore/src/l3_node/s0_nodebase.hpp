#pragma once

#include "l0_def/s1_global.hpp"
#include "l2_port/s0_portbase.hpp"

namespace hr::node {
	using namespace hr::def;
	using namespace hr::port;

	class NodeBase :public ObjectBase {
	private:
	protected:
		hr_vector<InPort> inports;
		hr_vector<OutPort> outports;
		virtual void ActivateMeNextTick();
	public:
		NodeBase();

		virtual void OnInit();
		virtual void OnActivated();
	};

}

