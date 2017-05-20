// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Utils.h"

vector<double> MPI_calculateVectorAvg(const vector<double>& input_vector,const int procid,const int nproc){
    int data_size = 0;
    int data_count = 0;
    double *data = NULL;
    double *data_all = NULL;
    int *data_sizes = NULL;
    int *data_displacement = NULL;
    int max_data_size = 0;
    double average = 0;
    vector<double> output_vector;
    if(procid==0){
        data_sizes = (int *)malloc(sizeof(int)*nproc);
    }
    data_size = (int)input_vector.size();
    data = (double *)malloc(sizeof(double)*data_size);
    for(int i=0;i<(int)input_vector.size();i++){
        data[i] = input_vector[i];
    }
    MPI_Gather(&data_size,1,MPI_INT,data_sizes,1,MPI_INT,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<nproc;i++){
            data_count += data_sizes[i];
        }
        data_all = (double *)malloc(sizeof(double)*data_count);
        data_displacement = (int *)malloc(sizeof(int)*nproc);
        data_displacement[0] = 0;
        for(int i=1;i<nproc;i++){
            data_displacement[i] = data_displacement[i-1] + data_sizes[i-1];
        }
    }
    MPI_Gatherv(data,data_size,MPI_DOUBLE,data_all,data_sizes,data_displacement,MPI_DOUBLE,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<nproc;i++){
            if(data_sizes[i]>max_data_size){
                max_data_size = data_sizes[i];
            }
        }
        for(int i=0;i<max_data_size;i++){
            average = 0;
            for(int j=0;j<nproc;j++){
                if(i<data_sizes[j]){
                    average += data_all[data_displacement[j]+i];
                }
            }
            average = average/nproc;
            output_vector.push_back(average);
        }
    }
    delete data;
    delete data_all;
    delete data_sizes;
    delete data_displacement;
    return output_vector;
}

vector<double> MPI_calculateVectorSum(const vector<double>& input_vector,const int procid,const int nproc){
    int data_size = 0;
    int data_count = 0;
    double *data = NULL;
    double *sum = NULL;
    vector<double> output_vector;
    data_size = (int)input_vector.size();
    data = (double *)malloc(sizeof(double)*data_size);
    sum = (double *)malloc(sizeof(double)*data_size);
    for(int i=0;i<(int)input_vector.size();i++){
        data[i] = input_vector[i];
    }
    MPI_Reduce(data,sum,data_size,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<data_size;i++){
            output_vector.push_back(sum[i]);
        }
    }
    delete data;
    delete sum;
    return output_vector;
}

vector<int> MPI_calculateVectorSum(const vector<int>& input_vector,const int procid,const int nproc){
    int data_size = 0;
    int data_count = 0;
    int *data = NULL;
    int *sum = NULL;
    vector<int> output_vector;
    data_size = (int)input_vector.size();
    data = (int *)malloc(sizeof(int)*data_size);
    sum = (int *)malloc(sizeof(int)*data_size);
    for(int i=0;i<(int)input_vector.size();i++){
        data[i] = input_vector[i];
    }
    MPI_Reduce(data,sum,data_size,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<data_size;i++){
            output_vector.push_back(sum[i]);
        }
    }
    delete data;
    delete sum;
    return output_vector;
}

bool importBooleanParam(const string& input,bool& error_status){
    if(input.compare("true")==0){
        return true;
    }
    else if(input.compare("false")==0){
        return false;
    }
    else{
        cout << "Error importing boolean parameter." << endl;
        error_status = true;
        return false;
    }
}

vector<double> MPI_gatherVectors(const vector<double>& input_vector,const int procid,const int nproc){
    int data_size = 0;
    int data_count = 0;
    double *data = NULL;
    double *data_all = NULL;
    int *data_sizes = NULL;
    int *data_displacement = NULL;
    int max_data_size = 0;
    double average = 0;
    vector<double> output_vector;
    if(procid==0){
        data_sizes = (int *)malloc(sizeof(int)*nproc);
    }
    data_size = (int)input_vector.size();
    data = (double *)malloc(sizeof(double)*data_size);
    for(int i=0;i<(int)input_vector.size();i++){
        data[i] = input_vector[i];
    }
    MPI_Gather(&data_size,1,MPI_INT,data_sizes,1,MPI_INT,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<nproc;i++){
            data_count += data_sizes[i];
        }
        data_all = (double *)malloc(sizeof(double)*data_count);
        data_displacement = (int *)malloc(sizeof(int)*nproc);
        data_displacement[0] = 0;
        for(int i=1;i<nproc;i++){
            data_displacement[i] = data_displacement[i-1] + data_sizes[i-1];
        }
    }
    MPI_Gatherv(data,data_size,MPI_DOUBLE,data_all,data_sizes,data_displacement,MPI_DOUBLE,0,MPI_COMM_WORLD);
    if(procid==0){
        for(int i=0;i<data_count;i++){
            output_vector.push_back(data_all[i]);
        }
    }
    delete data;
    delete data_all;
    delete data_sizes;
    delete data_displacement;
    return output_vector;
}

void createExponentialDOSVector(vector<double>& data,const double mode,const double urbach_energy,mt19937& gen){
    exponential_distribution<double> dist_exp(1/urbach_energy);
    auto rand_exp = bind(dist_exp,ref(gen));
    normal_distribution<double> dist_gaus(0,2*urbach_energy/sqrt(2*Pi));
    auto rand_gaus = bind(dist_gaus,ref(gen));
    double energy;
    for(int i=0;i<(int)data.size();i++){
        energy = rand_gaus();
        if(energy>0){
            data[i] = mode+energy;
        }
        else{
            data[i] = mode-rand_exp();
        }
    }
}

void createGaussianDOSVector(vector<double>& data,const double mean,const double stdev,mt19937& gen){
    normal_distribution<double> dist(mean,stdev);
    auto rand_gaus = bind(dist,ref(gen));
    for(int i=0;i<(int)data.size();i++){
        data[i] = rand_gaus();
    }
}

