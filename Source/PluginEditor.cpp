/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Controls/SCParameterSlider.h"
#include "Parameter/CrashSyncParameters.h"

//==============================================================================
CrashSyncAudioProcessorEditor::CrashSyncAudioProcessorEditor (CrashSyncAudioProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p)
{
    juce::Array<juce::AudioProcessorParameter*> params = m_audioProcessor.getParameters();
    m_pFrequencySlider.reset(new SCParameterSlider(params[kCrashSyncParamFrequency]));
    m_pThresholdSlider.reset(new SCParameterSlider(params[kCrashSyncParamThreshold]));
    m_pGainSlider.reset(new SCParameterSlider(params[kCrashSyncParamGain]));
    m_pWaveformSlider.reset(new SCParameterSlider(params[kCrashSyncParamWaveform]));
    m_pInputMode.reset(new SCParameterSlider(params[kCrashSyncParamInputMode]));
    m_pAttackSlider.reset(new SCParameterSlider(params[kCrashSyncParamAttack]));
    m_pReleaseSlider.reset(new SCParameterSlider(params[kCrashSyncParamRelease]));
    m_pPolyBlepSlider.reset(new SCParameterSlider(params[kCrashSyncParamPolyBlep]));

    addAndMakeVisible(m_pFrequencySlider.get());
    addAndMakeVisible(m_pThresholdSlider.get());
    addAndMakeVisible(m_pGainSlider.get());
    addAndMakeVisible(m_pWaveformSlider.get());
    addAndMakeVisible(m_pInputMode.get());
    addAndMakeVisible(m_pAttackSlider.get());
    addAndMakeVisible(m_pReleaseSlider.get());
    addAndMakeVisible(m_pPolyBlepSlider.get());

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

    m_pFrequencySlider->setBounds(10, 10, 200, 15);
    m_pThresholdSlider->setBounds(10, 30, 200, 15);
    m_pGainSlider->setBounds(10, 50, 200, 15);
    m_pWaveformSlider->setBounds(10, 70, 200, 15);
    m_pInputMode->setBounds(10, 90, 200, 15);
    m_pAttackSlider->setBounds(10, 110, 200, 15);
    m_pReleaseSlider->setBounds(10, 130, 200, 15);
    m_pPolyBlepSlider->setBounds(10, 150, 200, 15);
}