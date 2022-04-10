#include "StormData.h"

StormData::StormData(std::string filename, int year) :
	m_filename(filename),
	m_isLoaded(false),
	m_lineCount(0),
	m_year(year),
	m_currentTime(0),
	m_stop(false),
	m_progress(0.0),
	m_progressBar(m_progress)
{
	// get line count
	io::LineReader reader(m_filename);
	// skip header
	reader.next_line();
	// do the count
	while (char* line = reader.next_line())
		m_lineCount++;

	resetDate();

	m_progressBar.setBounds(juce::Rectangle<int>(5, 5, 90,90));
	addAndMakeVisible(m_progressBar);
}

void StormData::paint(juce::Graphics& g)
{
	// set draw colour
	g.setColour(juce::Colour(100, 100, 100));

	// fill background
	juce::Rectangle<int> areaRect = getLocalBounds();

	if (!m_isLoaded)
		g.fillRoundedRectangle(areaRect.getX(), areaRect.getY(), areaRect.getWidth(), areaRect.getHeight(), 20);
	else
		setVisible(false);
}

void StormData::resized()
{
	m_progressBar.resized();
}

float StormData::getFrequency()
{
	time_t equinox = 6739200;
	time_t yearInS = 31556952;
	float pi = 3.14159f;

	// convert time since the spring equinox to the range 0 - 2pi
	float progress = (float)((m_currentTime - equinox) % yearInS) / (float)yearInS * pi * 2.0f;

	// find the sine and shift into the range 0 - 1
	float freq = (sin(progress) + 1) * 0.5f;

	// Return a value in the range 220Hz - 440Hz
	freq *= 220.0f;
	freq += 220.0f;
	return freq;
}

void StormData::readFile()
{
	io::LineReader reader(m_filename);
	// skip header
	reader.next_line();
	m_currentLine = 0;

	while (char* line = reader.next_line())
	{
		if (m_stop)
			break;

		StormDataItem item{};
		
		std::vector<std::string> output = to_vector(line);
		std::tm dateTime{};

		// try catch block to catch invalid data entry i.e blank longitude and latitude
		try {
			// process time data
			dateTime.tm_year = m_year - 1900;
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
			m_currentLine++;
			continue;
		}

		time_t testTime = std::mktime(&dateTime);

		m_data.emplace(std::mktime(&dateTime), item);
		m_currentLine++;
		m_progress = (double)m_currentLine / (double)m_lineCount;
	}

	m_isLoaded = true;
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

std::vector<StormDataItem> StormData::stepThroughData(time_t step)
{
	std::vector<StormDataItem> outVect;
	auto it = m_data.lower_bound(m_currentTime);

	m_currentTime += step;

	while (it != m_data.end() && it->first < m_currentTime)
	{
		outVect.push_back(it->second);
		it++;
	}

	if (it == m_data.end())
		resetDate();
	//m_endOfDataReached = true;

	return outVect;
}

void StormData::resetDate()
{
	// set start time
	std::tm dateTime{};
	dateTime.tm_year = m_year - 1900; // years from 1900 as per tm strcut definition
	dateTime.tm_mon = 0;
	dateTime.tm_mday = 0;
	dateTime.tm_hour = 0;
	dateTime.tm_min = 0;
	// convert to epoch time
	m_currentTime = std::mktime(&dateTime);
}