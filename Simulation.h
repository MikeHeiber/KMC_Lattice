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
        virtual bool checkFinished()=0;
        virtual bool executeNextEvent()=0;
        int getN_events_executed();
        int getId();
        double getTime();
        void initializeSimulation(const Parameters_Simulation& params,const int id);
    protected:
        // Functions
        list<unique_ptr<Event>>::iterator addEvent(unique_ptr<Event>& event_ptr);
        void addObject(unique_ptr<Object>& object_ptr);
        void addSite(unique_ptr<Site>& site_ptr);
        int calculateDX(const int x,const int i);
        int calculateDY(const int y,const int j);
        int calculateDZ(const int z,const int k);
        list<unique_ptr<Event>>::iterator chooseNextEvent();
        int getNumSites();
        Coords getRandomCoords();
        vector<unique_ptr<Site>>::iterator getSiteIt(const Coords& coords);
        int getTemperature();
        double getUnitSize();
        void incrementTime(const double added_time);
        bool isOccupied(const Coords& coords);
        bool loggingEnabled();
        void logMSG(const stringstream& msg);
        void moveObject(const list<unique_ptr<Object>>::iterator object_it,const Coords& dest_coords);
        void removeObject(const list<unique_ptr<Object>>::iterator object_it);
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
        vector<unique_ptr<Site>> lattice;
        list<unique_ptr<Event>> events;
        list<unique_ptr<Object>> objects;
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
