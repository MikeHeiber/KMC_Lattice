// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Lattice.h"

Lattice::Lattice() {

}

Lattice::~Lattice() {

}

void Lattice::init(const Parameters_Lattice& params, mt19937* generator) {
	Enable_periodic_x = params.Enable_periodic_x;
	Enable_periodic_y = params.Enable_periodic_y;
	Enable_periodic_z = params.Enable_periodic_z;
	Length = params.Length;
	Width = params.Width;
	Height = params.Height;
	Unit_size = params.Unit_size;
	site_ptrs.assign(Length*Width*Height, nullptr);
	gen = generator;
}

Coords Lattice::calculateDestinationCoords(const Coords& coords_initial, const int i, const int j, const int k) {
	Coords dest_coords;
	dest_coords.x = coords_initial.x + i + calculateDX(coords_initial.x, i);
	dest_coords.y = coords_initial.y + j + calculateDY(coords_initial.y, j);
	dest_coords.z = coords_initial.z + k + calculateDZ(coords_initial.z, k);
	return dest_coords;
}

//  This function calculates the coordinate adjustment term needed to account for periodic boundaries in the x-direction.
int Lattice::calculateDX(const int x, const int i) {
	if (!Enable_periodic_x) {
		return 0;
	}
	else {
		if (x + i<0) {
			return Length;
		}
		else if (x + i >= Length) {
			return -Length;
		}
		else {
			return 0;
		}
	}
}

//  This function calculates the coordinate adjustment term needed to account for periodic boundaries in the y-direction.
int Lattice::calculateDY(const int y, const int j) {
	if (!Enable_periodic_y) {
		return 0;
	}
	else {
		if (y + j<0) {
			return Width;
		}
		else if (y + j >= Width) {
			return -Width;
		}
		else {
			return 0;
		}
	}
}

//  This function calculates the coordinate adjustment term needed to account for possible periodic boundaries in the z-direction.
int Lattice::calculateDZ(const int z, const int k) {
	if (!Enable_periodic_z) {
		return 0;
	}
	else {
		if (z + k<0) { // Check for periodic z boundary
			return Height;
		}
		else if (z + k >= Height) {
			return -Height;
		}
		else {
			return 0;
		}
	}
}

int Lattice::calculateLatticeDistanceSquared(const Coords& coords_start, const Coords& coords_dest) {
	int absx = abs(coords_dest.x - coords_start.x);
	int absy = abs(coords_dest.y - coords_start.y);
	int absz = abs(coords_dest.z - coords_start.z);
	int dx, dy, dz;
	if (Enable_periodic_x && 2 * absx>Length) {
		dx = -Length;
	}
	else {
		dx = 0;
	}
	if (Enable_periodic_y && 2 * absy>Width) {
		dy = -Width;
	}
	else {
		dy = 0;
	}
	if (Enable_periodic_z && 2 * absz>Height) {
		dz = -Height;
	}
	else {
		dz = 0;
	}
	return (absx + dx)*(absx + dx) + (absy + dy)*(absy + dy) + (absz + dz)*(absz + dz);
}

int Lattice::calculateXPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest) {
	if (Enable_periodic_x) {
		if (2*(coords_dest.x-coords_initial.x)<-Length) {
			return 1;
		}
		else if (2 * (coords_dest.x - coords_initial.x)>Length) {
			return -1;
		}
	}
	else {
		return 0;
	}
}

int Lattice::calculateYPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest) {
	if (Enable_periodic_y) {
		if (2 * (coords_dest.y - coords_initial.y)<-Width) {
			return 1;
		}
		else if (2 * (coords_dest.y - coords_initial.y)>Width) {
			return -1;
		}
	}
	else {
		return 0;
	}
}

int Lattice::calculateZPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest) {
	if (Enable_periodic_z) {
		if (2 * (coords_dest.z - coords_initial.z) < -Height) {
			return 1;
		}
		else if (2 * (coords_dest.z - coords_initial.z) > Height) {
			return -1;
		}
	}
	else {
		return 0;
	}
}

void Lattice::clearOccupancy(const Coords& coords) {
	site_ptrs[getSiteIndex(coords)]->clearOccupancy();
}

int Lattice::getHeight() {
	return Height;
}

int Lattice::getLength() {
	return Length;
}

int Lattice::getNumSites() {
	return (int)site_ptrs.size();
}

Coords Lattice::getRandomCoords() {
	Coords coords;
	coords.x = getRandomX();
	coords.y = getRandomY();
	coords.z = getRandomZ();
	return coords;
}

int Lattice::getRandomX() {
	uniform_int_distribution<int> distx(0, Length - 1);
	auto randx = bind(distx, ref(*gen));
	return randx();
}

int Lattice::getRandomY() {
	uniform_int_distribution<int> disty(0, Width - 1);
	auto randy = bind(disty, ref(*gen));
	return randy();
}

int Lattice::getRandomZ() {
	uniform_int_distribution<int> distz(0, Height - 1);
	auto randz = bind(distz, ref(*gen));
	return randz();
}

int Lattice::getSiteIndex(const Coords& coords) {
	return coords.x*Width*Height + coords.y*Height + coords.z;
}

vector<Site*>::iterator Lattice::getSiteIt(const Coords& coords) {
	auto site_it = site_ptrs.begin();
	advance(site_it, getSiteIndex(coords));
	return site_it;
}

double Lattice::getUnitSize() {
	return Unit_size;
}

int Lattice::getWidth() {
	return Width;
}

bool Lattice::isOccupied(const Coords& coords) {
	return (*getSiteIt(coords))->isOccupied();
}

bool Lattice::isXPeriodic() {
	return Enable_periodic_x;
}

bool Lattice::isYPeriodic() {
	return Enable_periodic_y;
}

bool Lattice::isZPeriodic() {
	return Enable_periodic_z;
}

void Lattice::outputLatticeOccupancy() {
	for (auto site_it = site_ptrs.begin(); site_it != site_ptrs.end(); ++site_it) {
		if ((*site_it)->isOccupied()) {
			cout << "Site " << distance(site_ptrs.begin(), site_it) << " is occupied." << endl;
		}
	}
}

void Lattice::setOccupied(const Coords& coords) {
	site_ptrs[getSiteIndex(coords)]->setOccupied();
}

bool Lattice::setSitePointers(vector<Site*> input_ptrs) {
	if ((int)input_ptrs.size() != getNumSites()){
		cout << "Error setting the lattice site pointers! The number of sites in the input vector do not equal the expected number of lattice sites." << endl;
		return false;
	}
	else {
		site_ptrs = input_ptrs;
		return true;
	}
}