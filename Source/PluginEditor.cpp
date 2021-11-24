/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SCTestSlider.h"
#include "Parameter/CrashSyncParameters.h"

//==============================================================================
CrashSyncAudioProcessorEditor::CrashSyncAudioProcessorEditor (CrashSyncAudioProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p)
{
    juce::Array<juce::AudioProcessorParameter*> params = m_audioProcessor.getParameters();
    m_pFrequencySlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamFrequency]));
    m_pThresholdSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamThreshold]));
    m_pGainSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamGain]));
    m_pWaveformSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamWaveform]));
    m_pInputMode.reset(new SCUI::SCTestSlider(params[kCrashSyncParamInputMode]));
    m_pAttackSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamAttack]));
    m_pReleaseSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamRelease]));
    m_pPolyBlepSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamPolyBlep]));
    m_pPulseWidthSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamPulseWidth]));
    m_pOutputVolumeSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamOutputVolume]));
    m_pToneSlider.reset(new SCUI::SCTestSlider(params[kCrashSyncParamTone]));
    m_pOversample.reset(new SCUI::SCTestSlider(params[kCrashSyncParamOversample]));
    m_pInputCutoff.reset(new SCUI::SCTestSlider(params[kCrashSyncParamInputFilter]));
    m_pMix.reset(new SCUI::SCTestSlider(params[kCrashSyncParamMix]));
    m_pLfoRate.reset(new SCUI::SCTestSlider(params[kCrashSyncParamLfoRate]));
    m_pLfoDepth.reset(new SCUI::SCTestSlider(params[kCrashSyncParamLfoDepth]));
    m_pOscPhase.reset(new SCUI::SCTestSlider(params[kCrashSyncParamOscPhase]));

    addAndMakeVisible(m_pFrequencySlider.get());
    addAndMakeVisible(m_pThresholdSlider.get());
    addAndMakeVisible(m_pGainSlider.get());
    addAndMakeVisible(m_pWaveformSlider.get());
    addAndMakeVisible(m_pInputMode.get());
    addAndMakeVisible(m_pAttackSlider.get());
    addAndMakeVisible(m_pReleaseSlider.get());
    addAndMakeVisible(m_pPolyBlepSlider.get());
    addAndMakeVisible(m_pPulseWidthSlider.get());
    addAndMakeVisible(m_pOutputVolumeSlider.get());
    addAndMakeVisible(m_pToneSlider.get());
    addAndMakeVisible(m_pOversample.get());
    addAndMakeVisible(m_pInputCutoff.get());
    addAndMakeVisible(m_pMix.get());
    addAndMakeVisible(m_pLfoRate.get());
    addAndMakeVisible(m_pLfoDepth.get());
    addAndMakeVisible(m_pOscPhase.get());

    setSize (400, 300);
}

CrashSyncAudioProcessorEditor::~CrashSyncAudioProcessorEditor()
{
}

//==============================================================================
void CrashSyncAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CrashSyncAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();

    m_pFrequencySlider->setBounds(10, 10, 300, 15);
    m_pThresholdSlider->setBounds(10, 30, 300, 15);
    m_pGainSlider->setBounds(10, 50, 300, 15);
    m_pWaveformSlider->setBounds(10, 70, 300, 15);
    m_pInputMode->setBounds(10, 90, 300, 15);
    m_pAttackSlider->setBounds(10, 110, 300, 15);
    m_pReleaseSlider->setBounds(10, 130, 300, 15);
    m_pOutputVolumeSlider->setBounds(10, 150, 300, 15);
    m_pToneSlider->setBounds(10, 170, 300, 15);
    m_pOversample->setBounds(10, 190, 300, 15);
    m_pInputCutoff->setBounds(10, 210, 300, 15);
    m_pMix->setBounds(10, 230, 300, 15);
    m_pLfoRate->setBounds(10, 250, 300, 15);
    m_pLfoDepth->setBounds(10, 270, 300, 15);
    m_pOscPhase->setBounds(10, 290, 300, 15);
}