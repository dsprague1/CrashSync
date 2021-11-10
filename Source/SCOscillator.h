#pragma once
#include <cstdint>
#include <memory>

class SCSmoothingFilter;

class SCOscillator
{
public:
	enum Waveform
	{
		kWaveformTri,
		kWaveformSaw,
		kWaveformRoundedSaw,
		kWaveformSquare,
		kWaveformWigglySquare,
		kWaveformFallingSquare,
		kWaveformNoise,
		kWaveformFilteredNoise,
		numWaveforms
	};

	SCOscillator(int waveform = kWaveformSaw, bool isBipolar = true);
	~SCOscillator();

	void setSamplerate(int samplerate);
	void setFrequency(float frequency);
	void setIsBipolar(bool isBipolar) { m_bIsBipolar = isBipolar; }
	void setWaveform(int wave);
	void setApplyPolyBlep(bool applyPolyBlep) { m_bApplyPolyBlep = applyPolyBlep; }
	void setPulseWidth(float pulseWidth) { m_fPulseWidth = pulseWidth; cookPulseWidth(); }

	int32_t getPhase() { return m_nPhase; }

	void reset(bool state);
	void setResetState(bool state) { m_bIsResetState = state; }
	float process();

private:
	void cookPulseWidth();
	inline float generateSaw(float value);
	inline float generateSquare(float value);
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
	float m_fPulseWidth;
	bool m_bIsBipolar;
	bool m_bIsResetState;
	bool m_bApplyPolyBlep;
	bool m_bPulseWidthFlag;

	float m_fAverage;
	float m_fNumAverageSamples;

	float m_fNoiseAverage;
	
	float m_fFallingSquareDiff;
	float m_fFallingSquareState;

	std::unique_ptr<SCSmoothingFilter> m_pOutputSmoother;
};

