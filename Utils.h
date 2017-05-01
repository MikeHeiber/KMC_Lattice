// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef UTILS_H
#define UTILS_H

#include "mpi.h"
#include <vector>
#include <random>
#include <cmath>
#include <memory>

using namespace std;

struct Coords{
    int x;
    int y;
    int z;
};

// Useful Constants
static constexpr double K_b = 8.61733035e-5;
static constexpr double Elementary_charge = 1.602176621e-19; // C
static constexpr double Vacuum_permittivity =  8.854187818e-12; // C/Vm
static constexpr double Plank = 6.626070040e-34;
static constexpr double Light_speed = 2.99792458e8;
static constexpr double Pi = 3.14159265359;
static constexpr double Coulomb_constant = 8.987551787e9; // N m^2 C^-2

double array_avg(const double data[],const int size);
double array_stdev(const double data[],const int size);
vector<double> calculateAverageVector(const vector<double>& input_vector,const int procid,const int nproc);
void createExponentialDOSVector(vector<double>& data,const double mode,const double urbach_energy);
void createGaussianDOSVector(vector<double>& data,const double mean,const double stdev);
double intpow(const double base,const int exponent);
double vector_avg(const vector<int>& dataset);
double vector_avg(const vector<double>& dataset);
double vector_stdev(const vector<int>& dataset);
double vector_stdev(const vector<double>& dataset);

template<typename Derived,typename Base,typename Del>
unique_ptr<Derived,Del> static_unique_ptr_cast(unique_ptr<Base,Del>&& p){
    auto d = static_cast<Derived*>(p.release());
    return unique_ptr<Derived,Del>(d,std::move(p.get_deleter()));
}

#endif // UTILS_H
