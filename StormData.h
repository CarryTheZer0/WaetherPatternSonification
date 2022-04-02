#pragma once

#include "csv.h"

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "StormDataItem.h"

class StormData
{
public:
	StormData() = delete;
	StormData(std::string filename);
	~StormData() = default;

	void readFile(int year);
	void set_line(unsigned int);

	int getMonth(std::string yearMonth);
	int getHour(std::string timeOfDay);
	int getMin(std::string timeOfDay);

	float getPositionDataPoint(std::string dataPoint);

	std::vector<StormDataItem> getDataInRange(time_t start, time_t end);

	std::vector<std::string> to_vector(char* line);
private:
	io::LineReader m_lineReader;

	std::map<time_t, StormDataItem> m_data;
};
