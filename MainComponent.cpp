#include "MainComponent.h"

MainComponent::MainComponent() :
    m_midiHandler(this),
    m_mapComp(0, 0)
{
    setSize (600, 400);
    juce::Rectangle<int> rect = getLocalBounds();
    m_mapComp.setSize(rect.getWidth() - rect.getWidth() * 0.1, rect.getHeight() - rect.getHeight() * 0.1);
    m_mapComp.setCentrePosition(rect.getCentre());
    m_midiHandler.setMidiInput(&m_deviceManager);
    addAndMakeVisible(m_mapComp);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> windowRect = getLocalBounds();

    // compute division only once
    int halfWidth = windowRect.getWidth() / 2;
    int halfHeight = windowRect.getHeight() / 2;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    juce::Rectangle<int> rect = getLocalBounds();
    m_mapComp.setSize(rect.getWidth() - rect.getWidth() * 0.1, rect.getHeight() - rect.getHeight() * 0.1);
    m_mapComp.setCentrePosition(rect.getCentre());
    m_mapComp.resized();
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