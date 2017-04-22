#include "Utils.h"

double array_avg(const double data[],const int array_size){
    double sum = 0;
    for(int i=0;i<array_size;i++){
        sum += data[i];
    }
    return sum/array_size;
}

double array_stdev(const double data[],const int array_size){
    double sum = 0;
    double avg = array_avg(data,array_size);
    for(int i=0;i<array_size;i++){
        sum += (data[i]-avg)*(data[i]-avg);
    }
    return sqrt(sum/(array_size-1));
}

vector<double> calculateAverageVector(const vector<double>& input_vector,const int procid,const int nproc){
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
                max_data_size =data_sizes[i];
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
    return output_vector;
}

void createExponentialDOSVector(vector<float>& data,const double mode,const double urbach_energy,const int seed){
    boost::mt19937 gen;
    gen.seed(time(0)*(1+seed));
    boost::exponential_distribution<> dist_exp(1/urbach_energy);
    boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> > rand_exp(gen,dist_exp);
    boost::normal_distribution<> dist_gaus(0,2*urbach_energy/sqrt(2*Pi));
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > rand_gaus(gen,dist_gaus);
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

void createGaussianDOSVector(vector<float>& data,const double mean,const double stdev,const int seed){
    boost::mt19937 gen;
    gen.seed(time(0)*(1+seed));
    boost::normal_distribution<> dist(mean,stdev);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > rand_gaus(gen,dist);
    for(int i=0;i<(int)data.size();i++){
        data[i] = rand_gaus();
    }
}

double intpow(const double base,const int exponent){
    double result = base;
    for(int i=1;i<exponent;i++){
        result *= base;
    }
    return result;
}

float intpow(const float base,const int exponent){
    float result = base;
    for(int i=1;i<exponent;i++){
        result *= base;
    }
    return result;
}

double vector_avg(const vector<int>& dataset){
    double sum = 0;
    double avg;
    vector<int>::const_iterator it;
    for(it=dataset.begin();it!=dataset.end();++it){
        sum += *it;
    }
    avg = sum/dataset.size();
    return avg;
}

double vector_avg(const vector<double>& dataset){
    double sum = 0;
    double avg;
    vector<double>::const_iterator it;
    for(it=dataset.begin();it!=dataset.end();++it){
        sum += *it;
    }
    avg = sum/dataset.size();
    return avg;
}

double vector_stdev(const vector<int>& dataset){
    double sum = 0;
    double avg, dev;
    avg = vector_avg(dataset);
    vector<int>::const_iterator it;
    for(it=dataset.begin();it!=dataset.end();++it){
        sum += (*it-avg)*(*it-avg);
    }
    dev = sqrt(sum/(dataset.size()-1));
    return dev;
}

double vector_stdev(const vector<double>& dataset){
    double sum = 0;
    double avg, dev;
    avg = vector_avg(dataset);
    vector<double>::const_iterator it;
    for(it=dataset.begin();it!=dataset.end();++it){
        sum += (*it-avg)*(*it-avg);
    }
    dev = sqrt(sum/(dataset.size()-1));
    return dev;
}

