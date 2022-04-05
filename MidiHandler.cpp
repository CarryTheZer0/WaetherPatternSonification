#include "MidiHandler.h"
#include "MidiCallBack.h"
#include "MainComponent.h"

MidiHandler::MidiHandler(MainComponent* pOwner) :
    m_pOwner(pOwner)
{}

void MidiHandler::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    (new MidiCallback(this, message, source->getName()))->post();
}

void MidiHandler::setMidiInput(juce::AudioDeviceManager* deviceManager)
{
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    auto midiInput = midiInputs[0];

    if (!deviceManager->isMidiInputDeviceEnabled(midiInput.identifier))
        deviceManager->setMidiInputDeviceEnabled(midiInput.identifier, true);

    deviceManager->addMidiInputDeviceCallback(midiInput.identifier, this);
}

void MidiHandler::processMessage(const juce::MidiMessage& m, juce::String& s)
{
    juce::uint8 vel = m.getVelocity();
    if (m.isNoteOn())
    {
        m_velocities[m.getNoteNumber()] = vel;
        m_pressed[m.getNoteNumber()] = true;
    }
    else if (m.isNoteOff())
    {
        m_velocities[m.getNoteNumber()] = 0;
    }

    m_pOwner->onMessage();
}

bool MidiHandler::wasPressed(int noteNo)
{
    bool pressed = m_pressed[noteNo];
    if (pressed)
        m_pressed[noteNo] = false;
    return pressed;
}