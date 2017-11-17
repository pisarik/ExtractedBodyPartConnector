#ifndef BODY_PART_CONNECTOR_H
#define BODY_PART_CONNECTOR_H

#include <vector>
#include <utility>

#include "point.hpp"

using MAP_TYPE = float;

// DEFAULT PARAMS
extern const float POSE_DEFAULT_CONNECT_INTER_MIN_ABOVE_THRESHOLD; // 0.85f // Matlab version
extern const float POSE_DEFAULT_CONNECT_INTER_THRESHOLD;
extern const unsigned int POSE_DEFAULT_CONNECT_MIN_SUBSET_CNT;
extern const float POSE_DEFAULT_CONNECT_MIN_SUBSET_SCORE; // 0.2f // Matlab version

/* poseModel - COCO_18*/
// Point<int>{heatMapsBlob->shape(3), heatMapsBlob->shape(2)}
void connectBodyPartsCpu(std::vector<MAP_TYPE>& poseKeypoints, std::vector<MAP_TYPE>& poseScores, const MAP_TYPE* const heatMapPtr,
	const MAP_TYPE* const peaksPtr, const op::Point<int>& heatMapSize,
	const int maxPeaks, const MAP_TYPE interMinAboveThreshold, const MAP_TYPE interThreshold,
	const int minSubsetCnt, const MAP_TYPE minSubsetScore, const MAP_TYPE scaleFactor = 1.f);

#endif //BODY_PART_CONNECTOR_H