#pragma once

#include <JuceHeader.h>

class MainComponent;

class MidiHandler : public juce::MidiInputCallback
{
public:
	MidiHandler() = delete;
	MidiHandler(MainComponent* pOwner);

	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage &message) override;

	void setMidiInput(juce::AudioDeviceManager* deviceManager);

	MainComponent* m_pOwner;
};