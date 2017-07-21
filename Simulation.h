// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Utils.h"
#include "Lattice.h"
#include "Object.h"
#include "Event.h"
#include "Site.h"
#include <list>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

struct Parameters_Simulation{
    // General Parameters
    bool Enable_logging;
    bool Enable_periodic_x;
    bool Enable_periodic_y;
    bool Enable_periodic_z;
    int Length;
    int Width;
    int Height;
    double Unit_size; // nm
    int Temperature; // Kelvin
    // First reaction method parameters
    int Recalc_cutoff;
    // Output files
    ofstream* Logfile;
};

class Simulation{
    public:
        // Functions
		Simulation();
		void init(const Parameters_Simulation& params, const int id);
        virtual ~Simulation();
        virtual bool checkFinished()=0;
        virtual bool executeNextEvent()=0;
        long int getN_events_executed() const;
        int getId() const;
		int getTemp() const;
        double getTime() const;
		bool isLoggingEnabled() const;
		double rand01();
		void setGeneratorSeed(int seed);
    protected:
        // Random number generator
        mt19937 gen;
        // Output Files
        ofstream* Logfile;
        // Functions
        list<Event*>::iterator addEvent(Event* event_ptr);
        list<Object*>::iterator addObject(Object* object_ptr);
        bool checkMoveEventValidity(const Coords& coords_initial,const int i,const int j,const int k);
        list<Event*>::iterator chooseNextEvent();
        vector<list<Object*>::iterator> findRecalcNeighbors(const Coords& coords);
        vector<list<Object*>::iterator> getAllObjectIts();
        void moveObject(const list<Object*>::iterator object_it,const Coords& dest_coords);
        void removeEvent(Event* event_ptr);
        void removeObject(const list<Object*>::iterator object_it);
        void removeObjectItDuplicates(vector<list<Object*>::iterator>& object_its);
        void setEvent(const list<Event*>::iterator event_it,Event* event_ptr);
        void updateTime(const double new_time);
		// Data Structures
		Lattice lattice;
    private:
        int Id;
        // General Parameters
        bool Enable_logging;
        int Temperature; // Kelvin
        // First Reaction Method Parameters
        int Recalc_cutoff;
        // Data Structures
        list<Object*> objects;
        list<Event*> events;
        // Counters
        double Time = 0;
        int N_objects = 0;
        int N_objects_created = 0;
        long int N_events_executed = 0;
        // Functions
};

#endif // SIMULATION_H
