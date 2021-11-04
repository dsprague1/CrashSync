/*
  ==============================================================================

    SCSmoothingFilter.cpp
    Created: 4 Nov 2021 2:06:09pm
    Author:  dspra

  ==============================================================================
*/

#include "SCSmoothingFilter.h"
#include <cmath>

SCSmoothingFilter::SCSmoothingFilter():
m_fCoeff(0.f),
m_fZ1(0),
m_nSamplerate(44100)
{

}

void SCSmoothingFilter::reset()
{
    m_fZ1 = 0;
}

float SCSmoothingFilter::process(float input)
{
    // y = coeff * x[t] + (1 - coeff) * y[t - 1]
    m_fZ1 = m_fZ1 + m_fCoeff * (input - m_fZ1);
    return m_fZ1;
}

void SCSmoothingFilter::setCoeffDbPerSec(float dbPerSec)
{
    m_fCoeff = std::expm1(-dbPerSec * std::log(10.f) / (20.f * m_nSamplerate));
}