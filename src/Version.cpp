// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software package.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Version.h"

using namespace std;

namespace KMC_Lattice {

	Version Current_version("2.1.0-beta.1");

	Version::Version() { }

	Version::Version(const std::string& version_str) {
		string str = version_str;
		// Separate out main version info from possible prerelease version info
		string main_str = str.substr(0, str.find('-'));
		// Parse main version string
		major_num = stoi(main_str.substr(0, main_str.find('.')));
		main_str.erase(0, main_str.find('.') + 1);
		if (main_str.find('.') != string::npos) {
			minor_num = stoi(main_str.substr(0, main_str.find('.')));
			main_str.erase(0, main_str.find('.') + 1);
			rev_num = stoi(main_str);
		}
		else {
			minor_num = stoi(main_str);
			rev_num = 0;
		}
		// Check if prerelease info is present
		if (str.find('-') != string::npos) {
			// Separate out prerelease info
			str.erase(0, str.find('-') + 1);
			// Parse prerelease version string
			prerelease_name = str.substr(0, str.find('.'));
			str.erase(0, str.find('.') + 1);
			prerelease_num = stoi(str);
		}
		// Check that final parsed version information is valid
		if ((int)prerelease_name.length() == 0) {
			if (major_num < 0 || minor_num < 0 || rev_num < 0 || major_num + minor_num + rev_num < 1) {
				cout << "Error! Input version string, '" << version_str << "', does not have the correct format." << endl;
				throw invalid_argument("Error! Input version string does not have the correct format.");
			}
		}
		else {
			if (major_num < 0 || minor_num < 0 || rev_num < 0 || major_num + minor_num + rev_num < 1 || (prerelease_name.compare("alpha") != 0 && prerelease_name.compare("beta") != 0 && prerelease_name.compare("rc") != 0) || prerelease_num < 1) {
				cout << "Error! Input version string, '" << version_str << "', does not have the correct format." << endl;
				throw invalid_argument("Error! Input version string does not have the correct format.");
			}
		}
	}

	std::string Version::getVersionStr() const {
		if ((int)prerelease_name.length() == 0) {
			return to_string(major_num) + "." + to_string(minor_num) + "." + to_string(rev_num);
		}
		else {
			return to_string(major_num) + "." + to_string(minor_num) + "." + to_string(rev_num) + "-" + prerelease_name + "." + to_string(prerelease_num);
		}
	}

	bool Version::operator == (const Version& input) const {
		return major_num == input.major_num	&& minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare(input.prerelease_name) == 0 && prerelease_num == input.prerelease_num;
	}

	bool Version::operator != (const Version& input) const {
		return !(*this == input);
	}

	bool Version::operator < (const Version& input) const {
		if (major_num < input.major_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num < input.minor_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num < input.rev_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("alpha") == 0 && input.prerelease_name.compare("beta") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("alpha") == 0 && input.prerelease_name.compare("rc") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("beta") == 0 && input.prerelease_name.compare("rc") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare(input.prerelease_name) == 0 && prerelease_num < input.prerelease_num) {
			return true;
		}
		return false;
	}

	bool Version::operator > (const Version& input) const {
		if (major_num > input.major_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num > input.minor_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num > input.rev_num) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("beta") == 0 && input.prerelease_name.compare("alpha") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("rc") == 0 && input.prerelease_name.compare("alpha") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare("rc") == 0 && input.prerelease_name.compare("beta") == 0) {
			return true;
		}
		if (major_num == input.major_num && minor_num == input.minor_num && rev_num == input.rev_num && prerelease_name.compare(input.prerelease_name) == 0 && prerelease_num > input.prerelease_num) {
			return true;
		}
		return false;
	}

	bool Version::operator <= (const Version& input) const {
		return !(*this > input);
	}

	bool Version::operator >= (const Version& input) const {
		return !(*this < input);
	}

	std::ostream& operator << (std::ostream& stream, const Version& input) {
		stream << input.major_num << "." << input.minor_num << "." << input.rev_num;
		if ((int)input.prerelease_name.length() > 0) {
			stream << "-" << input.prerelease_name << "." << input.prerelease_num;
		}
		return stream;
	}
}
