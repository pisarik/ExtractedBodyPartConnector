#include "BodyPartConnector.h"

#include <algorithm>
#include <functional>
#include <tuple>

// Constant Global Parameters
const unsigned int POSE_MAX_PEOPLE = 96u;
const unsigned int POSE_COCO_NUMBER_PARTS = 18u; // Equivalent to size of std::map POSE_COCO_BODY_PARTS - 1 (removing background)
const std::vector<unsigned int> POSE_COCO_PAIRS{ 1,2,   1,5,   2,3,   3,4,   5,6,   6,7,   1,8,   8,9,   9,10,  1,11,  11,12, 12,13,  1,0,   0,14, 14,16,  0,15, 15,17,  2,16,  5,17 };
const std::vector<unsigned int> POSE_COCO_MAP_IDX{ 31,32, 39,40, 33,34, 35,36, 41,42, 43,44, 19,20, 21,22, 23,24, 25,26, 27,28, 29,30, 47,48, 49,50, 53,54, 51,52, 55,56, 37,38, 45,46 };

// DEFAULT PARAMS
const float POSE_DEFAULT_CONNECT_INTER_MIN_ABOVE_THRESHOLD = 0.95f; // 0.85f // Matlab version
const float POSE_DEFAULT_CONNECT_INTER_THRESHOLD = 0.05f;
const unsigned int POSE_DEFAULT_CONNECT_MIN_SUBSET_CNT = 3;
const float POSE_DEFAULT_CONNECT_MIN_SUBSET_SCORE = 0.4f; // 0.2f // Matlab version

template<typename T>
inline int intRound(const T a)
{
	return int(a + 0.5f);
}

