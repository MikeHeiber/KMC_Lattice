// Copyright (c) 2017-2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Event.h"

using namespace std;

namespace KMC_Lattice {

	// Initialize static class members
	const string Event::event_type_base = "Event";

	Event::~Event() {

	}

	Event::Event() {

	}

	Event::Event(Simulation* simulation_ptr) {
		sim_ptr = simulation_ptr;
	}

	void Event::calculateExecutionTime(const double rate) {
		execution_time = sim_ptr->getTime() - (log(sim_ptr->rand01()) / rate);
	}

	void Event::calculateRateConstant(const double input_rate) {
		rate_constant = input_rate;
	}

	Coords Event::getDestCoords() const {
		return coords_dest;
	}

	string Event::getEventType() const {
		return event_type_base;
	}

	double Event::getExecutionTime() const {
		return execution_time;
	}

	Object* Event::getObjectPtr() const {
		return object_ptr;
	}

	Object* Event::getObjectTargetPtr() const {
		return object_target_ptr;
	}

	double Event::getRateConstant() const {
		return rate_constant;
	}

	void Event::setDestCoords(const Coords& coords) {
		coords_dest = coords;
	}

	bool Event::setExecutionTime(const double time) {
		if (time < 0) {
			return false;
		}
		else {
			execution_time = time;
			return true;
		}
	}

	void Event::setObjectPtr(Object* input_ptr) {
		object_ptr = input_ptr;
	}

	void Event::setObjectTargetPtr(Object* input_ptr) {
		object_target_ptr = input_ptr;
	}

	void Event::setRateConstant(double val) {
		rate_constant = val;
	}

}
