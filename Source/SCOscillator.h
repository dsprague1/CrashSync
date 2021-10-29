#pragma once
#include <cstdint>

class SCOscillator
{
public:
	SCOscillator(int waveform = WaveformSaw, bool isBipolar = true);
	~SCOscillator();

	void setFrequency(float frequency);
	void setIsBipolar(bool isBipolar) { m_bIsBipolar = isBipolar; }
	void setWaveform(int wave) { m_nWaveform = wave; }

	uint32_t getPhase() { return m_nPhase; }

	void reset();
	float process();

private:
	enum Waveform
	{
		WaveformTri,
		WaveformSaw,
		WaveformSquare,
		numWaveforms
	};

	inline uint32_t IncrementAndCookWaveform();

	int32_t m_nPhase;
	int32_t m_nIncrement;
	uint32_t m_nWaveform;
	bool m_bIsBipolar;
};

