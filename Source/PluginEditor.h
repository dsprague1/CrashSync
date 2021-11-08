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
class SCTestSlider;

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

    std::unique_ptr<SCTestSlider> m_pFrequencySlider;
    std::unique_ptr<SCTestSlider> m_pThresholdSlider;
    std::unique_ptr<SCTestSlider> m_pGainSlider;
    std::unique_ptr<SCTestSlider> m_pWaveformSlider;
    std::unique_ptr<SCTestSlider> m_pInputMode;
    std::unique_ptr<SCTestSlider> m_pAttackSlider;
    std::unique_ptr<SCTestSlider> m_pReleaseSlider;
    std::unique_ptr<SCTestSlider> m_pPolyBlepSlider;
    std::unique_ptr<SCTestSlider> m_pPulseWidthSlider;
    std::unique_ptr<SCTestSlider> m_pOutputVolumeSlider;
    std::unique_ptr<SCTestSlider> m_pToneSlider;
    std::unique_ptr<SCTestSlider> m_pOversample;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrashSyncAudioProcessorEditor)
};
