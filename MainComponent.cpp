#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
    m_pos(0),
    m_midiHandler(this)
{
    setSize (600, 400);
    m_midiHandler.setMidiInput(&m_deviceManager);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    //juce::Line<float> myLine = juce::Line<float>(0.0f, 0.0f, 100.0f, 100.0f);
    //g.drawDashedLine(myLine, m_dashLengths, 50, 1.0f, 0);
    juce::Rectangle<int> rect = getLocalBounds();
    rect.setX(rect.getX() + m_pos);
    g.drawText ("Hello World!", rect, juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::processMessage(const juce::MidiMessage& m, juce::String& s)
{
    m_pos += 10.0f;
}