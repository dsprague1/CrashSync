/*
  ==============================================================================

    SCOnePoleFilter.cpp
    Created: 4 Nov 2021 3:28:51pm
    Author:  dspra

  ==============================================================================
*/

#include "SCOnePoleFilter.h"
#include "SCMath.h"

SCOnePoleFilter::SCOnePoleFilter():
m_nSamplerate(44100),
m_fZ1(0),
m_fA0(1),
m_fB1(0)
{

}

void SCOnePoleFilter::reset()
{
    m_fZ1 = 0;
}

float SCOnePoleFilter::process(float input)
{
    m_fZ1 = input * m_fA0 + m_fZ1 * m_fB1;
    return m_fZ1;
}

void SCOnePoleFilter::setSamplerate(int samplerate)
{
    m_nSamplerate = samplerate;
}

void SCOnePoleFilter::setCutoff(float frequency)
{
    m_fB1 = exp(-2.f * PI * frequency / static_cast<float>(m_nSamplerate));
    m_fA0 = 1.0 - m_fB1;
}
