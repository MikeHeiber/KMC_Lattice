// Copyright (c) 2017-2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

# include "Parameters_Simulation.h"

using namespace std;

namespace KMC_Lattice {

	Parameters_Simulation::~Parameters_Simulation() {

	}

	Parameters_Simulation::Parameters_Simulation() {

	}

	bool Parameters_Simulation::checkParameters() const {
		if (!Params_lattice.checkParameters()) {
			return false;
		}
		if (!(Temperature > 0)) {
			cout << "Error! The temperature must be greater than zero." << endl;
			return false;
		}
		int KMC_algs = 0;
		if (Enable_FRM) {
			KMC_algs++;
		}
		if (Enable_selective_recalc) {
			KMC_algs++;
		}
		if (Enable_full_recalc) {
			KMC_algs++;
		}
		if (KMC_algs > 1) {
			cout << "Error! Only one of the first reaction method, the selective recalculation method, or the full recalculation method can be enabled." << endl;
			return false;
		}
		if (KMC_algs == 0) {
			cout << "Error! One of the first reaction method, the selective recalculation method, or the full recalculation method must be enabled." << endl;
			return false;
		}
		if (Enable_selective_recalc && !(Recalc_cutoff > 0)) {
			cout << "Error! The event recalculation cutoff radius must be greater than zero." << endl;
			return false;
		}
		if (Enable_logging && !(Logfile->good() && Logfile->is_open() ) ){
			cout << "Error! The logfile filestream is not open or has an error." << endl;
			return false;
		}
		return true;
	}

}
