#pragma once

#include "csv.h"

#include <JuceHeader.h>

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "StormDataItem.h"

class StormData
{
public:
	StormData() = delete;
	StormData(std::string filename, int year);
	~StormData() = default;

	void readFile();

	int getMonth(std::string yearMonth);
	int getHour(std::string timeOfDay);
	int getMin(std::string timeOfDay);

	bool getIsLoaded() { return m_isLoaded; }
	bool getEndOfDataReached() { return m_endOfDataReached; }
	void resetDate();

	float getPositionDataPoint(std::string dataPoint);

	std::vector<StormDataItem> stepThroughData(time_t step);

	std::vector<std::string> to_vector(char* line);
private:
	std::string m_filename;
	bool m_isLoaded;
	int m_lineCount;
	bool m_endOfDataReached;
	time_t m_currentTime;
	int m_year;

	std::map<time_t, StormDataItem> m_data;
};
