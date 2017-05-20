// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef UTILS_H
#define UTILS_H

#include <mpi.h>
#include <vector>
#include <random>
#include <cmath>
#include <memory>
#include <iostream>

using namespace std;

struct Coords{
    int x;
    int y;
    int z;
    void setXYZ(const int& xval,const int& yval,const int& zval){
        x = xval;
        y = yval;
        z = zval;
    }
};

// Useful Constants
static constexpr double K_b = 8.61733035e-5;
static constexpr double Elementary_charge = 1.602176621e-19; // C
static constexpr double Vacuum_permittivity =  8.854187818e-12; // C/Vm
static constexpr double Plank = 6.626070040e-34;
static constexpr double Light_speed = 2.99792458e8;
static constexpr double Pi = 3.14159265359;
static constexpr double Coulomb_constant = 8.987551787e9; // N m^2 C^-2

void createExponentialDOSVector(vector<double>& data,const double mode,const double urbach_energy,mt19937& gen);
void createGaussianDOSVector(vector<double>& data,const double mean,const double stdev,mt19937& gen);
bool importBooleanParam(const string& input,bool& error_status);
vector<double> MPI_calculateVectorAvg(const vector<double>& input_vector,const int procid,const int nproc);
vector<double> MPI_calculateVectorSum(const vector<double>& input_vector,const int procid);
vector<int> MPI_calculateVectorSum(const vector<int>& input_vector,const int procid);
vector<double> MPI_gatherVectors(const vector<double>& input_vector,const int procid,const int nproc);


// Template functions
template<typename T>
double array_avg(const T data[],const int array_size){
    double sum = 0;
    for(int i=0;i<array_size;i++){
        sum += data[i];
    }
    return sum/array_size;
}

template<typename T>
double array_stdev(const T data[],const int array_size){
    double sum = 0;
    double avg = array_avg(data,array_size);
    for(int i=0;i<array_size;i++){
        sum += (data[i]-avg)*(data[i]-avg);
    }
    return sqrt(sum/(array_size-1));
}

template<typename base_type>
base_type intpow(const base_type base,const int exponent){
    base_type result = base;
    for(int i=1;i<exponent;i++){
        result *= base;
    }
    return result;
}

template<typename T,typename A>
double vector_avg(const vector<T,A>& dataset){
    double sum = 0;
    for(auto it=dataset.begin();it!=dataset.end();++it){
        sum += *it;
    }
    return sum/dataset.size();
}

template<typename T,typename A>
double vector_stdev(const vector<T,A>& dataset){
    double sum = 0;
    double avg = vector_avg(dataset);
    for(auto it=dataset.begin();it!=dataset.end();++it){
        sum += (*it-avg)*(*it-avg);
    }
    return sqrt(sum/(dataset.size()-1));
}

#endif // UTILS_H
