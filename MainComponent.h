#pragma once

#include <JuceHeader.h>
#include "MidiHandler.h"
#include "MapComponent.h"

#include "StormData.h"
#include "Metronome.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void processMessage(const juce::MidiMessage& m, juce::String& s);

    void stepThroughData(time_t start, time_t end);

private:
    juce::AudioDeviceManager m_deviceManager;
    MidiHandler m_midiHandler;
    MapComponent m_mapComp;
    StormData m_data;
    Metronome m_metronome;

    juce::TextButton m_startBtton;
    juce::dsp::Oscillator<float> m_oscillator;
    juce::dsp::Gain<float> m_mainGain;
    bool m_playingSound;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
