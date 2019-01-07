// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software package.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef KMC_LATTICE_VERSION_H
#define KMC_LATTICE_VERSION_H

#include <iostream>
#include <stdexcept>
#include <string>

namespace KMC_Lattice {

	class Version;

	extern Version Current_version;

	//! \brief This class contains the current version information and version comparison operators.
	//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
	//! \author Michael C. Heiber
	//! \date 2017-2019
	class Version {

	public:
		//! \brief Default constructor that creates an empty Version object.
		//! \warning An empty Version object should not be used until initialized.
		Version();

		//! \brief Standard constructor that creates an initialized Version object.
		//! \param version_str is the version string used to initialize the Version object.
		Version(const std::string& version_str);

		//! \brief Returns the version string representation of the Version object.
		std::string getVersionStr() const;

		//! \brief Defined equal to operator for comparing versions.
		bool operator == (const Version& input) const;

		//! \brief Defined not equal to operator for comparing versions.
		bool operator != (const Version& input) const;

		//! \brief Defined less than operator for comparing versions.
		bool operator < (const Version& input) const;

		//! \brief Defined greater than operator for comparing versions.
		bool operator > (const Version& input) const;

		//! \brief Defined less than or equal to operator for comparing versions.
		bool operator <= (const Version& input) const;

		//! \brief Defined greater than or equal to operator for comparing versions.
		bool operator >= (const Version& input) const;

		//! \brief Defined stream operator for streaming the version string.
		friend std::ostream& operator << (std::ostream& stream, const Version& input);

	protected:

	private:
		int major_num = -1;
		int minor_num = -1;
		int rev_num = -1;
		std::string prerelease_name = "";
		int prerelease_num = -1;
	};
}

#endif // KMC_LATTICE_VERSION_H
