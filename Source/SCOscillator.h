#pragma once
#include <cstdint>

class SCOscillator
{
public:
	enum Waveform
	{
		WaveformTri,
		WaveformSaw,
		WaveformSquare,
		numWaveforms
	};

	SCOscillator(int waveform = WaveformSaw, bool isBipolar = true);
	~SCOscillator();

	void setSamplerate(int samplerate) { m_nSamplerate = samplerate;  setFrequency(m_fFrequency);  }
	void setFrequency(float frequency);
	void setIsBipolar(bool isBipolar) { m_bIsBipolar = isBipolar; }
	void setWaveform(int wave) { m_nWaveform = wave; }

	int32_t getPhase() { return m_nPhase; }

	void reset(bool state);
	void setResetState(bool state) { m_bIsResetState = state; }
	float process();

private:
	inline int32_t IncrementAndCookWaveform();
	inline int32_t CookWaveform(int32_t value);

	int32_t m_nPhase;
	int32_t m_nIncrement;
	int32_t m_nSamplerate;
	uint32_t m_nWaveform;
	float m_fFrequency;
	float m_fSmoothedOutZ;
	float m_fSmoothedOut;
	bool m_bIsBipolar;
	bool m_bIsResetState;
};

