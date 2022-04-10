#include "MapComponent.h"

MapComponent::MapComponent(int w, int h) :
    m_imageComp("MapImage"),
    m_cursorComp(w, h),
    m_coords(-95.0f, 37.0f),
    m_zoom(3.0f),
    m_coordBounds(-126.067436f, 49.655963f, 126.067436f - 65.888768f, 49.655963f - 24.108747f)
{
    setSize(w, h);
    juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile("Resources/USMap.PNG");
    m_image = juce::ImageFileFormat::loadFrom(file);

    addAndMakeVisible(m_imageComp);
    addAndMakeVisible(m_cursorComp);
    m_imageComp.setImage(m_image, juce::RectanglePlacement(356));
}

void MapComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    m_imageComp.paint(g);
    m_cursorComp.paint(g);
}

void MapComponent::resized()
{
    m_imageComp.setBounds(getLocalBounds());
    m_cursorComp.setBounds(getLocalBounds());
    m_imageComp.resized();
    m_cursorComp.resized();
}

void MapComponent::setPos()
{
    juce::Point<int> pos = coordsToPixels(m_coords);
    int radius = degreesToPixels(m_zoom);
    m_cursorComp.setPos(pos.x, pos.y);
    m_cursorComp.setZoom(radius);
}

void MapComponent::setX(float val)
{
    m_coords.x = analogToCoords(val, true);
    setPos();
}

void MapComponent::setY(float val)
{
    m_coords.y = analogToCoords(val, false);
    setPos();
}

void MapComponent::setZoom(float val)
{
    m_zoom = analogToDegrees(val);
    setPos();
}

juce::Point<int> MapComponent::coordsToPixels(juce::Point<float> coords)
{
    float xPercent = (coords.x - m_coordBounds.getX()) / m_coordBounds.getWidth();
    int xPix = xPercent * getLocalBounds().getWidth();
    float yPercent = (coords.y - m_coordBounds.getY()) / m_coordBounds.getHeight();
    int yPix = yPercent * getLocalBounds().getHeight();
    return juce::Point<int>(xPix, -yPix);
}

float MapComponent::degreesToPixels(float degrees)
{
    return getWidth() / (m_coordBounds.getWidth() / degrees);
}

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

float MapComponent::analogToDegrees(float val)
{
    float maxZoom = 10.0f;
    return (val / 127.0f) * maxZoom;
}