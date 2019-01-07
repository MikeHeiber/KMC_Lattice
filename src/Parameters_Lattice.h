// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef KMC_LATTICE_PARAMETERS_LATTICE_H
#define KMC_LATTICE_PARAMETERS_LATTICE_H

#include <iostream>

namespace KMC_Lattice {

	//! \brief This class contains all of the main input parameters needed by the Lattice class.
	//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
	//! \author Michael C. Heiber
	//! \date 2017-2019
	class Parameters_Lattice {
	public:
		// Member variables
		//! Determines whether the x-direction periodic boundaries will be enabled. 
		bool Enable_periodic_x;
		//! Determines whether the y-direction periodic boundaries will be enabled. 
		bool Enable_periodic_y;
		//! Determines whether the z-direction periodic boundaries will be enabled. 
		bool Enable_periodic_z;
		//! Defines the desired x-direction size of the lattice.
		int Length;
		//! Defines the desired y-direction size of the lattice.
		int Width;
		//! Defines the desired z-direction size of the lattice.
		int Height;
		//! Defines the desired lattice unit size, which is used to convert lattice units into real space units.
		double Unit_size; // nm

		// Functions
		//! Default virtual destructor.
		virtual ~Parameters_Lattice();

		//! Default constructor that creates an empty object.
		Parameters_Lattice();

		//! \brief Checks that validity of the parameter member variables
		//! return true if the member variable parameters are valid
		//! return false if the member variable parameters are invalid
		bool checkParameters() const;
	};
}

#endif // KMC_LATTICE_PARAMETERS_LATTICE_H
