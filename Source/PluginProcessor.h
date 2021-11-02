/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SCOscillator.h"
#include "SCEnvelopeFollower.h"

//==============================================================================
/**
*/
class CrashSyncAudioProcessor  : public juce::AudioProcessor
{
public:
    enum
    {
        kInputModeNormal,
        kInputModeEnvelope,
        kNumInputMode
    };

    //==============================================================================
    CrashSyncAudioProcessor();
    ~CrashSyncAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    // parameters
    juce::AudioParameterFloat * m_pFrequency;
    juce::AudioParameterFloat * m_pThreshold;
    juce::AudioParameterFloat * m_pGain;
    juce::AudioParameterFloat * m_pEnvAttack;
    juce::AudioParameterFloat * m_pEnvRelease;
    juce::AudioParameterInt * m_pWaveform;
    juce::AudioParameterInt * m_pInputMode;
    juce::AudioParameterInt * m_pPolyBlep;

    SCOscillator m_Oscillator;
    SCEnvelopeFollower m_EnvelopeFollower;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrashSyncAudioProcessor)
};
