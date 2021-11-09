/*
  ==============================================================================

    SCBiquadFilterUtil.h
    Created: 9 Nov 2021 10:54:36am
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include "SCBiquadFilterCoeff.h"
#include "SCMath.h"

// equations from RBJ cookbook https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
// normalized and shifted around to reduce number of operations

// w0 = 2 * pi * frequency / samplerate
// bw = 1 / q
// alpha = sin(w0) * (bw / 2)
// 
// dividing out all terms by b0 (1 + alpha) results in some common terms that can be separated out 
// beta = 0.5 * (1 - alpha) / (1 + alpha)
// gamma = (0.5 + beta) * cos(w0)
//
// LPF
//
// a0 = (0.5 + beta - gamma) * 0.5f
// a1 = 0.5 + beta - gamma
// a2 = (0.5 + beta - gamma) * 0.5f
// b1 = 2 * gamma
// b2 = -2 * beta
//
// HPF
//
// a0 = (0.5 + beta + gamma) * 0.5f
// a1 = - (0.5 + beta + gamma)
// a2 = (0.5 + beta + gamma) * 0.5f
// b1 = 2 * gamma
// b2 = -2 * beta

SCBiquadFilterCoeff GenerateSecondOrderLpfCoeffs(float frequency, float q, float samplerate)
{
    float w0 = 2.f * PI * frequency / samplerate;
    float bw = 1.f / q;
    float beta = 0.5f * (1.f - (bw / 2.f) * sin(w0)) / (1.f + (bw / 2.f) * sin(w0));
    float gamma = (0.5f + beta) * cos(w0);

    SCBiquadFilterCoeff coeff;
    coeff.a0 = (0.5f + beta - gamma) * 0.5f;
    coeff.a1 = (0.5f + beta - gamma);
    coeff.a2 = coeff.a0;
    coeff.b1 = 2.f * gamma;
    coeff.b2 = -2.f * beta;

    return coeff;
}