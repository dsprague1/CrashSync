/*
  ==============================================================================

    SCParameterSlider.cpp
    Created: 1 Nov 2021 9:03:08am
    Author:  dspra

  ==============================================================================
*/

#include "SCParameterSlider.h"

SCParameterSlider::SCParameterSlider::SCParameterSlider():
SCControlBase(),
m_bIsDragging(false)
{
    setRange(0.0, 1.0);
    setDoubleClickReturnValue(true, 0);
    setScrollWheelEnabled(false);

    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
}

SCParameterSlider::SCParameterSlider(juce::AudioProcessorParameter * parameter):
SCControlBase(parameter),
m_bIsDragging(false)
{
    setRange(0.0, 1.0, 1.0 / (getParameter()->getNumSteps() - 1.0));
    setDoubleClickReturnValue(true, getParameter()->getDefaultValue());
    setScrollWheelEnabled(false);

    setValue(getParameter()->getValue(), juce::dontSendNotification);

    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
}

void SCParameterSlider::controlValueChanged()
{
    float newVal = static_cast<float>(getValue());

    if(getParameter()->getValue() != newVal)
    {
        if(!m_bIsDragging)
            getParameter()->beginChangeGesture();

        getParameter()->setValueNotifyingHost(newVal);

        if(!m_bIsDragging)
            getParameter()->endChangeGesture();
    }
}

void SCParameterSlider::updateControl()
{
    triggerAsyncUpdate();
}

void SCParameterSlider::valueChanged()
{
    controlValueChanged();
}

void SCParameterSlider::handleAsyncUpdate() 
{
    setValue(getParameter()->getValue(), juce::dontSendNotification);
}