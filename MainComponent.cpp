#include "MainComponent.h"

MainComponent::MainComponent() :
    m_midiHandler(this),
    m_mapComp(0, 0),
    m_data("Data\\StormEvents_details-ftp_v1.0_d2010_c20170726.csv", 2010),
    m_startBtton("Button"),
    m_playingSound(false),
    m_metronome(this),
    m_isMetronomeOn(false),
    m_playbackSpeed(100.0f)
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

    m_seasonOscillator.initialise( [] (float x) { return std::sin(x); }, 128);

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
    m_oscillator.setFrequency(440);
    m_seasonOscillator.prepare(spec);
    m_seasonOscillator.setFrequency(440);
    m_mainGain.prepare(spec);
    m_mainGain.setGainLinear(0.01f);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::dsp::AudioBlock<float> audioBlock = juce::dsp::AudioBlock<float>(*bufferToFill.buffer);
    if (m_isMetronomeOn) {
        if (m_playingSound)
        {
            // fill buffer with oscillator samples
            m_oscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
        }
        // fill buffer with oscillator samples
        m_seasonOscillator.setFrequency(m_data.getFrequency());
        m_seasonOscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    }
    // adjust gain of the buffer
    m_mainGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

void MainComponent::releaseResources()
{

}

void MainComponent::onMessage()
{
    // move cursor
    int increase = m_midiHandler.getVelocity(36);
    int decrease = m_midiHandler.getVelocity(35);
    switch (m_state)
    {
        // Advance to latitude selection
        case ControlState::LONGITUDE: 
            m_mapComp.addX(increase * 0.005f);
            m_mapComp.addX(decrease * -0.005f);
            break;
        // Advance to zoom selection
        case ControlState::LATITUDE:
            m_mapComp.addY(increase * 0.005f);
            m_mapComp.addY(decrease * -0.005f);
            break;
        // Advance to playback control
        case ControlState::ZOOM:
            m_mapComp.addZoom(increase * 0.002f);
            m_mapComp.addZoom(decrease * -0.002f);
            break;
        case ControlState::PLAYBACK:
            m_playbackSpeed -= increase * 0.02f;
            m_playbackSpeed += decrease * 0.02f;
            m_metronome.stopTimer();
            m_metronome.startTimer(m_playbackSpeed);
            break;
    }
    
    // on enter pressed - advance selection stage
    if (m_midiHandler.wasPressed(37))
    {
        switch (m_state)
        {
            // Advance to latitude selection
            case ControlState::LONGITUDE: m_state = ControlState::LATITUDE; break;
            // Advance to zoom selection
            case ControlState::LATITUDE: m_state = ControlState::ZOOM; break;
            // Advance to playback control, start playback
            case ControlState::ZOOM: 
                m_state = ControlState::PLAYBACK; 
                togglePlayback();
                break;
            // Play/Pause until reset is pressed
            case ControlState::PLAYBACK: togglePlayback(); break;
        }
    }

    if (m_midiHandler.wasPressed(38))
    {
        m_metronome.stopTimer();
        m_playingSound = false;
        m_isMetronomeOn = false;
        m_state = ControlState::LONGITUDE;
    }

    repaint();
}

void MainComponent::togglePlayback()
{
    if (m_data.getIsLoaded() && !m_isMetronomeOn)
    {
        m_metronome.startTimer(m_playbackSpeed);
        m_isMetronomeOn = true;
    }
    else if (m_data.getIsLoaded() && m_isMetronomeOn)
    {
        m_metronome.stopTimer();
        m_playingSound = false;
        m_isMetronomeOn = false;
    }
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
        if (juce::Point<float>(item.longitude, item.latitude).getDistanceFrom(coords) < m_mapComp.getZoom())
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