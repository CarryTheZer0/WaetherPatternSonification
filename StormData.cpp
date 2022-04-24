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

	// initialise progress bar
	m_progressBar.setBounds(juce::Rectangle<int>(5, 5, 90,90));
	addAndMakeVisible(m_progressBar);
}

/**
 *   paint
 *   called by JUCE::Component base class - draws the UI
 */
void StormData::paint(juce::Graphics& g)
{
	// set draw colour
	g.setColour(juce::Colour(100, 100, 100));

	// fill background
	juce::Rectangle<int> areaRect = getLocalBounds();
	if (!m_isLoaded) // only draw while loading is taking place
		g.fillRoundedRectangle(areaRect.getX(), areaRect.getY(), areaRect.getWidth(), areaRect.getHeight(), 20);
	else
		setVisible(false);
}

/**
 *   resized
 *   called by JUCE::Component base class - called when the window is resized
 */
void StormData::resized()
{
	m_progressBar.resized();
}

/**
 *   getFrequency
 *   returns a frequency corresponding to time of year where the summer solstace is 
 *   high and the winter solstace is low
 */
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

/**
 *   readFile
 *   Reads data from the csv file and stores relevent information
 */
void StormData::readFile()
{
	io::LineReader reader(m_filename);
	// skip header
	reader.next_line();
	m_currentLine = 0;

	// loop through file
	while (char* line = reader.next_line())
	{
		if (m_stop) // check abort flag
			break;

		// initialise datum
		StormDataItem item{};
		std::tm dateTime{};

		// read data
		std::vector<std::string> output = toVector(line);

		// try catch block to catch invalid data entry i.e blank longitude and latitude
		try {
			// process time data
			dateTime.tm_year = m_year - 1900;
			dateTime.tm_mon = getMonth(output[0]);
			dateTime.tm_mday = std::stoi(output[1]);
			dateTime.tm_hour = getHour(output[2]);
			dateTime.tm_min = getMin(output[2]);

			// set injury data
			int injuries = getPositionDataPoint(output[20]) + getPositionDataPoint(output[21]);
			int deaths = getPositionDataPoint(output[22]) + getPositionDataPoint(output[23]);;
			// set location data
			item.longitude = getPositionDataPoint(output[45]);
			item.latitude = getPositionDataPoint(output[44]);
			// set time data
			item.dateTime = dateTime;
			// set severity
			item.severity = 1;
			if (injuries > 0) item.severity = 2;
			if (deaths > 0) item.severity = 3;
		}
		catch (const std::exception)
		{
			// skip erroneous data - no need to throw the exception
			m_currentLine++;
			continue;
		}

		// add data to map
		m_data.emplace(std::mktime(&dateTime), item);
		m_currentLine++;
		// update progress bar
		m_progress = (double)m_currentLine / (double)m_lineCount;
	}

	m_isLoaded = true;
	return;
}
 
/**
 *   getMonth
 *   Reads month data, converting from string to int
 */
int StormData::getMonth(std::string yearMonth)
{
	// get last two characters where format = YYYYMM
	std::string mon = "";
	mon += yearMonth.at(4);
	mon += yearMonth.at(5);

	return std::stoi(mon);
}

/**
 *   getHour
 *   Reads hour data, converting from string to int
 */
int StormData::getHour(std::string timeOfDay)
{
	// get first two characters where format = HHMM
	std::string hour = "";
	hour += timeOfDay.at(0);
	hour += timeOfDay.at(1);

	return std::stoi(hour);
}

/**
 *   getMin
 *   Reads minute data, converting from string to int
 */
int StormData::getMin(std::string timeOfDay)
{
	// get last two characters where format = HHMM
	std::string min = "";
	min += timeOfDay.at(2);
	min += timeOfDay.at(3);

	return std::stoi(min);
}

/**
 *   getPositioDataPoint
 *   Reads position data - this is stored in the csv file with quote marks that must be removed i.e. "2" -> 2
 */
float StormData::getPositionDataPoint(std::string dataPoint)
{
	std::string output;
	// append escape character
	dataPoint += '\n';
	
	// read each char
	int index = 0;
	while (dataPoint[index] != '\n')
	{
		// ignore quote marks
		if (dataPoint[index] == '\"')
		{
			index++;
			continue;
		}
		// read other chars to output
		output += dataPoint[index];
		index++;
	}

	// convert to float
	return std::stof(output);
}

/**
 *   toVector
 *   converts single string of csv data to a vector
 */
std::vector<std::string> StormData::toVector(char* line)
{
	std::vector<std::string> vect;

	int index = 0;
	std::string current = "";  // string 'current' stores data as the line is read
	// loop through line
	while (line[index] != '\n')
	{
		// upon finding a comma add data to the vector
		if (line[index] == ',')
		{
			vect.push_back(current);
			// reset 'current' to empty string
			current = "";
			index++;
			continue;
		}
		// add char to 'current'
		current += line[index];
		index++;
	}

	return vect;
}

/**
 *   stepThroughData
 *   Finds relevent data for one step (one day)
 */
std::vector<StormDataItem> StormData::stepThroughData(time_t step)
{
	std::vector<StormDataItem> outVect;
	// initialise iterator at the current time
	auto it = m_data.lower_bound(m_currentTime);

	// add one day to the current time
	m_currentTime += step;

	// add data points to the output until the iterator passes the new current time value
	while (it != m_data.end() && it->first < m_currentTime)
	{
		outVect.push_back(it->second);
		it++;
	}

	// loop the year when the end is reached
	if (it == m_data.end())
		resetDate();

	return outVect;
}

/**
 *   resetDate
 *   set date back to 1st January
 */
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