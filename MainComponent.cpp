#include "MainComponent.h"

#include <string>

MainComponent::MainComponent() :
    m_midiHandler(this),
    m_mapComp(0, 0),
    m_data("..\\..\\Source\\Resources\\StormEvents_details-ftp_v1.0_d2010_c20170726.csv", 2010),
    m_metronome(this),
    m_longLabel("Long", "Longitude: "),
    m_latLabel("Lat", "Latitude: "),
    m_title("Title", "Select a longitude then press enter: "),
    m_state(ControlState::LONGITUDE),
    m_playingSound(false),
    m_isMetronomeOn(false),
    m_playbackSpeed(100.0f)
{
    setSize(1259, 682);
    setAudioChannels(2, 2);
    juce::Rectangle<int> rect = getLocalBounds();
    
    // MidiHandler instance to handle MIDI input
    m_midiHandler.setMidiInput(&m_deviceManager);

    // set map size and position
    m_mapComp.setSize(getWidth() - getWidth() * 0.2f, getHeight() - getHeight() * 0.2f);
    m_mapComp.setCentrePosition(rect.getCentre());
    m_mapComp.resized();
    addAndMakeVisible(m_mapComp);

    // set data load progress bar bounds
    m_data.setBounds(rect.getCentreX() - 50, rect.getCentreY() - 50, 100, 100);
    addAndMakeVisible(m_data);

    // set longitude axis label
    m_longLabel.setBounds(rect.getCentreX() - 100, getHeight() * 0.94f, 200, getHeight() * 0.02f);
    m_longLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
    m_longLabel.setJustificationType(36); // 36 = centred
    addAndMakeVisible(m_longLabel);

    // set latitude axis label
    m_latLabel.setBounds(getWidth() * 0.04f, rect.getCentreY() + 100, 200, getWidth() * 0.02f);
    m_latLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
    m_latLabel.setTransform(juce::AffineTransform().rotation(4.71239f, getWidth() * 0.04f, rect.getCentreY() + 100)); // rotate 270 degrees
    m_latLabel.setJustificationType(36); // 36 = centred
    addAndMakeVisible(m_latLabel);

    // set title
    m_title.setBounds(getWidth() * 0.1f, 0.0f, getWidth() * 0.6f, getHeight() * 0.1f);
    m_title.setColour(juce::Label::textColourId, juce::Colours::cyan);
    addAndMakeVisible(m_title);

    // initialise metronome step size
    m_metronome.setStep(86400); // 86400 seconds = 1 day

    // initisialise storm data oscillator with a square wave
    m_oscillator.initialise( [] (float x) { return x < 0.0f ? 1.0f : -1.0f; }, 128);

    // initisialise time of year oscillator with a sine wave
    m_seasonOscillator.initialise( [] (float x) { return std::sin(x); }, 128);

    // load data in a separate thread
    // this allows the UI to function while data loads in the background
    m_loadThread = std::thread([&]() { m_data.readFile(); });
}

MainComponent::~MainComponent()
{
    // stop loading data to close thread correctly
    m_data.stop();
    // wait for the thread to close
    m_loadThread.join();
    shutdownAudio();
}

/**
 *   prepareToPlay
 *   called by JUCE::AudioAppComponent base class - prepares sound resources
 */
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // initialise spec
    juce::dsp::ProcessSpec spec{};
    spec.maximumBlockSize = 10000;
    spec.sampleRate = 44100;
    spec.numChannels = 2;

    // prepare oscillators
    m_oscillator.prepare(spec);
    m_oscillator.setFrequency(440);  // m_oscillator plays at 440Hz
    m_seasonOscillator.prepare(spec);
    m_seasonOscillator.setFrequency(440);  // m_seasonOscillator's frequency is changed over time later in the code

    // prepare gain control
    m_mainGain.prepare(spec);
    m_mainGain.setGainLinear(0.01f);  // adjusts gain to set output at a reasonable volume
}


/**
 *   getNextAudioBlock
 *   called by JUCE::AudioAppComponent base class - called repeatedly to fetch blocks of audio data
 */
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // clear old data from the active buffer region
    bufferToFill.clearActiveBufferRegion();

    juce::dsp::AudioBlock<float> audioBlock = juce::dsp::AudioBlock<float>(*bufferToFill.buffer);
    if (m_isMetronomeOn) {
        if (m_playingSound)
        {
            // fill buffer with oscillator samples
            m_oscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
        }
        // set new frequency
        m_seasonOscillator.setFrequency(m_data.getFrequency());
        // fill buffer with oscillator samples
        m_seasonOscillator.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    }
    // adjust gain of the buffer
    m_mainGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

