#include "MidiCallback.h"

void MidiCallback::messageCallback()
{
    if (m_owner != nullptr)
        m_owner->processMessage(m_message, m_source);
}