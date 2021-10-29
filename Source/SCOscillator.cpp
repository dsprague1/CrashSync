#include <cmath>
#include "SCOscillator.h"

SCOscillator::SCOscillator(int waveform, bool isBipolar) :
m_nPhase(0), 
m_nIncrement(0),
m_bIsBipolar(isBipolar),
m_nWaveform(waveform)
{

}

SCOscillator::~SCOscillator() 
{

}

void SCOscillator::setFrequency(float frequency)
{
	m_nIncrement = static_cast<int32_t>(static_cast<float>(0x7FFFFFFF) / frequency);
}

void SCOscillator::reset()
{
	m_nPhase = 0;
}

float SCOscillator::process()
{	
	uint32_t currentValue = IncrementAndCookWaveform();
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

inline uint32_t SCOscillator::IncrementAndCookWaveform()
{
	m_nPhase += m_nIncrement;

	switch(m_nWaveform)
	{
		case WaveformTri:
		{
			return static_cast<uint32_t>((abs(m_nPhase) - 0x3FFFFFFF) * 2);
			break;
		}

		case WaveformSaw:
		{
			return m_nPhase;
			break;
		}

		case WaveformSquare:
		{
			return (static_cast<int32_t>(m_nPhase) > 0) ? 0xFFFFFFFF : 0x7FFFFFFF;
			break;
		}

		default:
			return m_nIncrement;
			break;
	}
}