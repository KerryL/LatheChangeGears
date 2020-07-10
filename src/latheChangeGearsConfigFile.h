// File:  latheChangeGearsConfigFile.h
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Config file class for Lathe Change Gears app.

#ifndef LATHE_CHANGE_GEARS_CONFIG_FILE_H_
#define LATHE_CHANGE_GEARS_CONFIG_FILE_H_

// Utilities headers
#include "utilities/configFile.h"

// Local headers
#include "latheChangeGearsConfig.h"

class LatheChangeGearsConfigFile : public ConfigFile
{
public:
	explicit LatheChangeGearsConfigFile(UString::OStream &outStream = Cout);

	LatheChangeGearsConfig GetConfig() const { return config; }
	
private:
	LatheChangeGearsConfig config;

	void BuildConfigItems() override;
	void AssignDefaults() override;

	bool ConfigIsOK() override;
};

#endif// LATHE_CHANGE_GEARS_CONFIG_FILE_H_
