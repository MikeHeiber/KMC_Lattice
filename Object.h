// Copyright (c) 2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef OBJECT_H
#define OBJECT_H

#include "Utils.h"
#include <list>
#include <memory>

// Forward declaration of the Event class is needed by the compiler.
class Event;

//! \brief This base class contains the basic properties of a KMC simulation object and the functions needed 
//! to interact with it.
//! \details This base class is designed to work with the Simulation class to construct a KMC simulation. This 
//! base class is intended to be extended to create classes that represent specific types of objects. 
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2018
class Object{
    public:
		//! Default virtual destructor needed by the base class.
        virtual ~Object();

		//! Default constructor that creates an empty Object object.
        Object();

		//! \brief Constructor that creates and initializes a usable Object object.
		//! \param time is the current simulation time.
		//! \param tag_num is the specified tag ID number that will be used to label the Object.
		//! \param start_coords is the Coords struct that defines the starting coordinates of the Object.
        Object(const double time,const int tag_num,const Coords& start_coords);

		//! \brief Calculates the displacement of the object from its starting coordinates in lattice units.
		//! \details The function accounts for when periodic boundaries are crossed to determine the real 
		//! displacement distance.
        double calculateDisplacement() const;

		//! \brief Gets the current coordinates of the Object.
		//! \returns a Coords struct that represents the coordinates of the object.
        Coords getCoords() const;

		//! \brief Gets the simulation creation time of the Object.
		//! \return The simulation time value at which the object was created in units of seconds.
        double getCreationTime() const;
		
		//! \brief Gets the event list iterator for the event that is associated with the object.
		//! \details The event list iterator points to an Event pointer in the events list within the Simulation
		//! class, and the Event pointer points to a specific derived Event class stored in the derived Simulation
		//! class.
        std::list<Event*>::iterator getEventIt() const;

		//! \brief Gets the name of the Object class.
		//! \return "Object" when called on the base class.
        virtual std::string getObjectType() const;

		//! \brief Gets the tag id number of the Object.
		//! \warning This tag id number may not be unique between objects that are of different derived object classes.
        int getTag() const;

		//! \brief Increments the dx parameter, which is to be used when the Object crosses an x-direction periodic boundary.
		//! \param num is the input increment amount.
        void incrementDX(const int num);

		//! \brief Increments the dy parameter, which is to be used when the Object crosses a y-direction periodic boundary.
		//! \param num is the input increment amount.
        void incrementDY(const int num);

		//! \brief Increments the dz parameter, which is to be used when the Object crosses a z-direction periodic boundary.
		//! \param num is the input increment amount.
        void incrementDZ(const int num);

		//! \brief Resets the initial coordinates of the Object
		//! \details The dx, dy, and dz member variables that keep track of adjustments for crossing periodic boundaries are also reset to zero.
		//! \param input_coords is the Coords struct that designates the input coordinates and will be copied to the coords_initial member variable.
		void resetInitialCoords(const Coords& input_coords);

		//! \brief Sets the coordinates of the Object
		//! \param input_coords is the Coords struct that designates the input coordinates that will be copied to the coords_current member variable.
        void setCoords(const Coords& input_coords);

		//! \brief Sets the iterator that points to a specific entry in the events list within the Simulation class.
		//! \param input_it is the input event list iterator.
        void setEventIt(const std::list<Event*>::iterator input_it);
    private:
        static const std::string object_type_base;
        double time_created = 0;
        int tag = -1;
        // Current coords
		Coords coords_current = {-1,-1,-1};
        // Initial coords
        Coords coords_initial = { -1,-1,-1 };
		std::list<Event*>::iterator event_it;
        // Accounts for passes across periodic boundaries
        int dx = 0;
        int dy = 0;
        int dz = 0;
};

#endif // OBJECT_H
