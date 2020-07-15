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
#include <numeric>

RatioSolver::RatioSolver(const LatheChangeGearsConfig& config) : config(config)
{
}

std::vector<RatioSolver::Results> RatioSolver::SolveAvailable(const double& pitchMM) const
{
	std::vector<Results> results(config.showBestCount);

	const double desiredRatio(ComputeDesiredRatio(pitchMM));
	std::vector<unsigned int> bestDrivingGears;
	std::vector<unsigned int> bestDrivenGears;
	FindBestConfiguration(desiredRatio, config.availableGears, results);
	
	for (auto& r : results)
	{
		const double bestRatio(ComputeActualRatio(r.drivingGears, r.drivenGears));
		r.actualPitchMM = 25.4 / config.lead / bestRatio;
		ComputeError(pitchMM, r);
	}

	return results;
}

std::vector<RatioSolver::Results> RatioSolver::SolveAvailablePlus(const double& pitchMM) const
{
	std::vector<Results> results(config.showBestCount);
	const unsigned int minToothCount(16);
	std::vector<unsigned int> additionalGears(config.maxGearTeeth - minToothCount + 1);
	std::iota(additionalGears.begin(), additionalGears.end(), minToothCount);

	const double desiredRatio(ComputeDesiredRatio(pitchMM));
	for (const auto& plusGear : additionalGears)
	{
		std::vector<unsigned int> availableGears(config.availableGears);
		availableGears.push_back(plusGear);
		FindBestConfiguration(desiredRatio, availableGears, results);
	}

	for (auto& r : results)
	{
		const double bestRatio(ComputeActualRatio(r.drivingGears, r.drivenGears));
		r.actualPitchMM = 25.4 / config.lead / bestRatio;
		ComputeError(pitchMM, r);
	}

	return results;
}

void RatioSolver::FindBestConfiguration(const double& desiredRatio, const std::vector<unsigned int>& availableGears, std::vector<Results>& results) const
{
	// Brute force for now
	for (unsigned int numReductions = 1; numReductions <= config.maxReductions; ++numReductions)
	{
		const auto combinations(GenerateCombinations(2 * numReductions, availableGears.size()));
		const auto driving(GenerateCombinations(numReductions, 2 * numReductions));// indices of c
		for (const auto& c : combinations)
		{
			for (const auto& dcIndices : driving)
			{
				std::vector<unsigned int> drivingGears(numReductions);
				std::vector<unsigned int> drivenGears(numReductions);

				std::vector<unsigned int> dc(dcIndices.size());
				for (unsigned int i = 0; i < dc.size(); ++i)
					dc[i] = c[dcIndices[i]];

				const auto driven(GetRemainingSet(c, dc));// indices of availableGears
				for (unsigned int i = 0; i < dcIndices.size(); ++i)
				{
					drivingGears[i] = availableGears[c[dcIndices[i]]];
					drivenGears[i] = availableGears[driven[i]];
				}

				const double actualRatio(ComputeActualRatio(drivingGears, drivenGears));
				const double absError(fabs(actualRatio - desiredRatio));
				for (unsigned int i = 0; i < results.size(); ++i)
				{
					// Exclude gearsets identical to previously identified sets (can happen if available gears includes more than one of a size)
					if (absError == results[i].errorPercent &&
						drivingGears == results[i].drivingGears &&
						drivenGears == results[i].drivenGears)
						break;

					if (absError < results[i].errorPercent)
					{
						results.insert(results.begin() + i, Results());
						results.erase(results.end() - 1);
						results[i].drivingGears = drivingGears;
						results[i].drivenGears = drivenGears;
						results[i].errorPercent = absError;
						break;
					}
				}
			}
		}
	}
}

void RatioSolver::ComputeError(const double& desiredPitchMM, Results& results)
{
	results.errorMMPerThread = results.actualPitchMM - desiredPitchMM;
	results.errorPercent = results.errorMMPerThread / desiredPitchMM * 100.0;
	results.errorInchPerThread = results.errorMMPerThread / 25.4;
	results.errorInchPerFoot = results.errorInchPerThread / (desiredPitchMM / 25.4) * 12.0;
}

// Convention is ratio = driven gears / driving gears, so larger values indicate a higher
// resulting number of threads per distance (finer thread pitch).
double RatioSolver::ComputeDesiredRatio(const double& desiredPitchMM) const
{
	const double lathePitchMM(25.4 / config.lead);
	return lathePitchMM / desiredPitchMM;
}

double RatioSolver::ComputeActualRatio(const std::vector<unsigned int>& drivingGears, const std::vector<unsigned int>& drivenGears) const
{
	assert(drivingGears.size() == drivenGears.size());// For correctness

	double ratio(1.0);
	for (const auto& g : drivenGears)
		ratio *= g;

	for (const auto& g : drivingGears)
		ratio /= g;

	return ratio;
}

std::vector<std::vector<unsigned int>> RatioSolver::GenerateCombinations(const unsigned int& length, const unsigned int& base)
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
	// Equivalent to "n! / (k! * (n - k)!)"
	return static_cast<unsigned int>(tgamma(base + 1) / (tgamma(length + 1) * tgamma(base - length + 1)) + 0.5);// +0.5 to avoid truncation due to floating point math
}

std::vector<unsigned int> RatioSolver::GetRemainingSet(const std::vector<unsigned int>& fullSet, const std::vector<unsigned int>& alreadyTaken)
{
	std::vector<unsigned int> result(fullSet.size() - alreadyTaken.size());
	std::set_difference(fullSet.begin(), fullSet.end(), alreadyTaken.begin(), alreadyTaken.end(), result.begin());
	return result;
}
