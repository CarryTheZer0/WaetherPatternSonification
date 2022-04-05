#pragma once

#include <JuceHeader.h>

#include <map>

class MainComponent;

class MidiHandler : public juce::MidiInputCallback, public juce::Component
{
public:
	MidiHandler() = delete;
	MidiHandler(MainComponent* pOwner);

	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage &message) override;
	void setMidiInput(juce::AudioDeviceManager* deviceManager);

	void processMessage(const juce::MidiMessage& m, juce::String& s);

	bool wasPressed(int noteNo);
	juce::uint8 getVelocity(int noteNo) { return m_velocities[noteNo]; }
private:
	MainComponent* m_pOwner;

	std::map<int, juce::uint8> m_velocities;
	std::map<int, bool> m_pressed;
};