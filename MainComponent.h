#pragma once

#include <JuceHeader.h>

#include <thread>

#include "MidiHandler.h"
#include "MapComponent.h"

#include "StormData.h"
#include "Metronome.h"

/**
 *  class MainComponent
 *  Draws the main window, handles sound output and controls the UI
 */
class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override {}

    void paint (juce::Graphics&) override;
    void resized() override;

    void onMessage();
    void togglePlayback();
    void stepThroughData(time_t step);

private:
    juce::AudioDeviceManager m_deviceManager; // MIDI device manager
    MidiHandler m_midiHandler; // MIDI input handler
    MapComponent m_mapComp;    // map child component
    StormData m_data;          // data child component
    Metronome m_metronome;     // metronome

    juce::Label m_longLabel;   // labels for the UI
    juce::Label m_latLabel;
    juce::Label m_title;

    juce::dsp::Oscillator<float> m_oscillator;        // storm data oscillator
    juce::dsp::Oscillator<float> m_seasonOscillator;  // time of year oscillator
    juce::dsp::Gain<float> m_mainGain;                // gain control

    std::thread m_loadThread;  // thread for loading data

    // enum to track control loop stage
    enum class ControlState {
        LONGITUDE, LATITUDE, ZOOM, PLAYBACK
    };
    ControlState m_state = ControlState::LONGITUDE;  
    
    float m_playbackSpeed;     // current playback speed  (interval between metronome ticks)
    bool m_playingSound;       // flags for sound output
    bool m_isMetronomeOn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
