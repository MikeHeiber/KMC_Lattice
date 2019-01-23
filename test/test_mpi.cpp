// Copyright (c) 2017-2019 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "gtest/gtest.h"
#include "Utils.h"
#include <mpi.h>

using namespace std;
using namespace KMC_Lattice;

namespace MPI_Tests {

	class MPI_Test : public ::testing::Test {
	protected:
		int procid;
		int nproc;
		void SetUp() {
			MPI_Comm_size(MPI_COMM_WORLD, &nproc);
			MPI_Comm_rank(MPI_COMM_WORLD, &procid);
		}
	};
	
	TEST_F(MPI_Test, CalculateProbHistAvgTests) {
		// Setup unique data vectors on each proc
		vector<int> data(3);
		for (int i = 0; i < (int)data.size(); i++) {
			data[i] = procid + i;
		}
		// Calculate the histogram of the data
		auto hist = calculateHist(data, 1);
		// Check histogram size
		EXPECT_EQ(3, (int)hist.size());
		// Calculate average probability histogram from all procs
		vector<pair<double, double>> prob;
		try {
			prob = MPI_calculateProbHistAvg(hist);
		}
		catch (bad_alloc& ba) {
			cout << procid << ": bad_alloc caught: " << ba.what() << endl;
		}
		if (procid == 0) {
			ASSERT_EQ(6, (int)prob.size());
			EXPECT_DOUBLE_EQ(1.0 / 12.0, prob[0].second);
			EXPECT_DOUBLE_EQ(2.0 / 12.0, prob[1].second);
			EXPECT_DOUBLE_EQ(3.0 / 12.0, prob[2].second);
			EXPECT_DOUBLE_EQ(3.0 / 12.0, prob[3].second);
			EXPECT_DOUBLE_EQ(2.0 / 12.0, prob[4].second);
			EXPECT_DOUBLE_EQ(1.0 / 12.0, prob[5].second);
		}
		// Check behavior when the histogram is empty
		hist.clear();
		EXPECT_THROW(MPI_calculateProbHistAvg(hist), invalid_argument);
	}

	TEST_F(MPI_Test, CalculateVectorAvgTests) {
		// Seteup unique data vectors on each proc
		vector<double> data(3);
		for (int i = 0; i < (int)data.size(); i++) {
			data[i] = 3 * procid + i;
		}
		vector<double> data_avg = MPI_calculateVectorAvg(data);
		if (procid == 0) {
			EXPECT_EQ(3, (int)data_avg.size());
			EXPECT_DOUBLE_EQ(4.5, data_avg[0]);
			EXPECT_DOUBLE_EQ(5.5, data_avg[1]);
			EXPECT_DOUBLE_EQ(6.5, data_avg[2]);
		}
	}

	TEST_F(MPI_Test, CalculateVectorSumTests) {
		// Setup unique data vectors on each proc
		vector<double> data(3);
		for (int i = 0; i < (int)data.size(); i++) {
			data[i] = 3 * procid + i;
		}
		vector<double> data_sum = MPI_calculateVectorSum(data);
		if (procid == 0) {
			EXPECT_EQ(3, (int)data_sum.size());
			EXPECT_DOUBLE_EQ(18.0, data_sum[0]);
			EXPECT_DOUBLE_EQ(22.0, data_sum[1]);
			EXPECT_DOUBLE_EQ(26.0, data_sum[2]);
		}
		vector<int> data2(3);
		for (int i = 0; i < (int)data2.size(); i++) {
			data2[i] = 3 * procid + i;
		}
		vector<int> data_sum2 = MPI_calculateVectorSum(data2);
		if (procid == 0) {
			EXPECT_EQ(3, (int)data_sum2.size());
			EXPECT_EQ(18, data_sum2[0]);
			EXPECT_EQ(22, data_sum2[1]);
			EXPECT_EQ(26, data_sum2[2]);
		}
	}
	
	TEST_F(MPI_Test, GatherValuesTests) {
		// Collect procid from each proc integers
		auto data_all = MPI_gatherValues(procid);
		if (procid == 0) {
			// Check size of data vector
			EXPECT_EQ(nproc, (int)data_all.size());
			// check values of data vector
			for (int i = 0; i < (int)data_all.size(); i++) {
				EXPECT_EQ(i, data_all[i]);
			}
		}
		// Check behavior of negative ints
		data_all = MPI_gatherValues(-procid);
		if (procid == 0) {
			// Check size of data vector
			EXPECT_EQ(nproc, (int)data_all.size());
			// check values of data vector
			for (int i = 0; i < (int)data_all.size(); i++) {
				EXPECT_EQ(-i, data_all[i]);
			}
		}
		// Collect procid from each proc as doubles
		auto data_all2 = MPI_gatherValues((double)procid);
		if (procid == 0) {
			// Check size of data vector
			EXPECT_EQ(nproc, (int)data_all2.size());
			// check values of data vector
			for (int i = 0; i < (int)data_all2.size(); i++) {
				EXPECT_DOUBLE_EQ((double)i, data_all2[i]);
			}
		}
		// Check behavior of negative doubles
		data_all2 = MPI_gatherValues((double)-procid);
		if (procid == 0) {
			// Check size of data vector
			EXPECT_EQ(nproc, (int)data_all2.size());
			// check values of data vector
			for (int i = 0; i < (int)data_all2.size(); i++) {
				EXPECT_DOUBLE_EQ((double)-i, data_all2[i]);
			}
		}
	}

	TEST_F(MPI_Test, GatherVectorsTests) {
		// Setup unique data vectors on each proc
		vector<double> data(3);
		for (int i = 0; i < (int)data.size(); i++) {
			data[i] = 3 * procid + i;
		}
		vector<double> data_all = MPI_gatherVectors(data);
		if (procid == 0) {
			EXPECT_EQ(12, (int)data_all.size());
			for (int i = 0; i < 3 * nproc; i++) {
				EXPECT_DOUBLE_EQ((double)i, data_all[i]);
			}
		}
		vector<int> data2(3);
		for (int i = 0; i < (int)data2.size(); i++) {
			data2[i] = 3 * procid + i;
		}
		vector<int> data_all2 = MPI_gatherVectors(data2);
		if (procid == 0) {
			EXPECT_EQ(12, (int)data_all2.size());
			for (int i = 0; i < 3 * nproc; i++) {
				EXPECT_EQ(i, data_all2[i]);
			}
		}
	}
}

int main(int argc, char **argv) {
	int result = 0;
	::testing::InitGoogleTest(&argc, argv);
	MPI_Init(&argc, &argv);
	result = RUN_ALL_TESTS();
	MPI_Finalize();
	return result;
}
