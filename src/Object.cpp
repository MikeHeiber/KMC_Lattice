// Copyright (c) 2017-2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Object.h"

using namespace std;

namespace KMC_Lattice {

	const string Object::object_type_base = "Object";

	Object::~Object() {
		//dtor
	}

	Object::Object() {

	}

	Object::Object(const double time, const int tag_num, const Coords& start_coords) {
		time_created = time;
		tag = tag_num;
		coords_current = start_coords;
		coords_initial = start_coords;
	}

	double Object::calculateDisplacement(const int direction) const {
		switch (direction) {
		case 0:
			return sqrt((coords_current.x + dx - coords_initial.x)*(coords_current.x + dx - coords_initial.x) + (coords_current.y + dy - coords_initial.y)*(coords_current.y + dy - coords_initial.y) + (coords_current.z + dz - coords_initial.z)*(coords_current.z + dz - coords_initial.z));
		case 1:
			return (coords_current.x + dx - coords_initial.x);
		case 2:
			return (coords_current.y + dy - coords_initial.y);
		case 3:
			return (coords_current.z + dz - coords_initial.z);
		default:
			throw invalid_argument("Error! Specified direction input parameter is invalid.");
		}
	}

	Coords Object::getCoords() const {
		return coords_current;
	}

	double Object::getCreationTime() const {
		return time_created;
	}

	list<Event*>::iterator Object::getEventIt() const {
		return event_it;
	}

	string Object::getObjectType() const {
		return object_type_base;
	}

	int Object::getTag() const {
		return tag;
	}

	void Object::incrementDX(const int num) {
		dx += num;
	}

	void Object::incrementDY(const int num) {
		dy += num;
	}

	void Object::incrementDZ(const int num) {
		dz += num;
	}

	void Object::resetInitialCoords(const Coords& input_coords) {
		coords_initial = input_coords;
		dx = 0;
		dy = 0;
		dz = 0;
	}

	void Object::setCoords(const Coords& input_coords) {
		coords_current = input_coords;
	}

	void Object::setEventIt(const list<Event*>::iterator input_it) {
		event_it = input_it;
	}

}
