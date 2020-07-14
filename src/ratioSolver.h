// File:  ratioSolver.h
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Algorithm for change gear ratio optimization.

#ifndef RATIO_SOLVER_H_
#define RATIO_SOLVER_H_

// Local headers
#include "latheChangeGearsConfig.h"

class RatioSolver
{
public:
	RatioSolver(const LatheChangeGearsConfig& config);

	struct Results
	{
		std::vector<unsigned int> drivingGears;
		std::vector<unsigned int> drivenGears;

		double actualPitchMM;// [mm]

		double errorPercent;
		double errorMMPerThread;// [mm]
		double errorInchPerThread;// [in]
		double errorInchPerFoot;// [in/ft]
	};

	Results SolveAvailable(const double& pitchMM) const;
	Results SolveAvailablePlus(const double& pitchMM) const;

private:
	const LatheChangeGearsConfig config;

	double ComputeDesiredRatio(const double& desiredPitchMM) const;
	double ComputeActualRatio(const std::vector<unsigned int>& drivingGears, const std::vector<unsigned int>& drivenGears) const;

	void FindBestConfiguration(const double& desiredRatio, const std::vector<unsigned int>& availableGears,
		double& minAbsError, std::vector<unsigned int>& bestDrivingGears, std::vector<unsigned int>& bestDrivenGears) const;

	static void ComputeError(const double& desiredPitchMM, Results& results);
	static std::vector<std::vector<unsigned int>> GenerateCombinations(const unsigned int& length, const unsigned int& base);
	static unsigned int CountCombinations(const unsigned int& length, const unsigned int& base);
	static std::vector<unsigned int> GetRemainingSet(const std::vector<unsigned int>& fullSet, const std::vector<unsigned int>& alreadyTaken);
};

#endif// RATIO_SOLVER_H_
