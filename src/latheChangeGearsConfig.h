// File:  latheChangeGearsConfig.h
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Configuration parameters for Lathe Change Gears app.

#ifndef LATHE_CHANGE_GEARS_CONFIG_H_
#define LATHE_CHANGE_GEARS_CONFIG_H_

// Standard C++ headers
#include <vector>

struct LatheChangeGearsConfig
{
	std::vector<unsigned int> availableGears;
	unsigned int maxReductions;// maximum selected gears = 2 * maxReductions
	unsigned int maxGearTeeth;
	double lead;// [rev/in]
};

#endif// LATHE_CHANGE_GEARS_CONFIG_H_
