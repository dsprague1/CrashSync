#include <cmath>
#include "SCOscillator.h"

SCOscillator::SCOscillator(int waveform, bool isBipolar) :
m_nPhase(0), 
m_nIncrement(0),
m_bIsBipolar(isBipolar),
m_nWaveform(waveform),
m_nSamplerate(44100),
m_bApplyPolyBlep(false)
{

}

SCOscillator::~SCOscillator() 
{

}

void SCOscillator::setFrequency(float frequency)
{
	m_fFrequency = frequency; 
	m_nIncrement = static_cast<int32_t>(frequency / static_cast<float>(m_nSamplerate) * static_cast<float>(0x7FFFFFFF));
}

void SCOscillator::reset(bool state)
{
	m_bIsResetState = state;
	if(state)
	{
		m_fSmoothedOutZ = cookWaveform(m_nPhase / static_cast<float>(0x7FFFFFFF));
	}
	m_nPhase = 0;
}

float SCOscillator::process()
{	
	if(m_bIsResetState)
	{
		m_fSmoothedOutZ *= 0.001;
		return m_fSmoothedOutZ;
	}
	else
	{
		float sample = static_cast<float>(m_nPhase) / 0x7FFFFFFF;
		sample = cookWaveform(sample);
		sample = (m_bApplyPolyBlep) ? applyPolyBlep(sample) : sample;

		m_nPhase += m_nIncrement;

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