#pragma once

#include <ctime>

struct StormDataItem
{
	std::tm dateTime;
	float longitude;
	float latitude;
	int severity;
};