#include "MapComponent.h"

MapComponent::MapComponent(int w, int h) :
    m_pos(0, 0)
{
    setSize(w, h);
    juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile("USMap.PNG");
    m_image = juce::ImageFileFormat::loadFrom(file);
}

void MapComponent::paint(juce::Graphics& g)
{
    juce::Rectangle<int> windowRect = getLocalBounds();

    // compute division only once
    int halfWidth = windowRect.getWidth() / 2;
    int halfHeight = windowRect.getHeight() / 2;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.drawImageAt(m_image, 0, 0, false);

    // draw four dashed lines from a common point - creates a cross pattern at the centre
    const float dashLengths[2] = { 5.0f, 5.0f };
    juce::Line<float> upLine = juce::Line<float>(halfWidth + m_pos.x, halfHeight + m_pos.y, halfWidth + m_pos.x, 0.0f);
    juce::Line<float> downLine = juce::Line<float>(halfWidth + m_pos.x, halfHeight + m_pos.y, halfWidth + m_pos.x, windowRect.getHeight());
    juce::Line<float> leftLine = juce::Line<float>(halfWidth + m_pos.x, halfHeight + m_pos.y, 0.0f, halfHeight + m_pos.y);
    juce::Line<float> rightLine = juce::Line<float>(halfWidth + m_pos.x, halfHeight + m_pos.y, windowRect.getWidth(), halfHeight + m_pos.y);
    g.drawDashedLine(upLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(downLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(leftLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(rightLine, dashLengths, 2, 1.0f, 0);
}

void MapComponent::resized()
{}

void MapComponent::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
    repaint();
}

void MapComponent::addX(int val) 
{
    m_pos.x += val;
}

void MapComponent::addY(int val)
{
    m_pos.y += val;
}