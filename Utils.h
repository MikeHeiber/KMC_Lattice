#ifndef UTILS_H
#define UTILS_H

#include "mpi.h"
#include <vector>
#include <boost/random.hpp>
#include <cmath>

using namespace std;

struct Coords{
    int x;
    int y;
    int z;
};

// Useful Constants
static constexpr double K_b = 8.6173325e-5;
static constexpr double Elementary_charge = 1.6021766e-19; // C
static constexpr double Vacuum_permittivity =  8.8541878e-12; // C/Vm
static constexpr double J_to_eV = 6.2415097e18; // eV/J
static constexpr double Plank = 6.6260696e-34;
static constexpr double Light_speed = 2.9979246e8;
static constexpr double Pi = 3.14159265359;

double array_avg(const double data[],const int size);
double array_stdev(const double data[],const int size);
vector<double> calculateAverageVector(const vector<double>& input_vector,const int procid,const int nproc);
void createExponentialDOSVector(vector<float>& data,const double mode,const double urbach_energy,const int seed);
void createGaussianDOSVector(vector<float>& data,const double mean,const double stdev,const int seed);
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
