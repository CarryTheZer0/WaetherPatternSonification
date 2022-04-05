#include <JuceHeader.h>
#include "MidiHandler.h"

#pragma once

class MidiCallback : public juce::CallbackMessage
{
public:
    MidiCallback(MidiHandler* o, const juce::MidiMessage& m, const juce::String& s)
        : m_owner(o), m_message(m), m_source(s)
    {}

    void messageCallback() override;

private:
    juce::Component::SafePointer<MidiHandler> m_owner;
    juce::MidiMessage m_message;
    juce::String m_source;
};