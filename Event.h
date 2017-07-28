// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef EVENT_H
#define EVENT_H

#include "Simulation.h"
#include "Utils.h"
#include <list>
#include <memory>
#include <ctime>

using namespace std;

// Forward declaration of the Object and Simulation classes are needed by the compiler
class Object;
class Simulation;

//! \brief This base class contains the basic properties of a KMC simulation event and the functions needed to interact with it.
//! \details This base class is designed to work with the Simulation class to construct a KMC simulation.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
class Event{
    public:
		//! Default virtual destructor needed by the base class.
        virtual ~Event();

		//! Default constructor that creates an empty Event object.
        Event();

		//! \brief Calculates and sets the execution time of the event.
		//! \details The function accesses the random number generator and current simulation time from the Simulation object in order to
		//! to calculate the execution time.
		//! \param rate is the rate of the process represented by the event in units of 1/s.
		//! \param sim_ptr is a pointer to a Simulation object.
        void calculateExecutionTime(const double rate, Simulation* sim_ptr);

		//! \brief Gets the coordinates of the event destination site.
		//! \return The coordinates of the destination site if valid coordinates have been set.
		//! \return The coordinates (-1,-1,-1) if no valid destination site has been set.
        Coords getDestCoords() const;

		//! \brief Gets the name of event class.
		//! \return "Event" when called on the base class.
        virtual string getName() const;

		//! \brief Gets a pointer to the Object object that is designated as the subject of the event.
		//! \return nullptr if a valid Object object has not been designated as the subject of the event.
        Object* getObjectPtr() const;

		//! \brief Gets a pointer to the Object object that is designated as the target of the event.
		//! \return nullptr if a valid Object object has not been designated as the target of the event.
        Object* getObjectTargetPtr() const;

		//! \brief Gets the currently planned execution time of the event.
		//! \return 0 if the event execution time has not yet been calculated.
        double getExecutionTime() const;

		//! \brief Sets the desination coordinates of the event.
		//! \param coords is the Coords struct that designates the input coordinates.
        void setDestCoords(const Coords& coords);

		//! \brief Sets the execution time of the event.
		//! \param time is the input time.
		//! \return true if the input time non-negative.
		//! \return false if the input is negative to indicating an error.
        bool setExecutionTime(const double time);

		//! \brief Sets the pointer to the Object object that is designated as the subject of the event.
		//! \param input_ptr is the input Object pointer
        void setObjectPtr(Object* input_ptr);

		//! \brief Sets the pointer to the Object object that is designated as the target of the event.
		//! \param input_ptr is the input Object pointer
        void setObjectTargetPtr(Object* input_ptr);

    protected:
        // Variables
        // Functions
    private:
        // Variables and objects
        static const string name_base;
        double execution_time = 0;
        Object* object_ptr = nullptr;
        Object* object_target_ptr = nullptr;
		Coords coords_dest = {-1,-1,-1};
};

#endif // EVENT_H
