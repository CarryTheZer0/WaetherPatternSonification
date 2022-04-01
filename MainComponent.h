#pragma once

#include <JuceHeader.h>
#include "MidiHandler.h"
#include "MapComponent.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void processMessage(const juce::MidiMessage& m, juce::String& s);

private:
    juce::AudioDeviceManager m_deviceManager;
    MidiHandler m_midiHandler;
    MapComponent m_mapComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
