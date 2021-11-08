/*
  ==============================================================================

    SCTestSlider.h
    Created: 8 Nov 2021 8:32:41pm
    Author:  dspra

  ==============================================================================
*/

#pragma once

#include "SCParameterSlider.h"

class SCTestSlider : public juce::Component, private juce::AudioProcessorParameter::Listener, public juce::AsyncUpdater
{
public:
    SCTestSlider();
    SCTestSlider(juce::AudioProcessorParameter * parameter);
    ~SCTestSlider();

    void setupSlider();
    void resized() override;

    // listener
    virtual void parameterValueChanged(int parameterIndex, float newValue) override;
    virtual void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {}

    // async
    virtual void handleAsyncUpdate() override;

private:

    std::unique_ptr<SCParameterSlider> m_pSlider;
    std::unique_ptr<juce::Label> m_pValueLabel;
    std::unique_ptr<juce::Label> m_pNameLabel;
};