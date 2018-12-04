// Copyright (c) 2017-2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef KMC_LATTICE_EVENT_H
#define KMC_LATTICE_EVENT_H

#include "Simulation.h"
#include "Utils.h"
#include <list>

namespace KMC_Lattice {

	// Forward declaration of the Object and Simulation classes are needed by the compiler
	class Object;
	class Simulation;

	//! \brief This base class contains the basic properties of a KMC simulation event and the functions needed to
	//! interact with it.
	//! \details This base class is designed to work with the Simulation class to construct a KMC simulation. This 
	//! base class is intended to be extended to create classes that represent specific types of events. 
	//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
	//! \author Michael C. Heiber
	//! \date 2017-2018
	class Event {
	public:
		//! Default virtual destructor needed by the base class.
		virtual ~Event();

		//! The default constructor that creates an empty Event object.
		Event();

		//! Constructor that creates an empty Event object pre-initilized with the Simulation pointer.
		//! \param sim_ptr is a pointer to the Simulation object that the Event is associated with.
		Event(Simulation* sim_ptr);

		//! \brief Calculates and sets the execution time of the event.
		//! \details The function accesses the random number generator and current simulation time from the
		//! Simulation object in order to calculate the execution time. When creating a new derived event class,
		//! one will often write a new calculateExecutionTime function that contains additional factors needed 
		//! to calculate the rate. This base class function can then be called within the new function to calculate
		//! the final execution time.
		//! \param rate is the rate of the process represented by the event in units of 1/s.
		void calculateExecutionTime(const double rate);

		//! Default virtual function that can be overloaded by derived Event classes.
		void calculateRateConstant(const double input_rate);

		//! \brief Gets the coordinates of the event destination site.
		//! \warning Some events may not have a destination site determined until execution and will thus
		//! not have a valid set of coordinates defined.
		//! \return The coordinates of the destination site if valid coordinates have been set.
		//! \return The coordinates (-1,-1,-1) if no valid destination site has been set.
		Coords getDestCoords() const;

		//! \brief Gets the name of event class.
		//! \return "Event" when called on the base class.
		virtual std::string getEventType() const;

		//! \brief Gets the currently planned execution time of the event.
		//! \return -1 if the event execution time has not yet been calculated.
		//! \return the currently planned execution time of the event in units of seconds.
		double getExecutionTime() const;

		//! \brief Gets a pointer to the Object object that is designated as the subject of the event.
		//! \warning Some events may not operate on an object and will thus not have a subject
		//! object associated with them.
		//! \return nullptr if a valid Object object has not been designated as the subject of the event.
		Object* getObjectPtr() const;

		//! \brief Gets a pointer to the Object object that is designated as the target of the event.
		//! \warning Some events may not have a valid target object assigned. A target object will only 
		//! be defined when the event represent a reaction of some type between two objects.
		//! \return nullptr if a valid Object object has not been designated as the target of the event.
		Object* getObjectTargetPtr() const;

		//! \brief Gets the stored rate constant of the event.
		//! \return -1 if the rate constant has not yet been calculated.
		//! \return the stored rate constant of the event in units of inverse seconds.
		double getRateConstant() const;

		//! \brief Sets the desination coordinates of the event.
		//! \param coords is the Coords struct that designates the input coordinates.
		void setDestCoords(const Coords& coords);

		//! \brief Sets the execution time of the event.
		//! \param time is the input time.
		//! \return true if the input time non-negative.
		//! \return false if the input is negative to indicate an error.
		bool setExecutionTime(const double time);

		//! \brief Sets the pointer to the Object object that is designated as the subject of the event.
		//! \param input_ptr is the input Object pointer.
		void setObjectPtr(Object* input_ptr);

		//! \brief Sets the pointer to the Object object that is designated as the target of the event.
		//! \param input_ptr is the input Object pointer.
		void setObjectTargetPtr(Object* input_ptr);

		//! \brief Sets the value of the rate constant for the event.
		//! \param val is the designated value of the rate constant.
		void setRateConstant(double val);

	protected:
		// Variables
		//! \brief Points to the Simulation object that the Event is asociated with.
		//! \details The pointer is used to access the current time in the simulation and the random number generator.
		Simulation* sim_ptr = nullptr;

		//! \brief Stores the rate constant of the particular event.
		double rate_constant = -1.0;
		// Functions
	private:
		// Variables and objects
		static const std::string event_type_base;
		double execution_time = -1.0;
		Object* object_ptr = nullptr;
		Object* object_target_ptr = nullptr;
		Coords coords_dest = { -1,-1,-1 };
	};

}

#endif // KMC_LATTICE_EVENT_H
