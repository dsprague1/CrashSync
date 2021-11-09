/*
  ==============================================================================

    SCWavetableOsc.cpp
    Created: 2 Nov 2021 4:00:51pm
    Author:  dspra

  ==============================================================================
*/

#include "SCWavetableOsc.h"
#include <cstring>
#include "SCMath.h"

SCWavetableOsc::SCWavetableOsc():
m_nTableLength(1024),
m_fPhase(0),
m_fPhaseIncrement(0),
m_nNote(0),
m_nWave(0)
{
    for(int i = 0; i < kNumWaveforms; i++)
    {
        for(int j = 0; j < 128; j++)
        {
            m_pWavetables[i][j] = new float[m_nTableLength];
        }
    }
}

SCWavetableOsc::~SCWavetableOsc()
{
    for(int i = 0; i < kNumWaveforms; i++)
    {
        for(int j = 0; j < 128; j++)
        {
            delete [] m_pWavetables[i];
            m_pWavetables[i][j] = nullptr;
        }
    }
}

void SCWavetableOsc::generateTables()
{
    // wipe
    for(int i = 0; i < kNumWaveforms; i++)
    {
        std::memset(m_pWavetables[i], 0, m_nTableLength);
    }

    // generate 
    for(int note = 0; note < 128; note++) 
    {
        // calculate the maximum representable partial for this note, as found by Nyquist / frequency
        const int partials = static_cast<int>((m_nSamplerate * 0.5f) / (440.f * pow(2.f, static_cast<float>(note - 69) / 12.0f))); 

        float * sawTable = m_pWavetables[kWaveformSaw][note];
        float * squareTable = m_pWavetables[kWaveformSquare][note];
        float * triangleTable = m_pWavetables[kWaveformTriangle][note];

        for(int j = 1; j < partials; j++)
        {
            // frequency-dependent gain scaling to reduce Gibbs phenomenon
            float gibbsComp = cos(j * HALF_PI / static_cast<float>(partials));
            gibbsComp *= gibbsComp;
            gibbsComp /= static_cast<float>(j + 1);
            for(int i = 0; i < m_nTableLength; i++)
            {
                float phase = static_cast<float>(i) / m_nTableLength;
                bool odd = (j % 2) == 0;

                // calculate harmonic, then add if odd, subtract if even
                float harmonic = (1.f / j) * sin(j * phase);

                // apply offset to start of table and dereference, then set value
                *(sawTable + i) += (odd ? -harmonic : harmonic);
                *(squareTable + i) += (odd ? harmonic : 0);
            }
        }

        for(int i = 0; i < m_nTableLength; i++)
        {
            *(triangleTable + i) = fabs(*(sawTable + i));
        }
    }
}

void SCWavetableOsc::reset()
{
    m_fPhase = 0;
}

float SCWavetableOsc::process()
{
    // get indices and residual
    int tableIndex = static_cast<int>(m_fPhase);
    int tableIndex_1 = tableIndex + 1;
    float delta = m_fPhase - static_cast<float>(tableIndex);

    float * table = m_pWavetables[m_nWave][m_nNote];

    // get sample values
    float sample = table[tableIndex];
    float sample_1 = table[tableIndex_1];

    // interpolate
    sample = (1.f - delta) * sample + delta * sample_1;

    // increment phase and wrap
    m_fPhase += m_fPhaseIncrement;
    m_fPhase = (m_fPhase >= m_nTableLength) ? m_fPhase - m_nTableLength : m_fPhase;

    return sample;
}