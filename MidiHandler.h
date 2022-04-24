#pragma once

#include <JuceHeader.h>

#include <map>

// Forward declare when only a referece is stored.
class MainComponent;

/**
 *   class MidiHandler
 *   Handles incoming MIDI messages
 */
class MidiHandler : public juce::MidiInputCallback, public juce::Component
{
public:
	MidiHandler() = delete;
	MidiHandler(MainComponent* pOwner);

	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage &message) override;
	void setMidiInput(juce::AudioDeviceManager* deviceManager);

	void processMessage(const juce::MidiMessage& m);

	bool wasPressed(int noteNo);
	juce::uint8 getVelocity(int noteNo) { return m_velocities[noteNo]; }
private:
	MainComponent* m_pOwner; // refernce to the parent component

	std::map<int, juce::uint8> m_velocities; // map of MIDI message velocities
	std::map<int, bool> m_pressed;			 // map of MIDI messages (true when pressed)
};