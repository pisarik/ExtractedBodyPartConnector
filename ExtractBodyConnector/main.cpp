#include <iostream>
#include <iomanip>
#include <vector>
#include <regex>
#include <sstream>
#include <fstream>
#include <iterator>

#include "def_heatmap_cl.hpp"
#include "BodyPartConnector.h"

using namespace std;

int main() {
	//InpDataCF<double> heatmap_cf;
	//ifstream("heatmap_cf.txt") >> heatmap_cf;

	//Tensor<int, 2, 2, 2> cube{ {0, 1, 2, 3, 4, 5, 6, 7} };

	cout << HEATMAP_CL.channels << endl;
	//PAF_CF.data[0] = 3;
	//cout << PAF_CF.data[0] << endl;

	//for (int i = 0; i < 2; i++)
	//	for (int j = 0; j < 2; j++)
	//		for (int k = 0; k < 2; k++) {
	//			cout << cube.get(i, j, k) << " ";
	//		}
	std::vector<MAP_TYPE> poseKeypoints;
	std::vector<MAP_TYPE> poseScores;

	//op::Point<int> x(HEATMAP_CL.cols, HEATMAP_CL.rows);

	connectBodyPartsCpu(poseKeypoints, poseScores, HEATMAP_CL.data.data(), PAF_CL.data.data(),
						op::Point<int>{HEATMAP_CL.cols, HEATMAP_CL.rows}, /*maxPeaks = peaksBlob->shape(2) - 1; ??*/ PAF_CL.channels - 1,
						POSE_DEFAULT_CONNECT_INTER_MIN_ABOVE_THRESHOLD,
						POSE_DEFAULT_CONNECT_INTER_THRESHOLD,
						POSE_DEFAULT_CONNECT_MIN_SUBSET_CNT,
						POSE_DEFAULT_CONNECT_MIN_SUBSET_SCORE);

	cout << poseKeypoints.size() << endl;
	
	system("pause");
	return 0;
}