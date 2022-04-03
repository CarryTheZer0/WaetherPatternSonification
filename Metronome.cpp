#include "Metronome.h"

#include "MainComponent.h"

Metronome::Metronome(MainComponent* pParent) :
	m_stepSize(0),
	m_pParent(pParent)
{}

void Metronome::timerCallback()
{
	m_pParent->stepThroughData(m_stepSize);
}