#ifndef SIMULATION_H
#define SIMULATION_H

#include "Utils.h"
#include "Object.h"
#include "Event.h"
#include "Site.h"
#include <list>
#include <vector>
#include <memory>
#include <fstream>
#include <cmath>
#include <boost/random.hpp>

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
    bool Enable_recalc;
    int Recalc_cutoff;
    // Output files
    ofstream * Logfile;
};

class Simulation{
    public:
        // Functions
        virtual ~Simulation();
        //Simulation(const Parameters_Simulation& params,const int id);
        void addObject(unique_ptr<Object>& object_ptr);
        virtual void calculateEvents();
        virtual bool checkFinished();
        list<unique_ptr<Event>>::iterator chooseNextEvent();
        virtual bool executeNextEvent();
        int getNumSites();
        int getN_events_executed();
        int getId();
        int getTemperature();
        double getTime();
        double getUnitSize();
        void initializeSimulation(const Parameters_Simulation& params,const int id);
        void removeObject(const list<unique_ptr<Object>>::iterator object_it);
    protected:
        vector<unique_ptr<Site>> lattice;
        list<unique_ptr<Event>> events;
        list<unique_ptr<Object>> objects;
        inline int calculateDX(const int x,const int i);
        inline int calculateDY(const int y,const int j);
        inline int calculateDZ(const int z,const int k);
        inline vector<unique_ptr<Site>>::iterator getSiteIt(const Coords& coords);
        void setEvent(const list<unique_ptr<Event>>::iterator event_it,unique_ptr<Event> event_ptr);
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
        bool Enable_recalc;
        int Recalc_cutoff;
        // Output Files
        ofstream * Logfile;
        // Data Structures
        // Counters
        double Time;
        int N_objects;
        int N_objects_created;
        int N_events_executed;
        // Random number generator
        boost::mt19937 gen;
        // Functions
        vector<list<unique_ptr<Object>>::iterator> findRecalcNeighbors(const Coords& coords);
        inline int getSiteIndex(const Coords& coords);
        inline void removeObjectItDuplicates(vector<list<unique_ptr<Object>>::iterator>& object_its);
};

#endif // SIMULATION_H
