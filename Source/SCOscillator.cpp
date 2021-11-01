#include <cmath>
#include "SCOscillator.h"

SCOscillator::SCOscillator(int waveform, bool isBipolar) :
m_nPhase(0), 
m_nIncrement(0),
m_bIsBipolar(isBipolar),
m_nWaveform(waveform),
m_nSamplerate(44100)
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
		m_fSmoothedOutZ = CookWaveform(m_nPhase) / static_cast<float>(0x7FFFFFFF);
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
		int32_t currentValue = IncrementAndCookWaveform();
		float bipolarValue = static_cast<float>(currentValue) / 0x7FFFFFFF;
		if(m_bIsBipolar)
		{
			return bipolarValue;
		}
		else
		{
			return (bipolarValue + 1) * 0.5;
		}
	}
}

inline int32_t SCOscillator::IncrementAndCookWaveform()
{
	int32_t value = m_nPhase;
	m_nPhase += m_nIncrement;

	return CookWaveform(value);
}

inline int32_t SCOscillator::CookWaveform(int32_t value)
{
	switch(m_nWaveform)
	{
	case WaveformTri:
	{
		return static_cast<uint32_t>((abs(value) - 0x3FFFFFFF) * 2);
		break;
	}

	case WaveformSaw:
	{
		return value;
		break;
	}

	case WaveformSquare:
	{
		return (value > 0) ? 0xFFFFFFFF : (value < 0) ? 0x7FFFFFFF : 0;
		break;
	}

	default:
		return value;
		break;
	}
}