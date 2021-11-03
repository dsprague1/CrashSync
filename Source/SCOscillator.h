#pragma once
#include <cstdint>

class SCOscillator
{
public:
	enum Waveform
	{
		kWaveformTri,
		kWaveformSaw,
		kWaveformSquare,
		numWaveforms
	};

	SCOscillator(int waveform = kWaveformSaw, bool isBipolar = true);
	~SCOscillator();

	void setSamplerate(int samplerate) { m_nSamplerate = samplerate;  setFrequency(m_fFrequency);  }
	void setFrequency(float frequency);
	void setIsBipolar(bool isBipolar) { m_bIsBipolar = isBipolar; }
	void setWaveform(int wave) { m_nWaveform = wave; }
	void setApplyPolyBlep(bool applyPolyBlep) { m_bApplyPolyBlep = applyPolyBlep; }
	void setPulseWidth(float pulseWidth) { m_fPulseWidth = pulseWidth; cookPulseWidth(); }

	int32_t getPhase() { return m_nPhase; }

	void reset(bool state);
	void setResetState(bool state) { m_bIsResetState = state; }
	float process();

private:
	void cookPulseWidth();
	inline float cookWaveform(float value);
	inline float applyPolyBlep(float value);

	int32_t m_nPhase;
	int32_t m_nPwPhase;
	int32_t m_nIncrement;
	int32_t m_nPwLeftInc;
	int32_t m_nPwRightInc;
	int32_t m_nSamplerate;
	uint32_t m_nWaveform;
	float m_fFrequency;
	float m_fSmoothedOutZ;
	float m_fSmoothedOut;
	float m_fPulseWidth;
	bool m_bIsBipolar;
	bool m_bIsResetState;
	bool m_bApplyPolyBlep;
	bool m_bPulseWidthFlag;
};

