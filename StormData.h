#pragma once

#include "csv.h"

#include <JuceHeader.h>

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "StormDataItem.h"

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
	bool getEndOfDataReached() { return m_endOfDataReached; }
	void resetDate();

	void stop() { m_stop = true; }

	float getPositionDataPoint(std::string dataPoint);

	std::vector<StormDataItem> stepThroughData(time_t step);

	std::vector<std::string> to_vector(char* line);
private:
	std::string m_filename;
	bool m_isLoaded;
	int m_lineCount;
	int m_currentLine;
	bool m_endOfDataReached;
	bool m_stop;
	time_t m_currentTime;
	int m_year;
	double m_progress;
	juce::ProgressBar m_progressBar;

	std::map<time_t, StormDataItem> m_data;
};
