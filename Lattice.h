// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef LATTICE_H
#define LATTICE_H

#include "Utils.h"
#include "Site.h"
#include <functional>

using namespace std;

struct Parameters_Lattice{
	bool Enable_periodic_x;
	bool Enable_periodic_y;
	bool Enable_periodic_z;
	int Length;
	int Width;
	int Height;
	double Unit_size; // nm
};

class Lattice{
    public:
        virtual ~Lattice();
		Lattice();
        void init(const Parameters_Lattice& params, mt19937* gen);
		Coords calculateDestinationCoords(const Coords& coords_initial, const int i, const int j, const int k);
		int calculateDX(const int x, const int i);
		int calculateDY(const int y, const int j);
		int calculateDZ(const int z, const int k);
		int calculateLatticeDistanceSquared(const Coords& coords_start, const Coords& coords_dest);
		int calculateXPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest);
		int calculateYPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest);
		int calculateZPeriodicCrossing(const Coords& coords_initial, const Coords& coords_dest);
		void clearOccupancy(const Coords& coords);
		int getHeight();
		int getLength();
		int getNumSites();
		Coords getRandomCoords();
		int getRandomX();
		int getRandomY();
		int getRandomZ();
		int getSiteIndex(const Coords& coords);
		vector<Site*>::iterator getSiteIt(const Coords& coords);
		double getUnitSize();
		int getWidth();
		bool isOccupied(const Coords& coords);
		bool isXPeriodic();
		bool isYPeriodic();
		bool isZPeriodic();
		void outputLatticeOccupancy();
		void setOccupied(const Coords& coords);
		bool setSitePointers(vector<Site*> input_ptrs);
    protected:

    private:
		bool Enable_periodic_x;
		bool Enable_periodic_y;
		bool Enable_periodic_z;
		int Length; // nm
		int Width; // nm
		int Height; // nm
		double Unit_size; // nm
		vector<Site*> site_ptrs;
		mt19937* gen;
};

#endif // LATTICE_H