void connectBodyPartsCpu(std::vector<MAP_TYPE>& poseKeypoints, std::vector<MAP_TYPE>& poseScores, 
						 const MAP_TYPE * const heatMapPtr, const MAP_TYPE * const peaksPtr, 
						 const op::Point<int>& heatMapSize, const int maxPeaks, 
						 const MAP_TYPE interMinAboveThreshold, const MAP_TYPE interThreshold, 
						 const int minSubsetCnt, const MAP_TYPE minSubsetScore, const MAP_TYPE scaleFactor)
{
	try
	{
		// Parts Connection
		const auto& bodyPartPairs = POSE_COCO_PAIRS;
		const auto& mapIdx = POSE_COCO_MAP_IDX;
		const auto numberBodyParts = POSE_COCO_NUMBER_PARTS;
		const auto numberBodyPartPairs = bodyPartPairs.size() / 2;

		// Vector<int> = Each body part + body parts counter; double = subsetScore
		std::vector<std::pair<std::vector<int>, double>> subset;
		const auto subsetCounterIndex = numberBodyParts;
		const auto subsetSize = numberBodyParts + 1;

		const auto peaksOffset = 3 * (maxPeaks + 1);
		const auto heatMapOffset = heatMapSize.area();

		for (auto pairIndex = 0u; pairIndex < numberBodyPartPairs; pairIndex++)
		{
			const auto bodyPartA = bodyPartPairs[2 * pairIndex];
			const auto bodyPartB = bodyPartPairs[2 * pairIndex + 1];
			const auto* candidateA = peaksPtr + bodyPartA*peaksOffset;
			const auto* candidateB = peaksPtr + bodyPartB*peaksOffset;
			const auto nA = intRound(candidateA[0]);
			const auto nB = intRound(candidateB[0]);

			// Add parts into the subset in special case
			if (nA == 0 || nB == 0)
			{
				// Change w.r.t. other
				if (nA == 0) // nB == 0 or not
				{
					// poseModel == COCO_18
					for (auto i = 1; i <= nB; i++)
					{
						bool num = false;
						const auto indexB = bodyPartB;
						for (auto j = 0u; j < subset.size(); j++)
						{
							const auto off = (int)bodyPartB*peaksOffset + i * 3 + 2;
							if (subset[j].first[indexB] == off)
							{
								num = true;
								break;
							}
						}
						if (!num)
						{
							std::vector<int> rowVector(subsetSize, 0);
							// Store the index
							rowVector[bodyPartB] = bodyPartB*peaksOffset + i * 3 + 2;
							// Last number in each row is the parts number of that person
							rowVector[subsetCounterIndex] = 1;
							const auto subsetScore = candidateB[i * 3 + 2];
							// Second last number in each row is the total score
							subset.emplace_back(std::make_pair(rowVector, subsetScore));
						}
					}
				}
				else // if (nA != 0 && nB == 0)
				{
					// poseModel == COCO_18
					for (auto i = 1; i <= nA; i++)
					{
						bool num = false;
						const auto indexA = bodyPartA;
						for (auto j = 0u; j < subset.size(); j++)
						{
							const auto off = (int)bodyPartA*peaksOffset + i * 3 + 2;
							if (subset[j].first[indexA] == off)
							{
								num = true;
								break;
							}
						}
						if (!num)
						{
							std::vector<int> rowVector(subsetSize, 0);
							// Store the index
							rowVector[bodyPartA] = bodyPartA*peaksOffset + i * 3 + 2;
							// Last number in each row is the parts number of that person
							rowVector[subsetCounterIndex] = 1;
							// Second last number in each row is the total score
							const auto subsetScore = candidateA[i * 3 + 2];
							subset.emplace_back(std::make_pair(rowVector, subsetScore));
						}
					}
				}
			}
			else // if (nA != 0 && nB != 0)
			{
				std::vector<std::tuple<double, int, int>> temp;
				const auto* const mapX = heatMapPtr + mapIdx[2 * pairIndex] * heatMapOffset;
				const auto* const mapY = heatMapPtr + mapIdx[2 * pairIndex + 1] * heatMapOffset;
				for (auto i = 1; i <= nA; i++)
				{
					for (auto j = 1; j <= nB; j++)
					{
						const auto dX = candidateB[j * 3] - candidateA[i * 3];
						const auto dY = candidateB[j * 3 + 1] - candidateA[i * 3 + 1];
						const auto dMax = std::max(std::abs(dX), std::abs(dY));
						const auto numberPointsInLine = std::max(5, std::min(25, intRound(std::sqrt(5 * dMax))));
						const auto normVec = MAP_TYPE(std::sqrt(dX*dX + dY*dY));
						// If the peaksPtr are coincident. Don't connect them.
						if (normVec > 1e-6)
						{
							const auto sX = candidateA[i * 3];
							const auto sY = candidateA[i * 3 + 1];
							const auto vecX = dX / normVec;
							const auto vecY = dY / normVec;

							auto sum = 0.;
							auto count = 0;
							const auto dXInLine = dX / numberPointsInLine;
							const auto dYInLine = dY / numberPointsInLine;
							for (auto lm = 0; lm < numberPointsInLine; lm++)
							{
								const auto mX = std::min(heatMapSize.x - 1, intRound(sX + lm*dXInLine));
								const auto mY = std::min(heatMapSize.y - 1, intRound(sY + lm*dYInLine));
								if (mX < 0 || mY < 0)
									throw "mX || mY < 0";
								const auto idx = mY * heatMapSize.x + mX;
								const auto score = (vecX*mapX[idx] + vecY*mapY[idx]);
								if (score > interThreshold)
								{
									sum += score;
									count++;
								}
							}

							// parts score + connection score
							if (count / (float)numberPointsInLine > interMinAboveThreshold)
								temp.emplace_back(std::make_tuple(sum / count, i, j));
						}
					}
				}

				// select the top minAB connection, assuming that each part occur only once
				// sort rows in descending order based on parts + connection score
				if (!temp.empty())
					std::sort(temp.begin(), temp.end(), std::greater<std::tuple<MAP_TYPE, int, int>>());

				std::vector<std::tuple<int, int, double>> connectionK;
				const auto minAB = std::min(nA, nB);
				std::vector<int> occurA(nA, 0);
				std::vector<int> occurB(nB, 0);
				auto counter = 0;
				for (auto row = 0u; row < temp.size(); row++)
				{
					const auto score = std::get<0>(temp[row]);
					const auto x = std::get<1>(temp[row]);
					const auto y = std::get<2>(temp[row]);
					if (!occurA[x - 1] && !occurB[y - 1])
					{
						connectionK.emplace_back(std::make_tuple(bodyPartA*peaksOffset + x * 3 + 2,
							bodyPartB*peaksOffset + y * 3 + 2,
							score));
						counter++;
						if (counter == minAB)
							break;
						occurA[x - 1] = 1;
						occurB[y - 1] = 1;
					}
				}

				// Cluster all the body part candidates into subset based on the part connection
				if (!connectionK.empty())
				{
					// initialize first body part connection 15&16
					if (pairIndex == 0)
					{
						for (const auto connectionKI : connectionK)
						{
							std::vector<int> rowVector(numberBodyParts + 3, 0);
							const auto indexA = std::get<0>(connectionKI);
							const auto indexB = std::get<1>(connectionKI);
							const auto score = std::get<2>(connectionKI);
							rowVector[bodyPartPairs[0]] = indexA;
							rowVector[bodyPartPairs[1]] = indexB;
							rowVector[subsetCounterIndex] = 2;
							// add the score of parts and the connection
							const auto subsetScore = peaksPtr[indexA] + peaksPtr[indexB] + score;
							subset.emplace_back(std::make_pair(rowVector, subsetScore));
						}
					}
					// Add ears connections (in case person is looking to opposite direction to camera)
					// poseModel == COCO_18
					for (const auto& connectionKI : connectionK)
					{
						const auto indexA = std::get<0>(connectionKI);
						const auto indexB = std::get<1>(connectionKI);
						for (auto& subsetJ : subset)
						{
							auto& subsetJFirst = subsetJ.first[bodyPartA];
							auto& subsetJFirstPlus1 = subsetJ.first[bodyPartB];
							if (subsetJFirst == indexA && subsetJFirstPlus1 == 0)
								subsetJFirstPlus1 = indexB;
							else if (subsetJFirstPlus1 == indexB && subsetJFirst == 0)
								subsetJFirst = indexA;
						}
					}
				}
			}
		}

		// Delete people below the following thresholds:
		// a) minSubsetCnt: removed if less than minSubsetCnt body parts
		// b) minSubsetScore: removed if global score smaller than this
		// c) POSE_MAX_PEOPLE: keep first POSE_MAX_PEOPLE people above thresholds
		auto numberPeople = 0;
		std::vector<int> validSubsetIndexes;
		validSubsetIndexes.reserve(std::min((size_t)POSE_MAX_PEOPLE, subset.size()));
		for (auto index = 0u; index < subset.size(); index++)
		{
			const auto subsetCounter = subset[index].first[subsetCounterIndex];
			const auto subsetScore = subset[index].second;
			if (subsetCounter >= minSubsetCnt && (subsetScore / subsetCounter) >= minSubsetScore)
			{
				numberPeople++;
				validSubsetIndexes.emplace_back(index);
				if (numberPeople == POSE_MAX_PEOPLE)
					break;
			}
			else if (subsetCounter < 1)
				throw "Bad subsetCounter. Bug in this function if this happens.";
		}

		// Fill and return poseKeypoints
		if (numberPeople > 0)
		{
			//poseKeypoints.reset({ numberPeople, (int)numberBodyParts, 3 });
			poseKeypoints.resize(numberPeople*(int)numberBodyPartPairs * 3);
			poseScores.resize(numberPeople);
		}
		else
		{
			poseKeypoints.clear();
			poseScores.clear();
		}
		const auto numberBodyPartsAndPAFs = numberBodyParts + numberBodyPartPairs;
		for (auto person = 0u; person < validSubsetIndexes.size(); person++)
		{
			const auto& subsetPair = subset[validSubsetIndexes[person]];
			const auto& subsetI = subsetPair.first;
			for (auto bodyPart = 0u; bodyPart < numberBodyParts; bodyPart++)
			{
				const auto baseOffset = (person*numberBodyParts + bodyPart) * 3;
				const auto bodyPartIndex = subsetI[bodyPart];
				if (bodyPartIndex > 0)
				{
					// Best results for 1 scale: x + 0, y + 0.5
					// +0.5 to both to keep Matlab format
					poseKeypoints[baseOffset] = peaksPtr[bodyPartIndex - 2] * scaleFactor + 0.5f;
					poseKeypoints[baseOffset + 1] = peaksPtr[bodyPartIndex - 1] * scaleFactor + 0.5f;
					poseKeypoints[baseOffset + 2] = peaksPtr[bodyPartIndex];
				}
				else
				{
					poseKeypoints[baseOffset] = 0.f;
					poseKeypoints[baseOffset + 1] = 0.f;
					poseKeypoints[baseOffset + 2] = 0.f;
				}
			}
			poseScores[person] = subsetPair.second / (float)(numberBodyPartsAndPAFs);
		}
	}
	catch (const std::exception& e)
	{
		throw e.what();
	}
}
