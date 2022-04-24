#pragma once

#include <ctime>

/**
 *   struct StormDataItem
 *   stored the relevent data value after processing the larger dataset
 */
struct StormDataItem
{
	std::tm dateTime; // tm object storing the date and time of the storm
	float longitude;  // longitude of the storm's start point
	float latitude;   // longitude of the storm's end point
	int severity;     // severity value (1 = no reported injuries, 2 = injuries were reported, 3 = deaths were reported)
};