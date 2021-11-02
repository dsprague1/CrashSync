/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SCParameterSlider;

class CrashSyncAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CrashSyncAudioProcessorEditor (CrashSyncAudioProcessor&);
    ~CrashSyncAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CrashSyncAudioProcessor& m_audioProcessor;

    std::unique_ptr<SCParameterSlider> m_pFrequencySlider;
    std::unique_ptr<SCParameterSlider> m_pThresholdSlider;
    std::unique_ptr<SCParameterSlider> m_pGainSlider;
    std::unique_ptr<SCParameterSlider> m_pWaveformSlider;
    std::unique_ptr<SCParameterSlider> m_pInputMode;
    std::unique_ptr<SCParameterSlider> m_pAttackSlider;
    std::unique_ptr<SCParameterSlider> m_pReleaseSlider;
    std::unique_ptr<SCParameterSlider> m_pPolyBlepSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrashSyncAudioProcessorEditor)
};
