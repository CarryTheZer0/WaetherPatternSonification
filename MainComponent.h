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
    void onMessage();
    void togglePlayback();

    void stepThroughData(time_t step);

private:
    juce::AudioDeviceManager m_deviceManager;
    MidiHandler m_midiHandler;
    MapComponent m_mapComp;
    StormData m_data;
    Metronome m_metronome;

    juce::Label m_longLabel;
    juce::Label m_latLabel;

    juce::Label m_title;

    juce::dsp::Oscillator<float> m_oscillator;
    juce::dsp::Oscillator<float> m_seasonOscillator;
    juce::dsp::Gain<float> m_mainGain;
    float m_playbackSpeed;
    bool m_playingSound;
    bool m_isMetronomeOn;

    std::thread m_loadThread;

    enum class ControlState {
        LONGITUDE, LATITUDE, ZOOM, PLAYBACK
    };

    ControlState m_state = ControlState::LONGITUDE;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
