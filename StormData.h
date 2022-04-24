#pragma once

#include "csv.h"

#include <JuceHeader.h>

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "StormDataItem.h"

/**
 *   class StormData
 *   reads data from a csv file and steps through it returning relevent information
 */
class StormData : public juce::Component
{
public:
	StormData() = delete;
	StormData(std::string filename, int year);
	~StormData() = default;

	void paint(juce::Graphics&) override;
	void resized() override;

	void readFile();

	int getMonth(std::string yearMonth);
	int getHour(std::string timeOfDay);
	int getMin(std::string timeOfDay);

	bool getIsLoaded() { return m_isLoaded; }
	void resetDate();
	float getFrequency();

	void stop() { m_stop = true; }

	float getPositionDataPoint(std::string dataPoint);

	std::vector<StormDataItem> stepThroughData(time_t step);

	std::vector<std::string> toVector(char* line);
private:
	std::string m_filename;			  // filename containing the data
	bool m_isLoaded;				  // flag denoting whether loading is finished
	int m_lineCount;				  // total lines in the csv file
	int m_currentLine;				  // current line in the csv file
	bool m_stop;					  // bool flag used to abort loading
	time_t m_currentTime;			  // current point in the data
	int m_year;						  // year that the dataset comes from
	double m_progress;				  // progress of the loading of data
	juce::ProgressBar m_progressBar;  // progress bar child component displaying load progess

	std::map<time_t, StormDataItem> m_data;  // map containing the actual data
};
