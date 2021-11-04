/*
  ==============================================================================

    SCOnePoleFilter.h
    Created: 4 Nov 2021 3:28:51pm
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include <cstdint>

class SCOnePoleFilter
{
public:
    SCOnePoleFilter();

    void reset();
    float process(float input);

    void setSamplerate(int samplerate);
    void setCutoff(float frequency);
private:

    float m_fZ1;
    float m_fA0;
    float m_fB1;

    int m_nSamplerate;
};