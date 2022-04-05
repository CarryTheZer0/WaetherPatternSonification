#pragma once

#include <JuceHeader.h>

class CursorComponent : public juce::Component
{
public:
    CursorComponent() = delete;
    CursorComponent(int w, int h);
    ~CursorComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPos(int x, int y);

    juce::Point<int> getPos() { return m_pos; }
private:
    juce::Point<int> m_pos;
    int m_radius;
};
