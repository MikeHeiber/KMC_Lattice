// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace std;

//! \brief This simple struct contains Cartesian coordinates specified by integers x,y,z.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
struct Coords{
	//! The x Cartesian coordinate.
    int x;
	//! The y Cartesian coordinate.
    int y;
	//! The z Cartesian coordinate.
    int z;

	//! \brief Sets the x,y,z coordinates using the input values xval, yval, and zval.
	//! \param xval is the input x value.
	//! \param yval is the input y value.
	//! \param zval is the input z value.
    void setXYZ(const int xval,const int yval,const int zval){
        x = xval;
        y = yval;
        z = zval;
    }
	//! \brief Defines the conditions when two Coords structs are deemed to be equal.
	//! \param rhs is the right hand side Coords struct of the comparison operator.
	bool operator==(const Coords& rhs) const{
		return (x == rhs.x && y == rhs.y && z == rhs.z);
	}
	//! \brief Defines the conditions when two Coords structs are deemed to be unequal.
	//! \param rhs is the right hand side Coords struct  of the comparison operator.
	bool operator!=(const Coords& rhs) const {
		return (x != rhs.x || y != rhs.y || z != rhs.z);
	}
};

//! \brief This namespace provides useful constants and utility functions.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
namespace Utils {

	// Scientifc Constants
	static constexpr double K_b = 8.61733035e-5;
	static constexpr double Elementary_charge = 1.602176621e-19; // C
	static constexpr double Vacuum_permittivity = 8.854187818e-12; // C/Vm
	static constexpr double Plank = 6.626070040e-34;
	static constexpr double Light_speed = 2.99792458e8;
	static constexpr double Pi = 3.14159265359;
	static constexpr double Coulomb_constant = 8.987551787e9; // N m^2 C^-2

	//! \brief Creates a vector of doubles that has a custom asymmetric distribution with an exponential tail.
	//! \details The created distribution is Gaussian in the positive direction relative to the mode and exponential in 
	//! the negative direction. On the Gaussian side, the standard deviation is calculated relative to the urbach energy
	//! chosen for the exponential side, so that the distribution function is continuous.
	//! \param data is the data vector where the random numbers will be placed, which must be preallocated to the desired size.
	//! \param mode is the value of the peak of the distribution.
	//! \param urbach_energy is the parameter that detemines the shape of the exponential tail side of the distribution.
	//! \param gen is a Mersenne twister random number generator used to randomly draw numbers from the distribution.
	void createExponentialDOSVector(vector<double>& data, const double mode, const double urbach_energy, mt19937& gen);

	//! \brief Creates a vector of doubles that has a Gaussian distribution.
	//! \param data is the data vector where the numbers will be placed, which must be preallocated to the desired size.
	//! \param mean is the position of the peak and center of the distribution.
	//! \param stdev is the standard deviation of the distribution, which defines the width of the peak.
	//! \param gen is a Mersenne twister random number generator used to randomly draw numbers from the distribution.
	void createGaussianDOSVector(vector<double>& data, const double mean, const double stdev, mt19937& gen);

	//! \brief Extracts a boolean value from a string containing "true" or "false".
	//! \param input is the input string.
	//! \param error_status is an input boolean that is used to indicate an error with the import process.
	//! \return true if the input string is "true".
	//! \return flase if the input string is "flase".
	bool importBooleanParam(const string& input, bool& error_status);

	//! \brief Uses MPI to calculate the element-wise average vector from separate vectors coming from different processors.
	//! \details Each processor calls this function and sends an input vector.  Upon function return, processor 0 
	//! receives the average vector and all of the other processors receive an empty vector.
	//! \param input_vector is the input data from the processor calling the function.
	//! \return A vector that is the element-wise average of all input vectors from each processor, when called on processor 0.
	//! \return An empty vector when called on other processors.
	vector<double> MPI_calculateVectorAvg(const vector<double>& input_vector);

	//! \brief Uses MPI to calculate the element-wise sum vector from separate vectors coming from different processors.
	//! \details Each processor calls this function and sends an input vector.  Upon function return, processor 0 
	//! receives the sum vector and all of the other processors receive an empty vector.
	//! \param input_vector is the input data from the processor calling the function.
	//! \return A vector that is the element-wise sum of all input vectors from each processor, when called on processor 0.
	//! \return An empty vector when called on other processors.
	vector<double> MPI_calculateVectorSum(const vector<double>& input_vector);

