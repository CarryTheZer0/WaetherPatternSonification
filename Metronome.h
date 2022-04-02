#pragma once

#include <JuceHeader.h>

#include <ctime>

// Forward declare when only a reference is stored
class MainComponent;

class Metronome : public juce::Timer
{
public:
	Metronome() = delete;
	Metronome(MainComponent* pParent);
	~Metronome() = default;

	void timerCallback() override;

	void setTime(time_t newTime) { m_currentTime = newTime; }
	void setStep(time_t newStep) { m_stepSize = newStep; }
private:
	time_t m_stepSize;
	time_t m_currentTime;

	MainComponent* m_pParent;
};