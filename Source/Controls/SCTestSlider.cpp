/*
  ==============================================================================

    SCTestSlider.cpp
    Created: 8 Nov 2021 8:32:41pm
    Author:  dspra

  ==============================================================================
*/

#include "SCTestSlider.h"

SCTestSlider::SCTestSlider()
{
    m_pSlider.reset(new SCParameterSlider());
    addAndMakeVisible(m_pSlider.get());

    setupSlider();
}

SCTestSlider::SCTestSlider(juce::AudioProcessorParameter * parameter)
{
    parameter->addListener(this);

    m_pSlider.reset(new SCParameterSlider(parameter));
    addAndMakeVisible(m_pSlider.get());

    setupSlider();

    triggerAsyncUpdate();
}

SCTestSlider::~SCTestSlider()
{
    if(m_pSlider->getParameter() != nullptr)
    {
        m_pSlider->getParameter()->removeListener(this);
    }
}

void SCTestSlider::setupSlider()
{
    m_pValueLabel.reset(new juce::Label());
    m_pValueLabel->setColour(juce::Label::outlineColourId, m_pSlider->findColour(juce::Slider::textBoxOutlineColourId));
    m_pValueLabel->setBorderSize({ 1, 1, 1, 1 });
    m_pValueLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_pValueLabel.get());

    m_pNameLabel.reset(new juce::Label());  
    m_pNameLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_pNameLabel.get());

    if(m_pSlider->getParameter() != nullptr)
    {
        m_pNameLabel->setText(m_pSlider->getParameter()->getName(16), juce::dontSendNotification);
    }
}

void SCTestSlider::resized()
{
    auto area = getLocalBounds();
    int width = area.getWidth();

    m_pValueLabel->setBounds(area.removeFromRight(width / 6));
    area.removeFromRight(width / 16);
    m_pNameLabel->setBounds(area.removeFromLeft(width / 4));
    area.removeFromLeft(width / 16);
    m_pSlider->setBounds(area);
}

void SCTestSlider::parameterValueChanged(int parameterIndex, float newValue)
{
    triggerAsyncUpdate();
}

void SCTestSlider::handleAsyncUpdate()
{
    m_pValueLabel->setText(m_pSlider->getParameter()->getCurrentValueAsText(), juce::dontSendNotification);
}