#include "Metronome.h"

#include "MainComponent.h"

Metronome::Metronome(MainComponent* pParent) :
	m_stepSize(0),
	m_pParent(pParent)
{}

/**
 *   timerCallback
 *   called by JUCE::Timer base class - called once for every timer tick
 */
void Metronome::timerCallback()
{
	// process step in the parent class
	m_pParent->stepThroughData(m_stepSize);
}