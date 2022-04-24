#include "MidiHandler.h"
#include "MidiCallBack.h"
#include "MainComponent.h"

MidiHandler::MidiHandler(MainComponent* pOwner) :
    m_pOwner(pOwner)
{}

/**
 *   handleIncomingMidiMessage
 *   sends a MIDI callback message to the JUCE event queue
 */
void MidiHandler::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    // post a new callback message to the queue
    (new MidiCallback(this, message))->post();
}

/**
 *   setMidiInput
 *   detects MIDI devices and assigns one from which to detect messages,
 *   deviceManager is passed in from MainComponent.cpp
 */
void MidiHandler::setMidiInput(juce::AudioDeviceManager* deviceManager)
{
    // get list of MIDI devices
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    // get the first available device
    auto midiInput = midiInputs[0];

    // in not enabled, enable the device
    if (!deviceManager->isMidiInputDeviceEnabled(midiInput.identifier))
        deviceManager->setMidiInputDeviceEnabled(midiInput.identifier, true);

    // add the selected device to the device manager
    deviceManager->addMidiInputDeviceCallback(midiInput.identifier, this);
}

/**
 *   processMessage
 *   caled when a message is recieved
 */
void MidiHandler::processMessage(const juce::MidiMessage& m)
{
    juce::uint8 vel = m.getVelocity();
    if (m.isNoteOn())
    {
        // store velocity, set pressed bool to true
        m_velocities[m.getNoteNumber()] = vel;
        m_pressed[m.getNoteNumber()] = true;
    }
    else if (m.isNoteOff())
    {
        // set velocity to zero
        m_velocities[m.getNoteNumber()] = 0;
    }

    // signal to MainComponent to process the message
    m_pOwner->onMessage();
}

/**
 *   wasPressed
 *   returns true if a MIDI signal was received
 */
bool MidiHandler::wasPressed(int noteNo)
{
    // get pressed bool
    bool pressed = m_pressed[noteNo];
    if (pressed)
        // set pressed to false, so only one signal is ever sent per press
        m_pressed[noteNo] = false;
    return pressed;
}