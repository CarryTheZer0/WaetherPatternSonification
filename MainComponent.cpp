#include "MainComponent.h"

MainComponent::MainComponent() :
    m_midiHandler(this),
    m_mapComp(0, 0),
    m_data("Data\\StormEvents_details-ftp_v1.0_d2010_c20170726.csv"),
    m_startBtton("Button"),
    m_playingSound(false),
    m_metronome(this)
{
    setSize (600, 400);
    setAudioChannels(2, 2);

    juce::Rectangle<int> rect = getLocalBounds();
    m_mapComp.setSize(rect.getWidth() - rect.getWidth() * 0.1, rect.getHeight() - rect.getHeight() * 0.1);
    m_mapComp.setCentrePosition(rect.getCentre());
    m_midiHandler.setMidiInput(&m_deviceManager);
    addAndMakeVisible(m_startBtton);
    addAndMakeVisible(m_mapComp);

    m_startBtton.onClick = [&]()
    {
        std::tm dateTime;
        dateTime.tm_year = 2010 - 1900;
        dateTime.tm_mon = 0;
        dateTime.tm_mday = 0;
        dateTime.tm_hour = 0;
        dateTime.tm_min = 0;

        m_metronome.setTime(std::mktime(&dateTime));
        m_metronome.setStep(86400); // 1 day
        m_metronome.startTimer(10); // every second
    };

    m_oscillator.initialise( [] (float x) { return std::sin(x); }, 128);
    m_oscillator.setFrequency(440);

    m_data.readFile(2010);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> windowRect = getLocalBounds();

    // compute division only once
    int halfWidth = windowRect.getWidth() / 2;
    int halfHeight = windowRect.getHeight() / 2;

    // fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    m_startBtton.setBounds(windowRect);
    m_startBtton.paintButton(g, false, false);
}

void MainComponent::resized()
{
    juce::Rectangle<int> rect = getLocalBounds();
    m_mapComp.setSize(rect.getWidth() - rect.getWidth() * 0.1, rect.getHeight() - rect.getHeight() * 0.1);
    m_mapComp.setCentrePosition(rect.getCentre());
    m_mapComp.resized();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = 100000;
    spec.sampleRate = 44100;
    spec.numChannels = 2;
    m_oscillator.prepare(spec);
    m_mainGain.prepare(spec);
    m_mainGain.setGainLinear(0.01f);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::dsp::AudioBlock<float> audioBlock = juce::dsp::AudioBlock<float>(*bufferToFill.buffer);
    if (m_playingSound)
    {
        // fill buffer with oscillator samples
        m_oscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
        // adjust gain of the buffer
        m_mainGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    }
}

void MainComponent::releaseResources()
{

}

void MainComponent::processMessage(const juce::MidiMessage& m, juce::String& s)
{
    juce::uint8 vel = m.getVelocity();
    if (m.getNoteNumber() == 35 && m.isNoteOn())
        m_mapComp.addX(vel * 0.05f);
    else if (m.getNoteNumber() == 39 && m.isNoteOn())
        m_mapComp.addX(-vel * 0.05f);

    repaint();
}

void MainComponent::stepThroughData(time_t start, time_t end)
{
    std::vector<StormDataItem> myVect = m_data.getDataInRange(start, end);
    if (myVect.size() > 0)
    {
        m_oscillator.reset();
        m_playingSound = true;
    }
    else
        m_playingSound = false;
}