	//! \brief Uses MPI to calculate the element-wise sum vector from separate vectors coming from different processors.
	//! \details Each processor calls this function and sends an input vector.  Upon function return, processor 0 
	//! receives the sum vector and all of the other processors receive an empty vector.
	//! \param input_vector is the input data from the processor calling the function.
	//! \return A vector that is the element-wise sum of all input vectors from each processor, when called on processor 0.
	//! \return An empty vector when called on other processors.
	vector<int> MPI_calculateVectorSum(const vector<int>& input_vector);

	//! \brief Uses MPI to gather vectors from separate processors to build one big vector containing all of the data.
	//! \details Each processor calls this function and sends an input vector.  Upon function return, processor 0
	//! receives the large data vector and all of the othe rprocessors receive an empty vector.
	//! \param input_vector is the input data from the processor calling the function.
	//! \return A vector that is a concatenation of all input vectors from each processor, when called on processor 0.
	//! \return An empty vector when called on other processors.
	vector<double> MPI_gatherVectors(const vector<double>& input_vector);

	//! \brief Removes leading and trailing spaces surrounding a string.
	//! \param str is the input string
	//! \returns a new string that will have the surrounding spaces removed.
	string removeWhitespace(const string& str);

	// Template functions

	//! \brief This template function calculates and returns the average value in double format when given an array of numerical datatypes.
	//! \param data is the array of numerical data.
	//! \param array_size is the size of the input data array.
	//! \return The average of the data set in double format.
	template<typename T>
	double array_avg(const T data[], const int array_size) {
		double sum = 0;
		for (int i = 0; i < array_size; i++) {
			sum += data[i];
		}
		return sum / array_size;
	}

	//! \brief This template function calculates and returns the standard deviation in double format when given an array of numerical datatypes.
	//! \param data is the array of numerical data.
	//! \param array_size is the size of the input data array.
	//! \return The standard deviation of the data set in double format.
	template<typename T>
	double array_stdev(const T data[], const int array_size) {
		double sum = 0;
		double avg = array_avg(data, array_size);
		for (int i = 0; i < array_size; i++) {
			sum += (data[i] - avg)*(data[i] - avg);
		}
		return sqrt(sum / (array_size - 1));
	}

	//! \brief This template function calculates and returns the results of an integer power operation on a base numerical datatype.
	//! \param base is the base of the integer power operation and can be any numerical type.
	//! \param exponent is the integer exponent for the integer power operation.
	//! \return The result of the integer power operation in whichever datatype format was input with the base.
	template<typename base_type>
	base_type intpow(const base_type base, const int exponent) {
		base_type result;
		if (exponent == 0) {
			result = 1;
			return result;
		}
		int exponent_abs;
		if (exponent < 0) {
			exponent_abs = -exponent;
		}
		else {
			exponent_abs = exponent;
		}
		result = base;
		for (int i = 1; i < exponent_abs; i++) {
			result *= base;
		}
		if (exponent < 0) {
			result = 1.0 / result;
		}
		return result;
	}

	//! \brief This template function efficienctly removes the duplicate entries from an input vector.
	//! \details This algorithm allow efficient removal of duplicate vector objects when > or < comparison operators do not exist.
	//! \param vec is the input vector to be operated on.
	template<typename T>
	void removeDuplicates(vector<T>& vec) {
		for (auto it1 = vec.begin(); it1 < vec.end() - 1; ++it1) {
			for (auto it2 = it1 + 1; it2 < vec.end(); ++it2) {
				if (*it2 == *it1) {
					*it2 = *(it2 - 1);
				}
			}
		}
		auto it3 = unique(vec.begin(), vec.end());
		vec.resize(distance(vec.begin(), it3));
	}

	//! \brief This template function calculates and returns the average value in double format when given a vector of numerical datatypes.
	//! \param data is the vector of numerical data.
	//! \return The average of the data set in double format.
	template<typename T, typename A>
	double vector_avg(const vector<T, A>& data) {
		double sum = 0;
		for (auto it = data.begin(); it != data.end(); ++it) {
			sum += *it;
		}
		return sum / data.size();
	}

	//! \brief This template function calculates and returns the standard deviation in double format when given a vector of numerical datatypes.
	//! \param data is the vector of numerical data.
	//! \return The standard deviation of the data set in double format.
	template<typename T, typename A>
	double vector_stdev(const vector<T, A>& data) {
		double sum = 0;
		double avg = vector_avg(data);
		for (auto it = data.begin(); it != data.end(); ++it) {
			sum += (*it - avg)*(*it - avg);
		}
		return sqrt(sum / (data.size() - 1));
	}
}

#endif // UTILS_H
