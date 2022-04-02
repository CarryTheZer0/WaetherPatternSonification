#include "StormData.h"

StormData::StormData(std::string filename) :
	m_lineReader(filename)
{
	const char* header = m_lineReader.next_line();
}

void StormData::readFile(int year)
{
	while (char* line = m_lineReader.next_line()) 
	{
		StormDataItem item;
		
		std::vector<std::string> output = to_vector(line);
		std::tm dateTime{};

		// try catch block to catch invalid data entry i.e blank longitude and latitude
		try {
			// process time data
			dateTime.tm_year = year - 1900;
			dateTime.tm_mon = getMonth(output[0]);
			dateTime.tm_mday = std::stoi(output[1]);
			dateTime.tm_hour = getHour(output[2]);
			dateTime.tm_min = getMin(output[2]);

			// set location data
			item.longitude = getPositionDataPoint(output[45]);
			item.latitude = getPositionDataPoint(output[44]);
			// set time data
			item.dateTime = dateTime;
			// set severity TODO not yet added
			item.severity = 1;
		}
		catch (const std::exception)
		{
			// skip erroneous data - no need to throw the exception
			continue;
		}

		time_t testTime = std::mktime(&dateTime);

		m_data.emplace(std::mktime(&dateTime), item);
	}

	return;
}
 
int StormData::getMonth(std::string yearMonth)
{
	// get last two characters where format = YYYYMM
	std::string mon = "";
	mon += yearMonth.at(4);
	mon += yearMonth.at(5);

	return std::stoi(mon);
}

int StormData::getHour(std::string timeOfDay)
{
	// get first two characters where format = HHMM
	std::string hour = "";
	hour += timeOfDay.at(0);
	hour += timeOfDay.at(1);

	return std::stoi(hour);
}

int StormData::getMin(std::string timeOfDay)
{
	// get last two characters where format = HHMM
	std::string min = "";
	min += timeOfDay.at(2);
	min += timeOfDay.at(3);

	return std::stoi(min);
}

float StormData::getPositionDataPoint(std::string dataPoint)
{
	std::string output;
	dataPoint += '\n';
	
	int index = 0;
	while (dataPoint[index] != '\n')
	{
		if (dataPoint[index] == '\"')
		{
			index++;
			continue;
		}
		output += dataPoint[index];
		index++;
	}

	return std::stof(output);
}

std::vector<std::string> StormData::to_vector(char* line)
{
	std::vector<std::string> vect;

	int index = 0;
	std::string current = "";
	while (line[index] != '\n')
	{
		if (line[index] == ',')
		{
			vect.push_back(current);
			current = "";
			index++;
			continue;
		}
		current += line[index];
		index++;
	}

	return vect;
}

void StormData::set_line(unsigned int lineNo)
{
	m_lineReader.set_file_line(lineNo + 1);
}

std::vector<StormDataItem> StormData::getDataInRange(time_t start, time_t end)
{
	std::vector<StormDataItem> outVect;
	auto it = m_data.lower_bound(start);

	while (it != m_data.end() && it->first < end)
	{
		outVect.push_back(it->second);
		it++;
	}

	return outVect;
}