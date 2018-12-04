// Copyright (c) 2017-2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software package.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Utils.h"

using namespace std;

namespace KMC_Lattice {

	std::vector<std::pair<double, double>> calculateCumulativeHist(const std::vector<std::pair<double, double>>& hist) {
		// Check for valid input data
		if ((int)hist.size() == 0) {
			cout << "Error! Cannot calculate cumulative histogram because the input histogram vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate cumulative histogram because the input histogram vector is empty.");
		}
		auto result = hist;
		for (int i = 1; i < (int)hist.size(); i++) {
			result[i].second = result[i - 1].second + hist[i].second;
		}
		return result;
	}

	std::vector<std::pair<double, double>> calculateDensityHist(const std::vector<std::pair<double, double>>& hist) {
		// Check for valid input data
		if ((int)hist.size() == 0) {
			cout << "Error! Cannot calculate density histogram because input probability histogram is empty." << endl;
			throw invalid_argument("Error! Cannot calculate density histogram because the input probability histogram is empty.");
		}
		if ((int)hist.size() < 2) {
			cout << "Error! Cannot calculate density histogram because the input probability histogram is too small." << endl;
			throw invalid_argument("Error! Cannot calculate density histogram because the input probability histogram is too small.");
		}
		double bin_size = hist[1].first - hist[0].first;
		auto result = hist;
		for (auto& item : result) {
			item.second /= bin_size;
		}
		return result;
	}

	std::vector<std::pair<double, int>> calculateHist(const std::vector<int>& data, int bin_size) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate histogram because data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate histogram because the input data vector is empty.");
		}
		if (bin_size < 1) {
			cout << "Error! Cannot calculate histogram because bin size is less than 1." << endl;
			throw invalid_argument("Error! Cannot calculate histogram because bin size is less than 1.");
		}
		// Determine the starting bin position
		int min_val = *min_element(data.begin(), data.end());
		int max_val = *max_element(data.begin(), data.end());
		// Determine number of bins
		int num_bins = (int)((double)(max_val - min_val + 1) / (double)bin_size);
		// Calculate bins
		vector<pair<double, int>> hist(num_bins, make_pair(0.0, 0));
		for (int i = 0; i < num_bins; i++) {
			if (bin_size == 1) {
				hist[i].first = min_val + bin_size * i;
			}
			else {
				hist[i].first = min_val + 0.5*(bin_size - 1) + bin_size * i;
			}
		}
		// Calculate histogram
		int index;
		for (int i = 0; i < (int)data.size(); i++) {
			index = (data[i] - min_val) / bin_size;
			hist[index].second++;
		}
		return hist;
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<std::pair<double, int>> hist) {
		// Check for valid input data
		if ((int)hist.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input histogram is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input histogram is empty.");
		}
		// Add up the total counts in the histogram
		int total_counts = 0;
		for (const auto item : hist) {
			total_counts += item.second;
		}
		// Normalized histogram to get probability histogram
		vector<pair<double, double>> prob_hist(hist.size(), make_pair(0.0, 0.0));
		for (int i = 0; i < (int)hist.size(); i++) {
			prob_hist[i].first = hist[i].first;
			prob_hist[i].second = (double)hist[i].second / (double)(total_counts);
		}
		return prob_hist;
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<int>& data, int bin_size) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine the starting bin position
		int min_val = *min_element(data.begin(), data.end());
		int max_val = *max_element(data.begin(), data.end());
		// Determine number of bins
		int num_bins = (int)((double)(max_val - min_val + 1) / (double)bin_size);
		// Calculate bins
		vector<pair<double, double>> hist(num_bins, make_pair(0.0, 0.0));
		for (int i = 0; i < num_bins; i++) {
			hist[i].first = min_val + 0.5*(bin_size - 1) + (bin_size - 1) * i;
		}
		// Calculate histogram
		vector<int> counts(num_bins, 0);
		int index;
		for (int i = 0; i < (int)data.size(); i++) {
			index = (data[i] - min_val) / bin_size;
			counts[index]++;
		}
		// total counts
		int total_counts = accumulate(counts.begin(), counts.end(), 0);
		// Normalized histogram to get probability
		for (int i = 0; i < num_bins; i++) {
			hist[i].second = (double)counts[i] / (double)(total_counts);
		}
		return hist;
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<double>& data, int num_bins) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine data range
		double min_val = *min_element(data.begin(), data.end());
		double max_val = *max_element(data.begin(), data.end());
		// Limit the number of bins to the number of data entries
		if (num_bins > (int)data.size()) {
			num_bins = (int)data.size();
		}
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		max_val += 1e-12*abs(max_val);
		// Determine bin size
		double bin_size = (max_val - min_val) / num_bins;
		return calculateProbabilityHist(data, bin_size, num_bins);
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<float>& data, int num_bins) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine data range
		float min_val = *min_element(data.begin(), data.end());
		float max_val = *max_element(data.begin(), data.end());
		// Limit the number of bins to the number of data entries
		if (num_bins > (int)data.size()) {
			num_bins = (int)data.size();
		}
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		max_val += 1e-12*abs(max_val);
		// Determine bin size
		double bin_size = (max_val - min_val) / num_bins;
		return calculateProbabilityHist(data, bin_size, num_bins);
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<double>& data, double bin_size) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine data range
		double min_val = *min_element(data.begin(), data.end());
		double max_val = *max_element(data.begin(), data.end());
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		max_val += 1e-12*abs(max_val);
		// Determine number of bins
		int num_bins = (int)ceil((max_val - min_val) / bin_size);
		// Limit the number of bins to the number of data entries
		if (num_bins > (int)data.size()) {
			num_bins = (int)data.size();
			bin_size = (max_val - min_val) / (double)num_bins;
		}
		return calculateProbabilityHist(data, bin_size, num_bins);
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<float>& data, double bin_size) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine data range
		float min_val = *min_element(data.begin(), data.end());
		float max_val = *max_element(data.begin(), data.end());
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		max_val += 1e-12*abs(max_val);
		// Determine number of bins
		int num_bins = (int)ceil((max_val - min_val) / bin_size);
		// Limit the number of bins to the number of data entries
		if (num_bins > (int)data.size()) {
			num_bins = (int)data.size();
			bin_size = (max_val - min_val) / (double)num_bins;
		}
		return calculateProbabilityHist(data, bin_size, num_bins);
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<double>& data, const double bin_size, const int num_bins) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine the starting bin position
		double min_val = *min_element(data.begin(), data.end());
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		// Calculate bin-centered x values
		vector<pair<double, double>> hist(num_bins, make_pair(0.0, 0.0));
		for (int i = 0; i < num_bins; i++) {
			hist[i].first = min_val + 0.5*bin_size + bin_size * i;
		}
		// Calculate histogram
		vector<int> counts(num_bins, 0);
		int index;
		for (int i = 0; i < (int)data.size(); i++) {
			index = (int)floor((data[i] - min_val) / bin_size);
			counts[index]++;
		}
		// total counts
		int total_counts = accumulate(counts.begin(), counts.end(), 0);
		// Normalized histogram to get probability
		for (int i = 0; i < num_bins; i++) {
			hist[i].second = (double)counts[i] / (double)(total_counts);
		}
		return hist;
	}

	std::vector<std::pair<double, double>> calculateProbabilityHist(const std::vector<float>& data, const double bin_size, const int num_bins) {
		// Check for valid input data
		if ((int)data.size() == 0) {
			cout << "Error! Cannot calculate probability histogram because the input data vector is empty." << endl;
			throw invalid_argument("Error! Cannot calculate probability histogram because the input data vector is empty.");
		}
		// Determine the starting bin position
		float min_val = *min_element(data.begin(), data.end());
		// Extend the range a little bit to ensure all data fits in the range
		min_val -= 1e-12*abs(min_val);
		// Calculate bin-centered x values
		vector<pair<double, double>> hist(num_bins, make_pair(0.0, 0.0));
		for (int i = 0; i < num_bins; i++) {
			hist[i].first = min_val + 0.5*bin_size + bin_size * i;
		}
		// Calculate histogram
		vector<int> counts(num_bins, 0);
		int index;
		for (int i = 0; i < (int)data.size(); i++) {
			index = (int)floor((data[i] - min_val) / bin_size);
			counts[index]++;
		}
		// total counts
		int total_counts = accumulate(counts.begin(), counts.end(), 0);
		// Normalized histogram to get probability
		for (int i = 0; i < num_bins; i++) {
			hist[i].second = (double)counts[i] / (double)(total_counts);
		}
		return hist;
	}

	void createExponentialDOSVector(std::vector<double>& data, const double mode, const double urbach_energy, std::mt19937_64& gen) {
		exponential_distribution<double> dist_exp(1.0 / urbach_energy);
		auto rand_exp = bind(dist_exp, ref(gen));
		normal_distribution<double> dist_gaus(0.0, 2.0*urbach_energy / sqrt(2.0 * Pi));
		auto rand_gaus = bind(dist_gaus, ref(gen));
		for (auto &item : data) {
			double energy = rand_gaus();
			if (energy < 0) {
				energy = -rand_exp();
			}
			item = mode + energy;
		}
	}

	void createExponentialDOSVector(std::vector<float>& data, const double mode, const double urbach_energy, std::mt19937_64& gen) {
		exponential_distribution<float> dist_exp(1.0 / urbach_energy);
		auto rand_exp = bind(dist_exp, ref(gen));
		normal_distribution<float> dist_gaus(0.0, 2.0*urbach_energy / sqrt(2.0 * Pi));
		auto rand_gaus = bind(dist_gaus, ref(gen));
		for (auto &item : data) {
			float energy = rand_gaus();
			if (energy < 0) {
				energy = -rand_exp();
				while (isinf(energy)) {
					energy = -rand_exp();
				}
			}
			item = mode + energy;
		}
	}

	void createGaussianDOSVector(std::vector<double>& data, const double mean, const double stdev, std::mt19937_64& gen) {
		normal_distribution<double> dist(mean, stdev);
		auto rand_gaus = bind(dist, ref(gen));
		for (auto &item : data) {
			item = rand_gaus();
		}
	}

	void createGaussianDOSVector(std::vector<float>& data, const double mean, const double stdev, std::mt19937_64& gen) {
		normal_distribution<float> dist(mean, stdev);
		auto rand_gaus = bind(dist, ref(gen));
		for (auto &item : data) {
			item = rand_gaus();
		}
	}

	double integrateData(const std::vector<std::pair<double, double>>& data) {
		double area = 0;
		for (int i = 1; i < (int)data.size(); i++) {
			area += ((data[i - 1].second + data[i].second) / 2.0)*(data[i].first - data[i - 1].first);
		}
		return area;
	}

	double interpolateData(const std::vector<std::pair<double, double>>& data, const double x_val) {
		for (int i = 1; i < (int)data.size(); i++) {
			if (data[i - 1].first <= x_val && data[i].first >= x_val) {
				return data[i - 1].second + ((data[i].second - data[i - 1].second) / (data[i].first - data[i - 1].first))*(x_val - data[i - 1].first);
			}
		}
		cout << "Warning! The input x-value lies outside the range of the input data set." << endl;
		return NAN;
	}

	std::vector<std::pair<double, double>> MPI_calculateProbHistAvg(const std::vector<std::pair<double, int>>& input_hist) {
		if ((int)input_hist.size() < 2) {
			throw invalid_argument("Unable to calculate the average probability histogram because the input histogram must have more than one bin.");
		}
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Determine the bin size
		double bin_size = input_hist[1].first - input_hist[0].first;
		// Determine the smallest bin and largest bin
		double min_bin = input_hist[0].first;
		double max_bin = input_hist.back().first;
		// Gather all of the min and max bins from each proc into vectors
		auto min_bins = MPI_gatherValues(min_bin);
		auto max_bins = MPI_gatherValues(max_bin);
		// Determine the overall smallest and largest bin in the set
		double smallest_bin;
		double largest_bin;
		if (procid == 0) {
			smallest_bin = *min_element(min_bins.begin(), min_bins.end());
			largest_bin = *max_element(max_bins.begin(), max_bins.end());
		}
		// Broadcast the smallest and largest bins to all procs
		MPI_Bcast(&smallest_bin, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&largest_bin, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		int final_size = (int)ceil((largest_bin - smallest_bin) / bin_size) + 1;
		// Create counts vector with the same size as the final prob hist
		vector<int> counts(final_size);
		// Separate out the counts data from the input histograms into appropriate bins
		for (int i = 0; i < (int)input_hist.size(); i++) {
			int bin_index = (int)floor((input_hist[i].first - smallest_bin) / bin_size);
			counts[bin_index] = input_hist[i].second;
		}
		// Add up the counts from all processors
		auto counts_sum = MPI_calculateVectorSum(counts);
		// Create output probablilty histogram
		vector<pair<double, double>> prob_hist;
		if (procid == 0) {
			prob_hist.reserve(final_size);
			int total_counts = accumulate(counts_sum.begin(), counts_sum.end(), 0);
			for (int i = 0; i < final_size; i++) {
				prob_hist.push_back(make_pair(smallest_bin + bin_size * i, (double)counts_sum[i] / (double)total_counts));
			}
		}
		return prob_hist;
	}

	std::vector<double> MPI_calculateVectorAvg(const std::vector<double>& input_vector) {
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Get maximum size of all vectors
		int data_size = (int)input_vector.size();
		auto data_sizes = MPI_gatherValues(data_size);
		int max_size;
		if (procid == 0) {
			max_size = *max_element(data_sizes.begin(), data_sizes.end());
		}
		// Tell all procs what the max size is
		MPI_Bcast(&max_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// Copy the input data vector into new data vector and add zeros padding to vectors that are smaller than the max size
		auto data = input_vector;
		data.resize(max_size, 0.0);
		// Calculate the element-wise sum of the vectors
		auto output_vector = MPI_calculateVectorSum(data);
		if (procid == 0) {
			// Normalize the sum vector by nproc to get average
			for (auto& item : output_vector) {
				item /= nproc;
			}
		}
		// return final data vector
		return output_vector;
	}

	std::vector<double> MPI_calculateVectorSum(const std::vector<double>& input_vector) {
		int procid;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		// Get the size of the input data
		int data_size = (int)input_vector.size();
		// Copt the input data into an array
		double* data = new double[data_size];
		copy(input_vector.begin(), input_vector.end(), data);
		// Allocate array memory for the sum data
		double* sum = new double[data_size];
		// Calculate the sum of all of the input vectors
		MPI_Reduce(data, sum, data_size, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		// Copy the data from the sum array into the final data vector
		vector<double> output_vector;
		if (procid == 0) {
			output_vector.assign(sum, sum + data_size);
		}
		// Cleanup allocated memory
		delete[] data;
		delete[] sum;
		// return the final data vector
		return output_vector;
	}

	std::vector<int> MPI_calculateVectorSum(const std::vector<int>& input_vector) {
		int procid;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		// Get the size of the data
		int data_size = (int)input_vector.size();
		// Copy the input data to an array
		int* data = new int[data_size];
		copy(input_vector.begin(), input_vector.end(), data);
		// Allocate array memory for the sum data
		int* sum = new int[data_size];
		// Calculate the sum of all of the input vectors
		MPI_Reduce(data, sum, data_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		// Copy the data from the sum array into the final data vector
		vector<int> output_vector;
		if (procid == 0) {
			output_vector.assign(sum, sum + data_size);
		}
		// Cleanup allocated memory
		delete[] data;
		delete[] sum;
		// return final data vector
		return output_vector;
	}

	std::vector<int> MPI_gatherValues(int input_val) {
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Allocate array memory for the incoming data
		int* data = new int[nproc];
		// Gather all of the data into the array
		MPI_Gather(&input_val, 1, MPI_INT, data, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// Copy the data from the array into the final data vector
		vector<int> output_vector;
		if (procid == 0) {
			output_vector.assign(data, data + nproc);
		}
		// Cleanup allocated memory
		delete[] data;
		// return the final data vector
		return output_vector;
	}

	std::vector<double> MPI_gatherValues(double input_val) {
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Allocate array memory for the incoming data
		double* data = new double[nproc];
		// Gather all of the data into the array
		MPI_Gather(&input_val, 1, MPI_DOUBLE, data, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		// Copy the data from the array into the final data vector
		vector<double> output_vector;
		if (procid == 0) {
			output_vector.assign(data, data + nproc);
		}
		// Cleanup allocated memory
		delete[] data;
		// return the final data vector
		return output_vector;
	}

	std::vector<double> MPI_gatherVectors(const std::vector<double>& input_vector) {
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Get size of each vector
		int* data_sizes = new int[nproc];
		int data_size = (int)input_vector.size();
		MPI_Gather(&data_size, 1, MPI_INT, data_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// Determine data_count and displacements and allocate memory for all data
		int* data_displacement = new int[nproc];
		int data_count = 0;
		double* data_all = NULL;
		if (procid == 0) {
			data_count = accumulate(data_sizes, data_sizes + nproc, 0);
			data_displacement[0] = 0;
			for (int i = 1; i < nproc; i++) {
				data_displacement[i] = data_displacement[i - 1] + data_sizes[i - 1];
			}
			// Allocate memory to hold all of the data
			data_all = new double[data_count];
		}
		// Copy input data into an array
		double* data = new double[data_size];
		copy(input_vector.begin(), input_vector.end(), data);
		// Gather data arrays from all procs into the data_all array on proc 0
		MPI_Gatherv(data, data_size, MPI_DOUBLE, data_all, data_sizes, data_displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		// Copy data from data_all array into the output data vector
		vector<double> output_vector;
		if (procid == 0) {
			output_vector.assign(data_all, data_all + data_count);
		}
		// Cleanup allocated memory
		delete[] data;
		delete[] data_all;
		delete[] data_sizes;
		delete[] data_displacement;
		// return the final data vector
		return output_vector;
	}

	std::vector<int> MPI_gatherVectors(const std::vector<int>& input_vector) {
		int procid;
		int nproc;
		MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		MPI_Comm_size(MPI_COMM_WORLD, &nproc);
		// Get size of each vector
		int* data_sizes = new int[nproc];
		int data_size = (int)input_vector.size();
		MPI_Gather(&data_size, 1, MPI_INT, data_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// Determine data_count and displacements and allocate memory for all data
		int* data_displacement = new int[nproc];
		int data_count = 0;
		int* data_all = NULL;
		if (procid == 0) {
			data_count = accumulate(data_sizes, data_sizes + nproc, 0);
			data_displacement[0] = 0;
			for (int i = 1; i < nproc; i++) {
				data_displacement[i] = data_displacement[i - 1] + data_sizes[i - 1];
			}
			// Allocate memory to hold all of the data
			data_all = new int[data_count];
		}
		// Copy input data into an array
		int* data = new int[data_size];
		copy(input_vector.begin(), input_vector.end(), data);
		// Gather data arrays from all procs into the data_all array on proc 0
		MPI_Gatherv(data, data_size, MPI_INT, data_all, data_sizes, data_displacement, MPI_INT, 0, MPI_COMM_WORLD);
		// Copy data from data_all array into the output data vector
		vector<int> output_vector;
		if (procid == 0) {
			output_vector.assign(data_all, data_all + data_count);
		}
		// Cleanup allocated memory
		delete[] data;
		delete[] data_all;
		delete[] data_sizes;
		delete[] data_displacement;
		// return the final data vector
		return output_vector;
	}

	std::string removeWhitespace(const std::string& str_input) {
		// Remove tab characters
		string str_out = str_input;
		str_out.erase(remove(str_out.begin(), str_out.end(), '\t'), str_out.end());
		str_out.erase(remove(str_out.begin(), str_out.end(), ' '), str_out.end());
		return str_out;
	}

	int round_int(const double num) {
		return (num > 0.0) ? (int)(num + 0.5) : (int)(num - 0.5);
	}

	bool str2bool(const std::string& input) {
		string str = removeWhitespace(input);
		if (str.compare("true") == 0) {
			return true;
		}
		else if (str.compare("false") == 0) {
			return false;
		}
		else {
			throw invalid_argument("Error! Input string is not true or false.");
		}
	}
}
