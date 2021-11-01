/*
  ==============================================================================

    SCEnvelopeFollower.h
    Created: 1 Nov 2021 7:13:35pm
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include <cstdint>

class SCEnvelopeFollower
{
public:
    SCEnvelopeFollower();

    float process(float input);

    void setSamplerate(int32_t samplerate);
    void setAttackTimeMs(float attackTime);
    void setReleaseTimeMs(float releaseTime);
private:

    float m_fZ1;
    float m_fAttackCoeff;
    float m_fReleaseCoeff;
    float m_fAttackTimeMs;
    float m_fReleaseTimeMs;
    int32_t m_nSamplerate;
};