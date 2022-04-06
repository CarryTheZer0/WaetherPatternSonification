#include "MainComponent.h"

MainComponent::MainComponent() :
    m_midiHandler(this),
    m_mapComp(0, 0),
    m_data("Data\\StormEvents_details-ftp_v1.0_d2010_c20170726.csv", 2010),
    m_startBtton("Button"),
    m_playingSound(false),
    m_metronome(this)
{
    setSize (1259, 682);
    setAudioChannels(2, 2);

    juce::Rectangle<int> rect = getLocalBounds();
    m_mapComp.setSize(rect.getWidth() - rect.getWidth() * 0.1, rect.getHeight() - rect.getHeight() * 0.1);
    m_mapComp.resized();
    m_mapComp.setCentrePosition(rect.getCentre());
    m_midiHandler.setMidiInput(&m_deviceManager);
    m_data.setBounds(rect.getCentreX() - 50, rect.getCentreY() - 50, 100, 100);
    addAndMakeVisible(m_startBtton);
    addAndMakeVisible(m_mapComp);
    addAndMakeVisible(m_data);

    m_metronome.setStep(86400); // 1 day

    m_oscillator.initialise( [] (float x) { return x < 0.0f ? 1.0f : -1.0f; }, 128);
    m_oscillator.setFrequency(440);

    m_seasonOscillator.initialise([](float x) { return std::sin(x); }, 128);
    m_seasonOscillator.setFrequency(440);

    m_loadThread = std::thread([&]() { m_data.readFile(); });
    m_startBtton.setBounds(getLocalBounds());
}

MainComponent::~MainComponent()
{
    m_data.stop();
    m_loadThread.join();
    shutdownAudio();
}

void MainComponent::paint (juce::Graphics& g)
{
    // fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

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
    juce::dsp::ProcessSpec spec{};
    spec.maximumBlockSize = 100000;
    spec.sampleRate = 44100;
    spec.numChannels = 2;
    m_oscillator.prepare(spec);
    m_seasonOscillator.prepare(spec);
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
    }
    // fill buffer with oscillator samples
    m_seasonOscillator.setFrequency(m_data.getFrequency());
    m_seasonOscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    // adjust gain of the buffer
    m_mainGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

void MainComponent::releaseResources()
{

}

void MainComponent::onMessage()
{
    // move cursor
    m_mapComp.addX(m_midiHandler.getVelocity(35) * 0.005f);
    m_mapComp.addX(m_midiHandler.getVelocity(39) * -0.005f);
    
    // start and stop playback
    bool togglePressed = m_midiHandler.wasPressed(36);
    if (togglePressed && m_data.getIsLoaded() && m_isMetronomeOn)
    {
        m_metronome.startTimer(10); // every 100th second
        m_isMetronomeOn = false;
    }
    else if (togglePressed && m_data.getIsLoaded() && !m_isMetronomeOn)
    {
        m_metronome.stopTimer();
        m_playingSound = false;
        m_isMetronomeOn = true;
    }

    repaint();
}

void MainComponent::stepThroughData(time_t step)
{
    if (m_data.getEndOfDataReached())
    {
        m_metronome.stopTimer();
        return;
    }

    std::vector<StormDataItem> myVect = m_data.stepThroughData(step);

    juce::Point<float> coords = m_mapComp.getCoords();

    int count = 0;
    
    for (const StormDataItem &item : myVect)
    {
        if (juce::Point<float>(item.longitude, item.latitude).getDistanceFrom(coords) < 3)
            count++;
    }

    if (count)
    {
        m_oscillator.reset();
        m_playingSound = true;
    }
    else
        m_playingSound = false;
}