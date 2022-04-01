#pragma once

#include <JuceHeader.h>

class MapComponent : public juce::Component
{
public:
    MapComponent() = delete;
    MapComponent(int w, int h);
    ~MapComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPos(int x, int y);
    void addX(int val);
    void addY(int val);

    juce::Point<int> getPos() { return m_pos; }
private:

    juce::Image m_image;
    juce::Point<int> m_pos;
};
