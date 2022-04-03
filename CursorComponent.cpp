#include "CursorComponent.h"

CursorComponent::CursorComponent(int w, int h) :
    m_pos(0, 0)
{
    setSize(w, h);
}

void CursorComponent::paint(juce::Graphics& g)
{
    juce::Rectangle<int> windowRect = getLocalBounds();

    // set draw colour
    g.setColour(juce::Colours::cyan);

    // draw four dashed lines from a common point - creates a cross pattern at the centre
    const float dashLengths[2] = { 5.0f, 5.0f };
    // y
    juce::Line<float> upLine = juce::Line<float>(m_pos.x, m_pos.y, m_pos.x, 0.0f);
    juce::Line<float> downLine = juce::Line<float>(m_pos.x, m_pos.y, m_pos.x, windowRect.getHeight());
    // x
    juce::Line<float> leftLine = juce::Line<float>(m_pos.x, m_pos.y, 0.0f, m_pos.y);
    juce::Line<float> rightLine = juce::Line<float>(m_pos.x, m_pos.y, windowRect.getWidth(), m_pos.y);
    g.drawDashedLine(upLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(downLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(leftLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(rightLine, dashLengths, 2, 1.0f, 0);
}

void CursorComponent::resized()
{}

void CursorComponent::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
    repaint();
}
