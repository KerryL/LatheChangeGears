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

	Results SolveConstrained(const double& pitchMM) const;
	Results SolveUnconstrained(const double& pitchMM) const;

private:
	const LatheChangeGearsConfig config;

	double ComputeDesiredRatio(const double& desiredPitchMM) const;
	double ComputeActualRatio(const std::vector<unsigned int>& drivingGears, const std::vector<unsigned int>& drivenGears) const;

	static void ComputeError(const double& desiredPitchMM, Results& results);
	static std::vector<std::vector<unsigned int>> GeneratePermutations(const unsigned int& length, const unsigned int& base);
	static unsigned int CountCombinations(const unsigned int& length, const unsigned int& base);
};

#endif// RATIO_SOLVER_H_