/**
 *   paint
 *   called by JUCE::Component base class - draws the UI
 */
void MainComponent::paint(juce::Graphics& g)
{
    // fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

/**
 *   resized
 *   called by JUCE::Component base class - called when the window is resized
 *   note while this is implemented it is never called due to restrictions on resizing the main window
 */
void MainComponent::resized()
{
    // resize child components
    m_mapComp.setSize(getWidth() - getWidth() * 0.1, getHeight() - getHeight() * 0.1);
    m_mapComp.setCentrePosition(getLocalBounds().getCentre());
    m_mapComp.resized();
}

/**
 *   onMessage
 *   processes incoming messages from the midiHandler class 
 */
void MainComponent::onMessage()
{
    // move cursor
    int value = m_midiHandler.getVelocity(40);
    // switch statement to change the function of the potentiometer
    switch (m_state)
    {
        // Adjust longitude
        case ControlState::LONGITUDE: 
            m_mapComp.setX(value);
            // display new longitude to UI
            m_longLabel.setText("Longitude: " + std::to_string(m_mapComp.getCoords().getX()), juce::NotificationType::dontSendNotification);
            break;
        // Adjust latitiude
        case ControlState::LATITUDE:
            m_mapComp.setY(value);
            // display new latitude to UI
            m_latLabel.setText("Latitude: " + std::to_string(m_mapComp.getCoords().getY()), juce::NotificationType::dontSendNotification);
            break;
        // Adjust zoom
        case ControlState::ZOOM:
            m_mapComp.setZoom(value);
            break;
        // Control playback
        case ControlState::PLAYBACK:
            float min = 1;
            float range = 50;

            // map value from 0->127 to 51->1
            m_playbackSpeed = ((127.0f - value) / 127.0f) * range + min;

            m_metronome.stopTimer();
            m_metronome.startTimer(m_playbackSpeed);
            break;
    }
    
    // on enter pressed - advance selection stage
    if (m_midiHandler.wasPressed(36))
    {
        switch (m_state)
        {
            // Advance to latitude selection
            case ControlState::LONGITUDE: 
                m_state = ControlState::LATITUDE;
                // update title text
                m_title.setText("Select a latitude then press enter: ", juce::NotificationType::dontSendNotification);
                break;
            // Advance to zoom selection
            case ControlState::LATITUDE: 
                m_state = ControlState::ZOOM;
                // update title text
                m_title.setText("Select a zoom level then press enter: ", juce::NotificationType::dontSendNotification);
                break;
            // Advance to playback control, start playback
            case ControlState::ZOOM: 
                m_state = ControlState::PLAYBACK; 
                togglePlayback();
                // update title text
                m_title.setText("Press enter to play/pause, adjust the playback speed: ", juce::NotificationType::dontSendNotification);
                break;
            // Play/Pause until reset is pressed
            case ControlState::PLAYBACK: togglePlayback(); break;
        }
    }

    // on reset pressed
    if (m_midiHandler.wasPressed(37))
    {
        // reset sound output
        m_metronome.stopTimer();
        m_playingSound = false;
        m_isMetronomeOn = false;

        // reset control loop
        m_state = ControlState::LONGITUDE;
        m_title.setText("Select a longitude then press enter: ", juce::NotificationType::dontSendNotification);
    }

    // signal to update the UI
    repaint();
}

/**
 *   togglePlayback
 *   toggles the metronome
 */
void MainComponent::togglePlayback()
{
    // toggle the metronome
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

/**
 *   stepThroughData
 *   gets data for the current step and adjusts the sound output accordingly
 */
void MainComponent::stepThroughData(time_t step)
{
    // get data for this step
    std::vector<StormDataItem> myVect = m_data.stepThroughData(step);

    // get current cursor coords
    juce::Point<float> coords = m_mapComp.getCoords();

    // count the total number of data points
    int severity = 0;
    for (const StormDataItem &item : myVect)
    {
        if (juce::Point<float>(item.longitude, item.latitude).getDistanceFrom(coords) < m_mapComp.getZoom())
            severity = std::max(severity, item.severity);
    }

    // if data is present, play sound
    if (severity)
    {
        m_oscillator.reset();
        m_oscillator.setFrequency(severity * 220.0f);
        m_playingSound = true;
    }
    else
        m_playingSound = false;
}