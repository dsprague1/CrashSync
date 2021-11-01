/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CrashSyncAudioProcessor::CrashSyncAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    m_pFrequency = new juce::AudioParameterFloat("frequency", "Frequency", 0.0f, 1.0f, 0.0f);
    m_pThreshold = new juce::AudioParameterFloat("threshold", "Threshold", 0.0f, 1.0f, 0.0f);
    m_pGain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.0f);
    m_pWaveform = new juce::AudioParameterInt("waveform", "Waveform", SCOscillator::WaveformTri, SCOscillator::numWaveforms - 1, SCOscillator::WaveformSaw);
    m_pInputMode = new juce::AudioParameterInt("input_mode", "Input Mode", kInputModeNormal, kNumInputMode - 1, kInputModeNormal);
    m_pEnvAttack = new juce::AudioParameterFloat("env_attack", "Env Attack", 0.f, 1.f, 0.1);
    m_pEnvRelease = new juce::AudioParameterFloat("env_release", "Env Release", 0.f, 1.f, 0.1);

    addParameter(m_pFrequency);
    addParameter(m_pThreshold);
    addParameter(m_pGain);
    addParameter(m_pWaveform);
    addParameter(m_pInputMode);
    addParameter(m_pEnvAttack);
    addParameter(m_pEnvRelease);
}

CrashSyncAudioProcessor::~CrashSyncAudioProcessor()
{
}

//==============================================================================
const juce::String CrashSyncAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CrashSyncAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CrashSyncAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CrashSyncAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CrashSyncAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CrashSyncAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CrashSyncAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CrashSyncAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CrashSyncAudioProcessor::getProgramName (int index)
{
    return {};
}

void CrashSyncAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CrashSyncAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_EnvelopeFollower.setAttackTimeMs(sampleRate);
    m_Oscillator.setSamplerate(sampleRate);
    m_Oscillator.reset(false);
}

void CrashSyncAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CrashSyncAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CrashSyncAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // subrate
    m_Oscillator.setFrequency((m_pFrequency->get() * m_pFrequency->get()) * 19000 + 1000);
    m_Oscillator.setWaveform(static_cast<int>(m_pWaveform->get() * (SCOscillator::numWaveforms - 1) + 0.5f));
    m_EnvelopeFollower.setAttackTimeMs(m_pEnvAttack->get());
    m_EnvelopeFollower.setReleaseTimeMs(m_pEnvRelease->get());

    const float* inputL = buffer.getReadPointer(0);
    const float* inputR = buffer.getReadPointer(1);
    float* outputL = buffer.getWritePointer(0);
    float* outputR = buffer.getWritePointer(1);

    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
        float sig = (*(inputL + i) + *(inputR + i)) * 0.5f;
        
        // fuzz section
        float gain = 0.5f + 39.5f * m_pGain->get();
        sig *= gain;
        sig = (sig > 1.f) ? 1.f : sig;
        sig = (sig < 0) ? -1.f : sig;        

        if(m_pInputMode->get() == kInputModeEnvelope)
        {
            sig = m_EnvelopeFollower.process(sig);
        }

        // check for reset
        if(sig <= m_pThreshold->get()) // parameterize this threshold
        {
            m_Oscillator.reset(true);
        }
        else
        {
            m_Oscillator.setResetState(false);
        }

        float value = m_Oscillator.process();

        *outputL++ = value;
        *outputR++ = value;
    }
}

//==============================================================================
bool CrashSyncAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CrashSyncAudioProcessor::createEditor()
{
    return new CrashSyncAudioProcessorEditor (*this);
}

//==============================================================================
void CrashSyncAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CrashSyncAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrashSyncAudioProcessor();
}
