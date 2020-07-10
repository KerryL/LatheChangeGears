// File:  ratioSolver.cpp
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Algorithm for change gear ratio optimization.

// Local headers
#include "ratioSolver.h"

// Standard C++ headers
#include <cassert>
#include <algorithm>
#include <string>

RatioSolver::RatioSolver(const LatheChangeGearsConfig& config) : config(config)
{
}

RatioSolver::Results RatioSolver::SolveConstrained(const double& pitchMM) const
{
	Results results;

	// Brute force for now
	const double desiredRatio(ComputeDesiredRatio(pitchMM));
	double minAbsError(10.0 * desiredRatio);// something big to start with
	std::vector<unsigned int> bestDrivingGears;
	std::vector<unsigned int> bestDrivenGears;
	for (unsigned int numReductions = 1; numReductions <= config.maxReductions; ++numReductions)
	{
		const auto permutations(GeneratePermutations(2 * numReductions, config.availableGears.size()));
		std::vector<unsigned int> drivingGears(numReductions);
		std::vector<unsigned int> drivenGears(numReductions);
		for (const auto& p : permutations)
		{
			for (unsigned int i = 0; i < p.size() / 2; ++i)
			{
				drivingGears[i] = config.availableGears[p[i]];
				drivenGears[i] = config.availableGears[p[i + p.size() / 2]];
			}

			const double actualRatio(ComputeActualRatio(drivingGears, drivenGears));
			const double absError(fabs(actualRatio - desiredRatio));
			if (absError < minAbsError)
			{
				bestDrivingGears = drivingGears;
				bestDrivenGears = drivenGears;
				minAbsError = absError;
			}
		}
	}

	const double bestRatio(ComputeActualRatio(bestDrivingGears, bestDrivenGears));
	results.actualPitchMM = bestRatio * config.lead / 25.4;// TODO:  Check this
	ComputeError(pitchMM, results);

	return results;
}

RatioSolver::Results RatioSolver::SolveUnconstrained(const double& pitchMM) const
{
	Results results;
	results.actualPitchMM = 2.0;
	ComputeError(pitchMM, results);

	return results;
}

void RatioSolver::ComputeError(const double& desiredPitchMM, Results& results)
{
	results.errorMMPerThread = results.actualPitchMM - desiredPitchMM;
	results.errorPercent = results.errorMMPerThread / desiredPitchMM * 100.0;
	results.errorInchPerThread = results.errorMMPerThread / 25.4;
	results.errorInchPerFoot = results.errorInchPerThread / (desiredPitchMM / 25.4) * 12.0;
}

// Convention is ratio = driving gears / driven gears, so larger values indicate a higher resulting threads/distance (finer thread pitch)
double RatioSolver::ComputeDesiredRatio(const double& desiredPitchMM) const
{
	const double lathePitchMM(25.4 / config.lead);
	return desiredPitchMM / lathePitchMM;
}

double RatioSolver::ComputeActualRatio(const std::vector<unsigned int>& drivingGears, const std::vector<unsigned int>& drivenGears) const
{
	assert(drivingGears.size() == drivenGears.size());// For correctness

	double ratio(1.0);
	for (const auto& g : drivingGears)
		ratio *= g;

	for (const auto& g : drivenGears)
		ratio /= g;

	return ratio;
}

std::vector<std::vector<unsigned int>> RatioSolver::GeneratePermutations(const unsigned int& length, const unsigned int& base)
{
	std::vector<std::vector<unsigned int>> permutations(CountCombinations(length, base), std::vector<unsigned int>(length));
	std::string bitmask(length, 1);// leading 1's
	bitmask.resize(base, 0);// trailing 0's
	for (auto& p : permutations)
	{
		unsigned int j = 0;
		for (unsigned int i = 0; i < base; ++i)
		{
			if (bitmask[i])
			{
				p[j] = i;
				++j;
			}
		}
		std::prev_permutation(bitmask.begin(), bitmask.end());
	}

	return permutations;
}

unsigned int RatioSolver::CountCombinations(const unsigned int& length, const unsigned int& base)
{
	assert(length <= base);
	return static_cast<unsigned int>(tgamma(base + 1) / (tgamma(length + 1) * tgamma(base - length + 1)));
}
