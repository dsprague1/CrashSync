/*
  ==============================================================================

    SCParameterSlider.h
    Created: 1 Nov 2021 9:03:08am
    Author:  dspra

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "SCControlBase.h"

class SCParameterSlider : public juce::Slider, public SCControlBase
{
public:
    SCParameterSlider();
    SCParameterSlider(juce::AudioProcessorParameter * parameter);

    // base class overrides
    virtual void controlValueChanged() override;
    virtual void updateControl() override;

    // slider class overrides
    virtual void valueChanged() override;

private:
    bool m_bIsDragging;
};