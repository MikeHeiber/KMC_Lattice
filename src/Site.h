// Copyright (c) 2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef SITE_H
#define SITE_H

// Forward declaration of the Object class is needed by the compiler.
class Object;

//! \brief This base class contains the basic properties of a lattice site and the functions needed to interact with it.
//! \details This base class is designed to be used by the Lattice class to construct a lattice that will be used by a KMC simulation. 
//! This class is designed to for sites to have single occupancy, but multiple occupancy could potentially be implemented in a derived class.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2018
class Site {
public:
	//! Default virtual destructor needed by the base class.
	virtual ~Site();

	//! Default constructor that creates an empty Object object.
	Site();

	//! \brief Clears the occupancy of the site.
	//! \details This function also sets the Object pointer to nullptr.
	void clearOccupancy();

	//! Gets the pointer to the Object object that occupies the site.
	Object* getObjectPtr() const;

	//! \brief Checks whether the site is occupied or not.
	//! \return true if the site occupied.
	//! \return false if the site is unoccupied.
	bool isOccupied() const;

	//! \brief Sets the pointer to the occupying Object
	//! \details Also sets the site to an occupied state.
	void setObjectPtr(Object* input_ptr);

	//! Sets the site to an occupied state.
	void setOccupied();

private:
	bool occupied = false;
	Object* object_ptr = nullptr;
};

#endif // SITE_H
