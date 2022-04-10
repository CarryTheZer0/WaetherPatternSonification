#include "CursorComponent.h"

#include <numbers>

CursorComponent::CursorComponent(int w, int h) :
    m_pos(w / 2.0f, h / 2.0f),
    m_radius(50)
{
    setSize(w, h);
}

void CursorComponent::paint(juce::Graphics& g)
{
    float pi = 3.14159f;

    // set draw colour
    g.setColour(juce::Colours::cyan);

    // draw four dashed lines from a common point - creates a cross pattern at the centre
    const float dashLengths[2] = { 5.0f, 5.0f };
    // y
    juce::Line<float> upLine = juce::Line<float>(m_pos.x, m_pos.y - m_radius, m_pos.x, 0.0f);
    juce::Line<float> downLine = juce::Line<float>(m_pos.x, m_pos.y + m_radius, m_pos.x, getHeight());
    // x
    juce::Line<float> leftLine = juce::Line<float>(m_pos.x - m_radius, m_pos.y, 0.0f, m_pos.y);
    juce::Line<float> rightLine = juce::Line<float>(m_pos.x + m_radius, m_pos.y, getWidth(), m_pos.y);
    g.drawDashedLine(upLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(downLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(leftLine, dashLengths, 2, 1.0f, 0);
    g.drawDashedLine(rightLine, dashLengths, 2, 1.0f, 0);


    juce::Path p;
    p.startNewSubPath(m_pos.x, m_pos.y - m_radius);
    p.addArc(m_pos.x - m_radius, m_pos.y - m_radius, m_radius * 2, m_radius * 2, 0.0f, 2*pi);

    juce::PathStrokeType stroke(1.0);

    stroke.createDashedStroke(p, p, dashLengths, 1);

    g.strokePath(p, stroke);
}

void CursorComponent::resized()
{}

void CursorComponent::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
    repaint();
}

void CursorComponent::setZoom(int radius)
{
    m_radius = radius;
    repaint();
}
