#include <JuceHeader.h>
#include "MainComponent.h"

#pragma once

class MidiCallback : public juce::CallbackMessage
{
public:
    MidiCallback(MainComponent* o, const juce::MidiMessage& m, const juce::String& s)
        : m_owner(o), m_message(m), m_source(s)
    {}

    void messageCallback() override;

private:
    juce::Component::SafePointer<MainComponent> m_owner;
    juce::MidiMessage m_message;
    juce::String m_source;
};