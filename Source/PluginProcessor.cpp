/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SCBiquadFilterUtil.h"

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
    m_pFrequency = new juce::AudioParameterFloat("osc_frequency", "Osc Freq", 0.f, 1.f, 0.1f);
    m_pThreshold = new juce::AudioParameterFloat("threshold", "Threshold", 0.0f, 1.0f, 0.5f);
    m_pGain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.3f);
    m_pWaveform = new juce::AudioParameterInt("waveform", "Waveform", SCOscillator::kWaveformTri, SCOscillator::numWaveforms - 1, SCOscillator::kWaveformTri);
    m_pInputMode = new juce::AudioParameterInt("input_mode", "Input Mode", kInputModeNormal, kNumInputModes - 1, kInputModeNormal);
    m_pEnvAttack = new juce::AudioParameterFloat("env_attack", "Env Attack", 0.f, 1.f, 0.1);
    m_pEnvRelease = new juce::AudioParameterFloat("env_release", "Env Release", 0.f, 1.f, 0.1);
    m_pPolyBlep = new juce::AudioParameterInt("polyblep", "PolyBLEP", 0.f, 1.f, 0);
    m_pPulseWidth = new juce::AudioParameterFloat("pulse_width", "Pulse Width", 0.f, 1.f, 0.5);
    m_pOutputVolume = new juce::AudioParameterFloat("output_volume", "Output Lvl", 0.f, 1.f, 0.8);
    m_pTone = new juce::AudioParameterFloat("tone", "Tone", 0.f, 1.f, 0.8);
	m_pOversample = new juce::AudioParameterInt("oversample", "Oversample", 0.f, 1.f, 0.0);
	m_pInputFilterCutoff = new juce::AudioParameterFloat("input_filter", "Input Filter", 0.f, 1.f, 0.1f);
	m_pMix = new juce::AudioParameterFloat("mix", "Mix", 0.f, 1.f, 1.f);

    addParameter(m_pFrequency);
    addParameter(m_pThreshold);
    addParameter(m_pGain);
    addParameter(m_pWaveform);
    addParameter(m_pInputMode);
    addParameter(m_pEnvAttack);
    addParameter(m_pEnvRelease);
    addParameter(m_pPolyBlep);
    addParameter(m_pPulseWidth);
    addParameter(m_pOutputVolume);
    addParameter(m_pTone);
	addParameter(m_pOversample);
	addParameter(m_pInputFilterCutoff);
	addParameter(m_pMix);

	m_pOversampler.reset(new juce::dsp::Oversampling<float>(getNumInputChannels(), 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, false));
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
    m_EnvelopeFollowerL.setSamplerate(sampleRate);
	m_EnvelopeFollowerR.setSamplerate(sampleRate);
    m_OscillatorL.setSamplerate(sampleRate);
    m_OscillatorL.reset(false);
	m_OscillatorR.setSamplerate(sampleRate);
	m_OscillatorR.reset(false);
	m_FilterL.setSamplerate(sampleRate);
	m_FilterL.reset();
	m_FilterR.setSamplerate(sampleRate);
	m_FilterR.reset();
	m_InputFilterL.reset();
	m_InputFilterR.reset();

	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getNumInputChannels();

	m_pOversampler->reset();
	m_pOversampler->initProcessing(static_cast<size_t>(samplesPerBlock));
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

	processSubrate();

	float * inputL = buffer.getWritePointer(0);
	float * inputR = buffer.getWritePointer(1);
	float * outputL = buffer.getWritePointer(0);
	float * outputR = buffer.getWritePointer(1);
	int numSamples = buffer.getNumSamples();
	juce::dsp::AudioBlock<float> targetBlock;
	juce::dsp::AudioBlock<float> oversampleBuffer;

	if(m_pOversample->get())
	{
		targetBlock = juce::dsp::AudioBlock<float>(buffer);
		oversampleBuffer = m_pOversampler->processSamplesUp(targetBlock);

		inputL = oversampleBuffer.getChannelPointer(0);
		inputR = oversampleBuffer.getChannelPointer(1);
		outputL = oversampleBuffer.getChannelPointer(0);
		outputR = oversampleBuffer.getChannelPointer(1);
		numSamples = oversampleBuffer.getNumSamples();
	}

    for(int i = 0; i < numSamples; i++)
    {
		float dryL = *(inputL + i);
		float dryR = *(inputR + i);

		float sigL = m_InputFilterL.process(dryL);
		float sigR = m_InputFilterR.process(dryR);

		// fuzz section
		float gain = 0.5f + 39.5f * m_pGain->get();
		sigL *= gain;
		sigL = (sigL > 1.f) ? 1.f : sigL;
		sigL = (sigL < 0) ? -1.f : sigL;

		sigR *= gain;
		sigR = (sigR > 1.f) ? 1.f : sigR;
		sigR = (sigR < 0) ? -1.f : sigR;

		if(m_pInputMode->get() == kInputModeEnvelope)
		{
			sigL = m_EnvelopeFollowerL.process(sigL);
			sigR = m_EnvelopeFollowerR.process(sigR);
		}

		// check for reset
		if(sigL <= m_pThreshold->get())
		{
			m_OscillatorL.reset(true);
		}
		else
		{
			m_OscillatorL.setResetState(false);
		}

		if(sigR <= m_pThreshold->get())
		{
			m_OscillatorR.reset(true);
		}
		else
		{
			m_OscillatorR.setResetState(false);
		}

		sigL = m_OscillatorL.process();
		sigR = m_OscillatorR.process();
		
		sigL = m_FilterL.process(sigL);
		sigR = m_FilterR.process(sigR);

		// just do linear mix for now
		sigL = sigL * m_pMix->get() + dryL * (1.f - m_pMix->get());
		sigR = sigR * m_pMix->get() + dryR * (1.f - m_pMix->get());

		*(outputL + i) = sigR * m_pOutputVolume->get();
		*(outputR + i) = sigL * m_pOutputVolume->get();
    }

	if(m_pOversample->get())
	{
		m_pOversampler->processSamplesDown(targetBlock);
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

void CrashSyncAudioProcessor::processSubrate()
{
	// subrate
	const int samplerate = m_pOversample->get() ? getSampleRate() * m_pOversampler->getOversamplingFactor() : getSampleRate();

	m_OscillatorL.setSamplerate(samplerate);
	m_OscillatorR.setSamplerate(samplerate);
	m_EnvelopeFollowerL.setSamplerate(samplerate);
	m_EnvelopeFollowerR.setSamplerate(samplerate);
	m_FilterL.setSamplerate(samplerate);
	m_FilterR.setSamplerate(samplerate);
	
	m_OscillatorL.setFrequency((m_pFrequency->get() * m_pFrequency->get()) * 19900 + 100);
	m_OscillatorL.setWaveform(m_pWaveform->get());
	m_OscillatorL.setApplyPolyBlep(m_pPolyBlep->get());
	m_OscillatorL.setPulseWidth(m_pPulseWidth->get());
	m_OscillatorR.setFrequency((m_pFrequency->get() * m_pFrequency->get()) * 19900 + 100);
	m_OscillatorR.setWaveform(m_pWaveform->get());
	m_OscillatorR.setApplyPolyBlep(m_pPolyBlep->get());
	m_OscillatorR.setPulseWidth(m_pPulseWidth->get());
	m_EnvelopeFollowerL.setAttackTimeMs(m_pEnvAttack->get());
	m_EnvelopeFollowerL.setReleaseTimeMs(m_pEnvRelease->get());
	m_EnvelopeFollowerR.setAttackTimeMs(m_pEnvAttack->get());
	m_EnvelopeFollowerR.setReleaseTimeMs(m_pEnvRelease->get());

	float cutoff = m_pTone->get() * m_pTone->get();
	cutoff = 20.f + (19980.f * cutoff);
	m_FilterL.setCutoff(cutoff);
	m_FilterR.setCutoff(cutoff);

	float inputCutoff = m_pInputFilterCutoff->get() * m_pInputFilterCutoff->get();
	inputCutoff = 20.f + (19980.f * inputCutoff);

	// fixed reso variable fc
	m_InputFilterL.setCoeffsForBand(GenerateSecondOrderLpfCoeffs(inputCutoff, 3.f, samplerate), 0); 
	m_InputFilterR.setCoeffsForBand(GenerateSecondOrderLpfCoeffs(inputCutoff, 3.f, samplerate), 0);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrashSyncAudioProcessor();
}