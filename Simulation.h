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
#include <list>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

//! \brief This struct contains all of the main input parameters needed by the Simulation class.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
struct Parameters_Simulation{

	//! Determines whether logging to a logfile during the simulation will be enabled or not.
    bool Enable_logging; 
	//! Determines whether the x-direction periodic boundary conditions will be enabled or not in the Lattice.
    bool Enable_periodic_x;
	//! Determines whether the y-direction periodic boundary conditions will be enabled or not in the Lattice.
    bool Enable_periodic_y;
	//! Determines whether the z-direction periodic boundary conditions will be enabled or not in the Lattice.
    bool Enable_periodic_z;
	//! Defines the desired x-direction size of the Lattice.
    int Length;
	//! Defines the desired y-direction size of the Lattice.
    int Width;
	//! Defines the desired z-direction size of the Lattice.
    int Height;
	//! Defines the desired resolution of the Lattice in units of nm.
    double Unit_size;
	//! Defines the desired temperature of the simulation in Kelvin.
    int Temperature;
	//! Defines the desired event recalculation cutoff radius for the simulation in nm.
    int Recalc_cutoff;
	//! Defines the desired output file stream pointer to the logfile.
    ofstream* Logfile;
};

//! \brief This abstract base class contains the basic properties of a KMC simulation and the functions needed 
//! to interact with it.
//! \details This abstract base class must be extended using a derived simulation class.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
class Simulation{
    public:
		//! Default virtual destructor needed by the base class.
		virtual ~Simulation();

		//! \brief Default constructor that creates an empty Simulation object.
		//! \warning An empty Simulation object should not be used until the init function has been called.
		Simulation();

		//! \brief Initializes the Simulation object using the provided Parameters_Simulation struct containing 
		//! the input parameters.
		//! \param params is a Parameters_Simulation struct that contains all of the required
		//! parameters to initialize the Simulation object. 
		//! \param id is the processor ID number for the processor that is running the simulation.
		void init(const Parameters_Simulation& params, const int id);
        
		//! \brief Checks whether or not the simulation has finished.
		//! \details This is a pure virtual function in the base class that must be defined by any derived class.
		//! \return true if the simulation is finished.
		//! \return false if the simulation is not finished.
        virtual bool checkFinished() const = 0;

		//! \brief Executes the next event in the simulation.
		//! \details This is a pure virtual function in the base class that must be defined by any derived class.
		//! \return true if the next event is succesfully executed.
		//! \return false if execution of the next event is unsuccessful.
        virtual bool executeNextEvent() = 0;

		//! Gets the number of events that have been executed in the simulation.
        long int getN_events_executed() const;

		//! \brief Gets the processor ID number for the processor that is running the simulation.
		//! \details This is primarly used with MPI to differentiate between different simualtions running on 
		//! different cores.
        int getId() const;

		//! Gets the value of the temperature parameter.
		int getTemp() const;

		//! Get the current simulation time in units of seconds.
        double getTime() const;

		//! \brief Checks whether or not logging is enabled.
		//! \details This is primarily used for debugging purposes.
		//! \return true if logging is enabled.
		//! \return false if logging is disabled.
		bool isLoggingEnabled() const;

		//! Generates a uniform random number from 0 to 1, not including 0.
		double rand01();

		//! \brief Sets the random number generator seed.
		//! \details This is primarily used for testing with a set starting seed.
		void setGeneratorSeed(const int seed);
    protected:
        //! Mersenne Twister random number generator
        mt19937 gen;
        //! Pointer to an output file stream that is used to print log messages to a logfile when logging is enabled.
        ofstream* Logfile;
		//! The Lattice object represents a three-dimensional lattice, its boundary conditions, and its occupancy.
		Lattice lattice;
		//! The Error_found flag indicates whether or not there has been an error during one of the simulation operations.
		bool Error_found = false;

        //------ Functions

		//! \brief Adds a pointer to an Event object to the event list and returns the iterator to its position in the list.
		//! \param event_ptr is the input Event pointer.
		//! \return A list iterator that indicates where in the event list the newly added Event pointer is located.
        list<Event*>::iterator addEvent(Event* event_ptr);

		//! \brief Adds a pointer to an Object object to the object list.
		//! \param object_ptr is the input Object pointer.
        void addObject(Object* object_ptr);

		//! \brief Searches the event list and determines which event will be executed next.
		//! \details Chooses the event that has the smallest execution time.
		//! \return A list iterator points to an Event pointer in event list that has been selected to be executed next.
        list<Event*>::iterator chooseNextEvent();

		//! \brief Locates and returns a vector of pointers to all Object objects near the input coordinates within 
		//! the Recalc_cutoff radius.
		//! \param coords is the Coords struct that designates the input coordinates.
		//! \return a vector of Object pointers.
        vector<Object*> findRecalcNeighbors(const Coords& coords) const;

		//! \brief Returns a vector of pointers to all Object objects in the simulation.
        vector<Object*> getAllObjectPtrs() const;

		//! \brief Moves the designated object to the designated destination coordinates.
		//! \param object_ptr is an Object pointer to the object that is to be moved.
		//! \param dest_coords is the Coords struct that designates the coordinates where the object is to be moved.
        void moveObject(Object* object_ptr,const Coords& dest_coords);

		//! \brief Removes an Event pointer from the event list.
		//! \details The Event objects are allocated and maintained by the derived Simulation class and only the Event 
		//! pointers are stored in the base class. Removing the Event pointer does not delete the Event from the derived
		//! class, it only prevents the event from being executed in future simulation iterations.
		//! \param event_ptr is the Event pointer to be removed from the simulation.
        void removeEvent(Event* event_ptr);

		//! \brief Removes the Object pointer from the base simulation class.
		//! \details The Object objects are allocated and maintained by the derived Simulation class and only the Object 
		//! pointers are stored in the base class. Removing the Object pointer does not delete the Object from the derived
		//! class.  This function also calls the removeEvent function to remove the Event pointer associated with the 
		//! Object and also communites with the Lattice object to clear the occupancy of the Site where the Object was located.
		//! \param object_ptr is the Object pointer to be removed from the simulation.
        void removeObject(Object* object_ptr);

		//! \brief Overwrites the Event pointer in the event list indicated by the input iterator to the input Event pointer.
		//! \details This is used to update the Event associated with a particular object.
		//! \param event_it is the list iterator pointing to the Event pointer that is to be overwritten.
		//! \param event_ptr is the input Event pointer.
        void setEvent(const list<Event*>::iterator event_it,Event* event_ptr);

		//! \brief Updates the simulation time with the input time.
		//! \param input_time is the input time that will become the new current simulation time.
        void updateTime(const double input_time);
		
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
