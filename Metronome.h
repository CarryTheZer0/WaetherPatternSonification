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

	void setStep(time_t newStep) { m_stepSize = newStep; }
private:
	time_t m_stepSize;

	MainComponent* m_pParent;
};