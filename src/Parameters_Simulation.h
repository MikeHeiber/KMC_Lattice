// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef KMC_LATTICE_PARAMETERS_SIMULATION_H
#define KMC_LATTICE_PARAMETERS_SIMULATION_H

#include "Parameters_Lattice.h"
#include <fstream>
#include <iostream>

namespace KMC_Lattice {

	//! \brief This class contains all of the main input parameters needed by the Simulation class.
	//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
	//! \author Michael C. Heiber
	//! \date 2017-2019
	class Parameters_Simulation {
	public:
		// Member variables
		Parameters_Lattice Params_lattice;
		//! Determines whether logging to a logfile during the simulation will be enabled or not.
		bool Enable_logging;
		//! Defines the desired temperature of the simulation in Kelvin.
		int Temperature;
		//! Determines whether the first reaction method will be used or not.
		bool Enable_FRM;
		//! Determines whether the selective recalculation method will be used or not.
		bool Enable_selective_recalc;
		//! Defines the desired event recalculation cutoff radius for the simulation in nm.
		int Recalc_cutoff;
		//! Determines whether the full recalculation method will be used or not.
		bool Enable_full_recalc;
		//! Defines the desired output file stream pointer to the logfile.
		std::ofstream* Logfile = nullptr;

		// Functions
		//! Default virtual destructor.
		virtual ~Parameters_Simulation();

		//! Default constructor that creates an empty object.
		Parameters_Simulation();

		//! \brief Checks the validity of the parameter member variables
		//! return true if the member variable parameters are valid
		//! return false if the member variable parameters are invalid
		bool checkParameters() const;
	};
}

#endif // KMC_LATTICE_PARAMETERS_SIMULATION_H
