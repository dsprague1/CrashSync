/*
  ==============================================================================

    SCEnvelopeFollower.cpp
    Created: 1 Nov 2021 7:13:35pm
    Author:  dspra

  ==============================================================================
*/

#include "SCEnvelopeFollower.h"
#include <cmath>

SCEnvelopeFollower::SCEnvelopeFollower():
m_fAttackCoeff(1.f),
m_fReleaseCoeff(1.f),
m_fAttackTimeMs(1.f),
m_fReleaseTimeMs(1.f),
m_fZ1(0.f),
m_nSamplerate(44100)
{
}

float SCEnvelopeFollower::process(float input)
{
    float sig = fabs(input);
    m_fZ1 = (sig > m_fZ1) ? (m_fZ1 - sig) * m_fAttackCoeff : (m_fZ1 - sig) * m_fReleaseCoeff;
    m_fZ1 = m_fZ1 + sig;
    return m_fZ1;
}

void SCEnvelopeFollower::setSamplerate(int32_t samplerate)
{
    m_nSamplerate = samplerate;
    setAttackTimeMs(m_fAttackTimeMs);
    setReleaseTimeMs(m_fReleaseTimeMs);
}

void SCEnvelopeFollower::setAttackTimeMs(float attackTime)
{
    m_fAttackTimeMs = attackTime;
    m_fAttackCoeff = pow(0.01f, 1.f /(m_fAttackTimeMs * m_nSamplerate * 0.001f));
}

void SCEnvelopeFollower::setReleaseTimeMs(float releaseTime)
{
    m_fReleaseTimeMs = releaseTime;
    m_fReleaseCoeff = pow(0.01f, 1.f /(m_fReleaseTimeMs * m_nSamplerate * 0.001f));
}
