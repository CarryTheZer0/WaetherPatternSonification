#include "MapComponent.h"

MapComponent::MapComponent(int w, int h) :
    m_imageComp("MapImage"),
    m_cursorComp(w, h),
    // hard-coded data here is the centre of the map in longitude and latitude
    m_coords(-95.0f, 37.0f),
    m_zoom(3.0f),
    // hard-coded data here matches the map image boundaries in longitude and latitude
    m_coordBounds(-126.067436f, 49.655963f, 126.067436f - 65.888768f, 49.655963f - 24.108747f) 
{
    // initialise the image
    setSize(w, h);
    juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile("../../Source/Resources/USMap.PNG");
    m_image = juce::ImageFileFormat::loadFrom(file);

    addAndMakeVisible(m_imageComp);
    addAndMakeVisible(m_cursorComp);
    m_imageComp.setImage(m_image, juce::RectanglePlacement(356));
}

/**
 *   paint
 *   Called by the JUCE::Component base class - draws the UI
 */
void MapComponent::paint(juce::Graphics& g)
{
    // fill the background with a solid colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // draw child components
    m_imageComp.paint(g);
    m_cursorComp.paint(g);
}

/**
 *   resized
 *   Called by the JUCE::Component base class when resized - updates position information
 */
void MapComponent::resized()
{
    m_imageComp.setBounds(getLocalBounds());
    m_cursorComp.setBounds(getLocalBounds());
    m_imageComp.resized();
    m_cursorComp.resized();
}

/**
 *   setPos
 *   converts the distance data to pixels and updates the cursor
 */
void MapComponent::setPos()
{
    juce::Point<int> pos = coordsToPixels(m_coords);
    int radius = degreesToPixels(m_zoom);
    m_cursorComp.setPos(pos.x, pos.y);
    m_cursorComp.setZoom(radius);
}

/**
 *   setX
 *   handles changes in the Y axis
 */
void MapComponent::setX(float val)
{
    m_coords.x = analogToCoords(val, true);
    setPos();
}

/**
 *   setY
 *   handles changes in the X axis
 */
void MapComponent::setY(float val)
{
    m_coords.y = analogToCoords(val, false);
    setPos();
}

/**
 *   setZoom
 *   handles changes to the zoom
 */
void MapComponent::setZoom(float val)
{
    m_zoom = analogToDegrees(val);
    setPos();
}

/**
 *   coordsToPixels
 *   converts position data from real world distance to pixels according to the screen size
 */
juce::Point<int> MapComponent::coordsToPixels(juce::Point<float> coords)
{
    float xPercent = (coords.x - m_coordBounds.getX()) / m_coordBounds.getWidth();
    int xPix = xPercent * getLocalBounds().getWidth();
    float yPercent = (coords.y - m_coordBounds.getY()) / m_coordBounds.getHeight();
    int yPix = yPercent * getLocalBounds().getHeight();
    return juce::Point<int>(xPix, -yPix);
}

/**
 *   degreesToPixels
 *   converts radius data from real world distance to pixels according to the screen size
 */
float MapComponent::degreesToPixels(float degrees)
{
    return getWidth() / (m_coordBounds.getWidth() / degrees);
}

/**
 *   analogToCoords
 *   converts analog coord data from the microcontroller to real world distance
 */
float MapComponent::analogToCoords(int val, bool isX)
{
    if (isX)
    {
        return ((val / 127.0f) * m_coordBounds.getWidth()) + m_coordBounds.getX();
    }
    else
    {
        return ((val / 127.0f) * m_coordBounds.getHeight()) + m_coordBounds.getY() - m_coordBounds.getHeight();
    }

}

/**
 *   analogToDegrees
 *   converts analog zoom data from the microcontroller to real world distance
 */
float MapComponent::analogToDegrees(float val)
{
    float maxZoom = 10.0f;
    return (val / 127.0f) * maxZoom;
}