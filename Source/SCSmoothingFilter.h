/*
  ==============================================================================

    SCSmoothingFilter.h
    Created: 4 Nov 2021 2:06:09pm
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include <stdint.h>

class SCSmoothingFilter
{
public:
    SCSmoothingFilter();

    void reset();
    float process(float input);
    
    float getCurrentValue() { return m_fZ1; }
    void setCoeff(float coeff) { m_fCoeff = coeff; }
    void setCoeffDbPerSec(float dbPerSec);
    void setSamplerate(int32_t samplerate) { m_nSamplerate = samplerate; }
    void setStartValue(float value) { m_fZ1 = value; }

private:
    float m_fZ1;
    float m_fCoeff;
    int32_t m_nSamplerate;
};