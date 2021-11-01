/*
  ==============================================================================

    SCControlBase.h
    Created: 1 Nov 2021 4:53:49pm
    Author:  dspra

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SCControlBase : public juce::AudioProcessorParameter::Listener
{
public:
    SCControlBase():
    m_pParameter(nullptr)
    {}

    SCControlBase(juce::AudioProcessorParameter* parameter):
    m_pParameter(nullptr)
    {
        setParameter(parameter);
    }

    ~SCControlBase() override
    {
        removeParameter();
    }

    void setParameter(juce::AudioProcessorParameter* parameter)
    {
        if(m_pParameter != nullptr)
        {
            m_pParameter->removeListener(this);
        }

        m_pParameter = parameter;

        if(m_pParameter != nullptr)
        {
            m_pParameter->addListener(this);
        }
    }

    // convenience method
    void removeParameter()
    {
        setParameter(nullptr);
    }

    juce::AudioProcessorParameter * getParameter() { return m_pParameter; }

    virtual void parameterValueChanged(int parameterIndex, float newValue) override { updateControl();  }
    virtual void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { updateControl(); }

    virtual void controlValueChanged() = 0;
    virtual void updateControl() = 0;

protected:
    juce::AudioProcessorParameter * m_pParameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SCControlBase)
};
