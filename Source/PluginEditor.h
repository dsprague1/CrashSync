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
namespace SCUI
{
    class SCTestSlider;
}

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

    std::unique_ptr<SCUI::SCTestSlider> m_pFrequencySlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pThresholdSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pGainSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pWaveformSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pInputMode;
    std::unique_ptr<SCUI::SCTestSlider> m_pAttackSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pReleaseSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pPolyBlepSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pPulseWidthSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pOutputVolumeSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pToneSlider;
    std::unique_ptr<SCUI::SCTestSlider> m_pOversample;
    std::unique_ptr<SCUI::SCTestSlider> m_pInputCutoff;
    std::unique_ptr<SCUI::SCTestSlider> m_pMix;
    std::unique_ptr<SCUI::SCTestSlider> m_pLfoRate;
    std::unique_ptr<SCUI::SCTestSlider> m_pLfoDepth;
    std::unique_ptr<SCUI::SCTestSlider> m_pOscPhase;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrashSyncAudioProcessorEditor)
};
