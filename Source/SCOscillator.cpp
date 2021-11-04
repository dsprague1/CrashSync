#include <cmath>
#include "SCOscillator.h"
#include "SCSmoothingFilter.h"

#define USE_PW 0

SCOscillator::SCOscillator(int waveform, bool isBipolar) :
m_nPhase(0), 
m_nIncrement(0),
m_bIsBipolar(isBipolar),
m_nWaveform(waveform),
m_nSamplerate(44100),
m_bApplyPolyBlep(false),
m_nPwLeftInc(0),
m_nPwRightInc(0),
m_bIsResetState(0),
m_bPulseWidthFlag(0),
m_fPulseWidth(0.5f),
m_nPwPhase(0),
m_fFrequency(440)
{
	m_pOutputSmoother.reset(new SCSmoothingFilter());
}

SCOscillator::~SCOscillator() 
{

}

void SCOscillator::setSamplerate(int samplerate)
{ 
	m_nSamplerate = samplerate;  
	setFrequency(m_fFrequency);  
	m_pOutputSmoother->setSamplerate(samplerate);
}

void SCOscillator::setFrequency(float frequency)
{
	m_fFrequency = frequency; 
	m_nIncrement = static_cast<int32_t>(frequency / static_cast<float>(m_nSamplerate) * static_cast<float>(0x7FFFFFFF));
	
#if USE_PW
	cookPulseWidth();
#endif
}

void SCOscillator::reset(bool state)
{
	m_bIsResetState = state;
	if(state)
	{
		m_pOutputSmoother->setStartValue(cookWaveform(m_nPhase / static_cast<float>(0x7FFFFFFF)));
	}
	m_nPhase = 0;
	m_nPwPhase = 0;
}

float SCOscillator::process()
{	
	if(m_bIsResetState)
	{
		return m_pOutputSmoother->process(0.f);
	}
	else
	{
		float sample = static_cast<float>(m_nPhase) / 0x7FFFFFFF;
		sample = cookWaveform(sample);
		sample = (m_bApplyPolyBlep) ? applyPolyBlep(sample) : sample;

		m_nPhase += m_nIncrement;
		int32_t pwPrev = m_nPwPhase;

#if USE_PW
		m_nPwPhase += (m_bPulseWidthFlag) ? m_nPwLeftInc : m_nPwRightInc;
		m_bPulseWidthFlag = (pwPrev > 0 && m_nPwPhase < 0) ? !m_bPulseWidthFlag : m_bPulseWidthFlag;
		sample *= m_bPulseWidthFlag;
#endif

		if(m_bIsBipolar)
		{
			return sample;
		}
		else
		{
			return (sample + 1.f) * 0.5;
		}
	}
}

void SCOscillator::cookPulseWidth()
{
	float pwRatioLeft = 1.f / (m_fPulseWidth * 0.5);
	float pwRatioRight = m_fPulseWidth;

	m_nPwLeftInc = m_nIncrement * pwRatioLeft;//static_cast<int32_t>((m_fFrequency * pwRatioLeft) / static_cast<float>(m_nSamplerate) * static_cast<float>(0x7FFFFFFF));
	m_nPwRightInc = m_nIncrement * pwRatioRight;//(m_nIncrement * 2) - m_nPwLeftInc; // static_cast<int32_t>((m_fFrequency * pwRatioRight) / static_cast<float>(m_nSamplerate) * static_cast<float>(0x7FFFFFFF));
}

// expects -1 to 1
inline float SCOscillator::cookWaveform(float value)
{
	switch(m_nWaveform)
	{
		case kWaveformTri:
		{
			return fabs(value) * 2.f -1.f;
			break;
		}

		case kWaveformSaw:
		{
			return value;
			break;
		}

		case kWaveformSquare:
		{
			return (value > 0) ? 1.f : (value < 0) ? -1.f : 0;
			break;
		}

		default:
			return value;
			break;
	}
}

inline float SCOscillator::applyPolyBlep(float value)
{
	float correction = 0.f;

	int32_t previous = m_nPhase - m_nIncrement;
	int32_t next = m_nPhase + m_nIncrement;

	// check if before or after continuity, and calculate the residual based on that
	if(m_nPhase > 0 && next < 0) // before discontinuity?
	{
		float t = static_cast<float>(m_nPhase - 0x7FFFFFFF) / m_nIncrement;
		correction = t * t + 2.f * t + 1.f;
	}
	else if(m_nPhase < 0 && previous > 0)
	{
		float t = static_cast<float>(m_nPhase + 0x7FFFFFFF) / m_nIncrement;
		correction = 2.f * t - t * t - 1.f;
	}

	// invert for falling edge
	bool fallingEdge = (m_nWaveform == kWaveformSquare) ? (next < 0) : true;
	if(fallingEdge)
		correction *= -1.f;

	return value + correction;
}