#include <JuceHeader.h>
#include "MidiHandler.h"

#pragma once

/**
 *   class MidiCallback
 *   callback message added to the JUCE backend's message queue
 */
class MidiCallback : public juce::CallbackMessage
{
public:
    MidiCallback(MidiHandler* o, const juce::MidiMessage& m)
        : m_pOwner(o), m_message(m)
    {}

    void messageCallback() override;

private:
    juce::Component::SafePointer<MidiHandler> m_pOwner; // refernce to the parent class that created the callback
    juce::MidiMessage m_message;    // MIDI message content
};