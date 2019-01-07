// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Lattice.h"

using namespace std;

namespace KMC_Lattice {

	Lattice::Lattice() {

	}

	void Lattice::init(const Parameters_Lattice& params, mt19937_64* generator_ptr) {
		if (!params.checkParameters()) {
			throw invalid_argument("Error! Input lattice parameters are invalid");
		}
		Enable_periodic_x = params.Enable_periodic_x;
		Enable_periodic_y = params.Enable_periodic_y;
		Enable_periodic_z = params.Enable_periodic_z;
		Length = params.Length;
		Width = params.Width;
		Height = params.Height;
		Unit_size = params.Unit_size;
		site_ptrs.assign(Length*Width*Height, nullptr);
		gen_ptr = generator_ptr;
	}

	void Lattice::calculateDestinationCoords(const Coords& coords_initial, const int i, const int j, const int k, Coords& coords_dest) const {
		coords_dest.x = coords_initial.x + i + calculateDX(coords_initial.x, i);
		coords_dest.y = coords_initial.y + j + calculateDY(coords_initial.y, j);
		coords_dest.z = coords_initial.z + k + calculateDZ(coords_initial.z, k);
	}

	int Lattice::calculateDX(const int x, const int i) const {
		if (Enable_periodic_x && x + i < 0) {
			return Length;
		}
		else if (Enable_periodic_x && x + i >= Length) {
			return -Length;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateDX(const Coords& coords_initial, const Coords& coords_dest) const {
		if (Enable_periodic_x && 2 * (coords_dest.x - coords_initial.x) > Length) {
			return Length;
		}
		else if (Enable_periodic_x && 2 * (coords_dest.x - coords_initial.x) < -Length) {
			return -Length;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateDY(const int y, const int j) const {
		if (Enable_periodic_y && y + j < 0) {
			return Width;
		}
		else if (Enable_periodic_y && y + j >= Width) {
			return -Width;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateDY(const Coords& coords_initial, const Coords& coords_dest) const {
		if (Enable_periodic_y && 2 * (coords_dest.y - coords_initial.y) > Width) {
			return Width;
		}
		else if (Enable_periodic_y && 2 * (coords_dest.y - coords_initial.y) < -Width) {
			return -Width;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateDZ(const int z, const int k) const {
		if (Enable_periodic_z && z + k < 0) {
			return Height;
		}
		else if (Enable_periodic_z && z + k >= Height) {
			return -Height;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateDZ(const Coords& coords_initial, const Coords& coords_dest) const {
		if (Enable_periodic_z && 2 * (coords_dest.z - coords_initial.z) > Height) {
			return Height;
		}
		else if (Enable_periodic_z && 2 * (coords_dest.z - coords_initial.z) < -Height) {
			return -Height;
		}
		else {
			return 0;
		}
	}

	int Lattice::calculateLatticeDistanceSquared(const Coords& coords_start, const Coords& coords_dest) const {
		int absx = abs(coords_dest.x - coords_start.x);
		int absy = abs(coords_dest.y - coords_start.y);
		int absz = abs(coords_dest.z - coords_start.z);
		int dx, dy, dz;
		if (Enable_periodic_x && 2 * absx > Length) {
			dx = -Length;
		}
		else {
			dx = 0;
		}
		if (Enable_periodic_y && 2 * absy > Width) {
			dy = -Width;
		}
		else {
			dy = 0;
		}
		if (Enable_periodic_z && 2 * absz > Height) {
			dz = -Height;
		}
		else {
			dz = 0;
		}
		return (absx + dx)*(absx + dx) + (absy + dy)*(absy + dy) + (absz + dz)*(absz + dz);
	}

	bool Lattice::checkMoveValidity(const Coords& coords_initial, const int i, const int j, const int k) const {
		if (i == 0 && j == 0 && k == 0) {
			return false;
		}
		if (!Enable_periodic_x && (coords_initial.x + i >= Length || coords_initial.x + i < 0)) {
			return false;
		}
		if (!Enable_periodic_y && (coords_initial.y + j >= Width || coords_initial.y + j < 0)) {
			return false;
		}
		if (!Enable_periodic_z && (coords_initial.z + k >= Height || coords_initial.z + k < 0)) {
			return false;
		}
		return true;
	}

	Coords Lattice::chooseRandomUnoccupiedNeighbor(const Coords& coords_i) {
		vector<Coords> coords_vec;
		coords_vec.reserve(6);
		Coords coords_f;
		for (int n = 0; n < 6; n++) {
			int i = 0;
			int j = 0;
			int k = 0;
			if (n == 0) {
				i = -1;
			}
			else if (n == 1) {
				i = 1;
			}
			else if (n == 2) {
				j = -1;
			}
			else if (n == 3) {
				j = 1;
			}
			else if (n == 4) {
				k = -1;
			}
			else if (n == 5) {
				k = 1;
			}
			if (checkMoveValidity(coords_i, i, j, k)) {
				calculateDestinationCoords(coords_i, i, j, k, coords_f);
				if (!isOccupied(coords_f)) {
					coords_vec.push_back(coords_f);
				}
			}
		}
		if ((int)coords_vec.size() == 1) {
			return coords_vec[0];
		}
		else if ((int)coords_vec.size() > 1) {
			uniform_int_distribution<> dist(0, (int)coords_vec.size() - 1);
			return coords_vec[dist(*gen_ptr)];
		}
		else {
			coords_f = { -1,-1,-1 };
			return coords_f;
		}
	}

	void Lattice::clearOccupancy(const Coords& coords) {
		site_ptrs[getSiteIndex(coords)]->clearOccupancy();
	}

	Coords Lattice::generateRandomCoords() {
		Coords coords;
		coords.x = generateRandomX();
		coords.y = generateRandomY();
		coords.z = generateRandomZ();
		return coords;
	}

	int Lattice::generateRandomX() {
		uniform_int_distribution<int> distx(0, Length - 1);
		return distx(*gen_ptr);
	}

	int Lattice::generateRandomY() {
		uniform_int_distribution<int> disty(0, Width - 1);
		return disty(*gen_ptr);
	}

	int Lattice::generateRandomZ() {
		uniform_int_distribution<int> distz(0, Height - 1);
		return distz(*gen_ptr);
	}

	int Lattice::getHeight() const {
		return Height;
	}

	int Lattice::getLength() const {
		return Length;
	}

	long int Lattice::getNumSites() const {
		return (int)site_ptrs.size();
	}

	Coords Lattice::getSiteCoords(long int site_index) {
		if (site_index < 0 || site_index >= Length * Width*Height) {
			cout << "Error! Input site index is not located in the lattice." << endl;
			throw out_of_range("Input site index is not located in the lattice.");
		}
		Coords coords;
		coords.x = site_index / (Width*Height);
		int remainder = site_index % (Width*Height);
		coords.y = remainder / Height;
		coords.z = remainder % Height;
		return coords;
	}

	long int Lattice::getSiteIndex(const Coords& coords) const {
		if (coords.x >= 0 && coords.x < Length && coords.y >= 0 && coords.y < Width && coords.z >= 0 && coords.z < Height) {
			return coords.x*Width*Height + coords.y*Height + coords.z;
		}
		else {
			cout << "Error! Input coordinates are not located in the lattice." << endl;
			throw out_of_range("Input coordinates are not located in the lattice.");
		}
	}

	vector<Site*>::iterator Lattice::getSiteIt(const Coords& coords) {
		auto site_it = site_ptrs.begin();
		advance(site_it, getSiteIndex(coords));
		return site_it;
	}

	double Lattice::getUnitSize() const {
		return Unit_size;
	}

	int Lattice::getWidth() const {
		return Width;
	}

	double Lattice::getVolume() const {
		return ((Length*Width*Height*1e-7*Unit_size)*1e-7*Unit_size)*1e-7*Unit_size;
	}

	bool Lattice::isOccupied(const Coords& coords) const {
		return site_ptrs[getSiteIndex(coords)]->isOccupied();
	}

	bool Lattice::isXPeriodic() const {
		return Enable_periodic_x;
	}

	bool Lattice::isYPeriodic() const {
		return Enable_periodic_y;
	}

	bool Lattice::isZPeriodic() const {
		return Enable_periodic_z;
	}

	void Lattice::outputLatticeOccupancy() const {
		for (auto site_it = site_ptrs.begin(); site_it != site_ptrs.end(); ++site_it) {
			if ((*site_it)->isOccupied()) {
				cout << "Site " << distance(site_ptrs.begin(), site_it) << " is occupied." << endl;
			}
		}
	}

	void Lattice::setOccupied(const Coords& coords) {
		site_ptrs[getSiteIndex(coords)]->setOccupied();
	}

	bool Lattice::setSitePointers(const vector<Site*>& input_ptrs) {
		if ((int)input_ptrs.size() != getNumSites()) {
			cout << "Error setting the lattice site pointers! The number of sites in the input vector do not equal the expected number of lattice sites." << endl;
			return false;
		}
		else {
			site_ptrs = input_ptrs;
			return true;
		}
	}

}
