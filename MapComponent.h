#pragma once

#include <JuceHeader.h>

#include "CursorComponent.h"

/**
 *  class MapComponent
 *  Draws the map, controls the cursor
 */
class MapComponent : public juce::Component
{
public:
    MapComponent() = delete;
    MapComponent(int w, int h);
    ~MapComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPos();
    void setX(float val);
    void setY(float val);
    void setZoom(float val);

    juce::Point<float> getCoords() { return m_coords; }
    float getZoom() { return m_zoom; }
private:
    juce::Image m_image;                    // image object for drawing the map
    juce::ImageComponent m_imageComp;       // image child component for drawing the map
    CursorComponent m_cursorComp;           // cursor child component for drawing the cursor
    juce::Point<float> m_coords;            // current corrdinates (longitude, latitude)
    float m_zoom;                           // current zoom (km)
    juce::Rectangle<float> m_coordBounds;   // boundary coordinates (min longitude, min latitude, width, height)

    juce::Point<int> coordsToPixels(juce::Point<float> coords);
    float degreesToPixels(float degrees);

    float analogToCoords(int val, bool isX);
    float analogToDegrees(float val);
};
