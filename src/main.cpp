// File:  main.cpp
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Optimization for change gears for cutting metric thread pitches on a standard lathe.

// Local headers
#include "latheChangeGearsConfigFile.h"
#include "ratioSolver.h"

// Standard C++ headers
#include <iostream>
#include <sstream>

struct Arguments
{
	std::string configFileName;
	double desiredPitchMM;// [mm]
};

bool ParseArguments(const int argc, char*argv[], Arguments& arguments)
{
	if (argc != 3)
		return false;

	arguments.configFileName = argv[1];

	const std::string pitchArg(argv[2]);
	const std::string pitchPrefix("--mm=");
	if (pitchArg.substr(0, pitchPrefix.length()) != pitchPrefix)
		return false;

	const std::string pitchString(pitchArg.substr(pitchPrefix.length()));
	std::istringstream ss(pitchString);
	if ((ss >> arguments.desiredPitchMM).fail())
		return false;

	if (arguments.desiredPitchMM <= 0.0)
	{
		std::cerr << "Desired pitch must be strictly positive\n";
		return false;
	}

	return true;
}

void PrintUsage(const std::string& calledAs)
{
	std::cout << "Usage:  " << calledAs << " <config file> --mm=<desired metric pitch>\n" << std::endl;
}

void PrintResults(const RatioSolver::Results& results)
{
	std::cout << "Actual achieved pitch = " << results.actualPitchMM << " mm/thread\n";

	std::cout << "\nDriving gears:\n";
	for (const auto& t : results.drivingGears)
		std::cout << "  " << t << "\n";

	std::cout << "\nDriven gears:\n";
	for (const auto& t : results.drivenGears)
		std::cout << "  " << t << "\n";

	std::cout
		<< "\nPercent Error = " << results.errorPercent
		<< "\nError (mm/thread) = " << results.errorMMPerThread
		<< "\nError (in/thread) = " << results.errorInchPerThread
		<< "\nError (in/ft) = " << results.errorInchPerFoot
		<< std::endl;
}

int main(int argc, char *argv[])
{
	Arguments args;
	if (!ParseArguments(argc, argv, args))
	{
		PrintUsage(argv[0]);
		return 1;
	}

	LatheChangeGearsConfigFile configFile;
	if (!configFile.ReadConfiguration(UString::ToStringType(args.configFileName)))
		return 1;

	RatioSolver solver(configFile.GetConfig());
	if (configFile.GetConfig().availableGears.size() > 1)
	{
		std::cout << "\nUsing only available gear options:\n";
		PrintResults(solver.SolveConstrained(args.desiredPitchMM));
	}

	std::cout << "\nUsing any gears up to " << configFile.GetConfig().maxGearTeeth << " teeth:\n";
	PrintResults(solver.SolveUnconstrained(args.desiredPitchMM));

	return 0;
}
