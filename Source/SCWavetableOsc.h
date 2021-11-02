/*
  ==============================================================================

    SCWavetableOsc.h
    Created: 2 Nov 2021 4:00:51pm
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include <cstdint>

class SCWavetableOsc
{
public:
    enum
    {
        kWaveformTriangle,
        kWaveformSaw,
        kWaveformSquare,
        kNumWaveforms
    };


    SCWavetableOsc();
    ~SCWavetableOsc();

    void setSamplerate(int32_t samplerate) { m_nSamplerate = samplerate;  generateTables(); }
    void generateTables();
    void reset();

    float process();

protected:
    int32_t m_nSamplerate;
    const int32_t m_nTableLength;
    float m_fPhase;
    float m_fPhaseIncrement;

    int32_t m_nWave;
    int32_t m_nNote;

    float * m_pWavetables[kNumWaveforms][128];
};