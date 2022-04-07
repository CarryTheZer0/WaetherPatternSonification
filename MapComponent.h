#pragma once

#include <JuceHeader.h>

#include "CursorComponent.h"

class MapComponent : public juce::Component
{
public:
    MapComponent() = delete;
    MapComponent(int w, int h);
    ~MapComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPos();
    void addX(float val);
    void addY(float val);
    void addZoom(float val);

    juce::Point<float> getCoords() { return m_coords; }
    float getZoom() { return m_zoom; }
private:
    juce::Image m_image;
    juce::ImageComponent m_imageComp;
    CursorComponent m_cursorComp;
    juce::Point<int> m_pos;
    juce::Point<float> m_coords;
    float m_zoom;
    juce::Rectangle<float> m_coordBounds;

    juce::Point<int> coordsToPixels(juce::Point<float> coords);
    float degreesToPixels(float degrees);
};
