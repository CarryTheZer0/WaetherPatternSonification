#include "Metronome.h"

#include "MainComponent.h"

Metronome::Metronome(MainComponent* pParent) :
	m_currentTime(0),
	m_stepSize(0),
	m_pParent(pParent)
{}

void Metronome::timerCallback()
{
	m_pParent->stepThroughData(m_currentTime, m_currentTime + m_stepSize);
	m_currentTime += m_stepSize;
}