// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Utils.h"
#include "Object.h"
#include "Event.h"
#include "Site.h"
#include <list>
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
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
        virtual ~Simulation();
        virtual bool checkFinished()=0;
        virtual bool executeNextEvent()=0;
        int getN_events_executed();
        int getId();
        double getTime();
        void initializeSimulation(const Parameters_Simulation& params,const int id);
    protected:
        // Random number generator
        mt19937 gen;
        // Output Files
        ofstream* Logfile;
        // Functions
        list<Event*>::iterator addEvent(Event* event_ptr);
        list<Object*>::iterator addObject(Object* object_ptr);
        void addSite(Site* site_ptr);
        Coords calculateDestinationCoords(const Coords& coords_initial,const int i,const int j,const int k);
        int calculateDX(const int x,const int i);
        int calculateDY(const int y,const int j);
        int calculateDZ(const int z,const int k);
        int calculateLatticeDistanceSquared(const Coords& coords_start,const Object& object_target);
        bool checkMoveEventValidity(const Coords& coords_initial,const int i,const int j,const int k);
        list<Event*>::iterator chooseNextEvent();
        vector<list<Object*>::iterator> findRecalcNeighbors(const Coords& coords);
        vector<list<Object*>::iterator> getAllObjectIts();
        int getHeight();
        int getLength();
        int getNumSites();
        Coords getRandomCoords();
        int getRandomX();
        int getRandomY();
        int getRandomZ();
        int getSiteIndex(const Coords& coords);
        vector<Site*>::iterator getSiteIt(const Coords& coords);
        int getTemperature();
        double getUnitSize();
        int getWidth();
        bool isOccupied(const Coords& coords);
        bool isXPeriodic();
        bool isYPeriodic();
        bool isZPeriodic();
        bool loggingEnabled();
        void moveObject(const list<Object*>::iterator object_it,const Coords& dest_coords);
        void outputLatticeOccupancy();
        void removeEvent(Event* event_ptr);
        void removeObject(const list<Object*>::iterator object_it);
        void removeObjectItDuplicates(vector<list<Object*>::iterator>& object_its);
        void setEvent(const list<Event*>::iterator event_it,Event* event_ptr);
        void updateTime(const double new_time);
    private:
        int Id;
        // General Parameters
        bool Enable_logging;
        bool Enable_periodic_x;
        bool Enable_periodic_y;
        bool Enable_periodic_z;
        int Length; // nm
        int Width; // nm
        int Height; // nm
        double Unit_size; // nm
        int Temperature; // Kelvin
        // First Reaction Method Parameters
        int Recalc_cutoff;
        // Data Structures
        vector<Site*> lattice;
        list<Object*> objects;
        list<Event*> events;
        // Counters
        double Time;
        int N_objects;
        int N_objects_created;
        int N_events_executed;
        // Functions
};

#endif // SIMULATION_H
