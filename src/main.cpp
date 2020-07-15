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
#include <iomanip>

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

void PrintInColumns(const std::vector<std::string>& values, const std::vector<unsigned int>& widths)
{
	assert(values.size() == widths.size());

	std::ios_base::fmtflags flags(std::cout.flags());

	for (unsigned int i = 0; i < values.size(); ++i)
		std::cout << std::setw(widths[i]) << std::setfill(' ') << values[i];
	std::cout << '\n';

	std::cout.flags(flags);
}

void PrintResults(const std::vector<RatioSolver::Results>& results)
{
	const std::vector<unsigned int> widths = { 17, 15, 15, 15, 15 };
	const std::vector<std::string> headings = { "Actual pitch (mm)", "Driving Gears", "Driven Gears", "Error(%)", "Error(in/ft)" };
	PrintInColumns(headings, widths);

	for (const auto& r : results)
	{
		std::vector<std::string> data(widths.size());
		std::ostringstream ss;
		ss << r.actualPitchMM;
		data[0] = ss.str();

		ss.clear();
		ss.str("");
		for (auto& g : r.drivingGears)
		{
			if (!ss.str().empty())
				ss << ", ";
			ss << g;
		}
		data[1] = ss.str();

		ss.clear();
		ss.str("");
		for (auto& g : r.drivenGears)
		{
			if (!ss.str().empty())
				ss << ", ";
			ss << g;
		}
		data[2] = ss.str();

		ss.clear();
		ss.str("");
		ss << r.errorPercent;
		data[3] = ss.str();

		ss.clear();
		ss.str("");
		ss << r.errorInchPerFoot;
		data[4] = ss.str();

		PrintInColumns(data, widths);
	}

	std::cout << std::endl;
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
		PrintResults(solver.SolveAvailable(args.desiredPitchMM));
	}

	std::cout << "\nUsing availalbe gear options plus any one additional gear up to " << configFile.GetConfig().maxGearTeeth << " teeth:\n";
	PrintResults(solver.SolveAvailablePlus(args.desiredPitchMM));

	return 0;
}
