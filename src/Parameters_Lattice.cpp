// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

# include "Parameters_Lattice.h"

using namespace std;

namespace KMC_Lattice {

	Parameters_Lattice::~Parameters_Lattice() {

	}

	Parameters_Lattice::Parameters_Lattice() {

	}

	bool Parameters_Lattice::checkParameters() const {
		if (!(Length > 0) || !(Width > 0) || !(Height > 0)) {
			cout << "Error! All lattice dimensions must be greater than zero." << endl;
			return false;
		}
		if (!(Unit_size > 0)) {
			cout << "Error! The lattice unit size must be greater than zero." << endl;
			return false;
		}
		return true;
	}

}
