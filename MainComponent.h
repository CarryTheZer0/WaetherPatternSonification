#pragma once

#include <JuceHeader.h>

#include <thread>

#include "MidiHandler.h"
#include "MapComponent.h"

#include "StormData.h"
#include "Metronome.h"

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

    void stepThroughData(time_t step);

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

    std::thread m_loadThread;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
