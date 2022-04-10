#pragma once

#include <JuceHeader.h>

/**
 *  class CursorComponent
 *  Draws the cursor over the map
 */
class CursorComponent : public juce::Component
{
public:
    CursorComponent() = delete;
    CursorComponent(int w, int h);
    ~CursorComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPos(int x, int y);
    void setZoom(int radius);

    juce::Point<int> getPos() { return m_pos; } // getter for m_pos
private:
    juce::Point<int> m_pos;  // centre position in pixels
    int m_radius;            // radius in pixels
};
