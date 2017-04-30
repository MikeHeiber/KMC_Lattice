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
        // Functions
        list<unique_ptr<Event>>::iterator addEvent(unique_ptr<Event>& event_ptr);
        list<unique_ptr<Object>>::iterator addObject(unique_ptr<Object>& object_ptr);
        void addSite(unique_ptr<Site>& site_ptr);
        Coords calculateDestinationCoords(const Coords& coords_initial,const int i,const int j,const int k);
        int calculateDX(const int x,const int i);
        int calculateDY(const int y,const int j);
        int calculateDZ(const int z,const int k);
        bool checkMoveEventValidity(const Coords& coords_initial,const int i,const int j,const int k);
        list<unique_ptr<Event>>::iterator chooseNextEvent();
        vector<list<unique_ptr<Object>>::iterator> findRecalcNeighbors(const Coords& coords);
        int getHeight();
        int getLength();
        int getNumSites();
        Coords getRandomCoords();
        int getSiteIndex(const Coords& coords);
        vector<unique_ptr<Site>>::iterator getSiteIt(const Coords& coords);
        int getTemperature();
        double getUnitSize();
        int getWidth();
        void incrementTime(const double added_time);
        bool isOccupied(const Coords& coords);
        bool isXPeriodic();
        bool isYPeriodic();
        bool isZPeriodic();
        bool loggingEnabled();
        void logMSG(const ostringstream& msg);
        void moveObject(const list<unique_ptr<Object>>::iterator object_it,const Coords& dest_coords);
        void outputLatticeOccupancy();
        void removeObject(const list<unique_ptr<Object>>::iterator object_it);
        void removeObjectItDuplicates(vector<list<unique_ptr<Object>>::iterator>& object_its);
        void setEvent(const list<unique_ptr<Event>>::iterator event_it,unique_ptr<Event>& event_ptr);
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
        // Output Files
        ofstream * Logfile;
        // Data Structures
        vector<unique_ptr<Site>> lattice;
        list<unique_ptr<Object>> objects;
        list<unique_ptr<Event>> events;
        // Counters
        double Time;
        int N_objects;
        int N_objects_created;
        int N_events_executed;
        // Functions
};

#endif // SIMULATION_H
