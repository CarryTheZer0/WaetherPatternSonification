#include "MidiCallback.h"

/**
 *   messageCallback
 *   sends a signal to the parent class to process a message
 */
void MidiCallback::messageCallback()
{
    if (m_pOwner != nullptr)
        m_pOwner->processMessage(m_message);
}