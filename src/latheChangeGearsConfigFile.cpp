// File:  latheChangeGearsConfigFile.cpp
// Date:  7/10/2020
// Auth:  K. Loux
// Desc:  Config file class for Lathe Change Gears app.

// Local headers
#include "latheChangeGearsConfigFile.h"

LatheChangeGearsConfigFile::LatheChangeGearsConfigFile(UString::OStream &outStream) : ConfigFile(outStream)
{
}

void LatheChangeGearsConfigFile::BuildConfigItems()
{
	AddConfigItem(_T("GEAR"), config.availableGears);
	AddConfigItem(_T("MAX_REDUCTIONS"), config.maxReductions);
	AddConfigItem(_T("MAX_TEETH"), config.maxGearTeeth);
	AddConfigItem(_T("LEAD"), config.lead);
	AddConfigItem(_T("SHOW_TOP"), config.showBestCount);
}

void LatheChangeGearsConfigFile::AssignDefaults()
{
	config.maxReductions = 0;
	config.maxGearTeeth = 120;
	config.lead = 0;
	config.showBestCount = 10;
}

bool LatheChangeGearsConfigFile::ConfigIsOK()
{
	bool ok(true);
	if (config.maxReductions == 0)
	{
		ok = false;
		outStream << GetKey(config.maxReductions) << " must be specified and must be greater than zero" << std::endl;
	}

	if (config.lead <= 0.0)
	{
		ok = false;
		outStream << GetKey(config.lead) << " must be specified and must be greater than zero" << std::endl;
	}

	if (config.showBestCount == 0)
	{
		ok = false;
		outStream << GetKey(config.showBestCount) << " must be greater than zero" << std::endl;
	}

	return ok;
}
