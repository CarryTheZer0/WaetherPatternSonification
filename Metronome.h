#pragma once

#include <JuceHeader.h>

#include <ctime>

// Forward declare when only a reference is stored
class MainComponent;

/**
 *  class Metronome
 *  Timer to step through data
 */
class Metronome : public juce::Timer
{
public:
	Metronome() = delete;
	Metronome(MainComponent* pParent);
	~Metronome() = default;

	void timerCallback() override;

	void setStep(time_t newStep) { m_stepSize = newStep; }
private:
	time_t m_stepSize;			// increase in time per tick
	MainComponent* m_pParent;	// parent component reference for callbacks
};