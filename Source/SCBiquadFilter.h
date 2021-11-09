/*
  ==============================================================================

    SCBiquadFilter.h
    Created: 9 Nov 2021 10:54:08am
    Author:  dspra

  ==============================================================================
*/

#pragma once
#include "SCBiquadFilterCoeff.h"

template<int numBands>
class SCMultibandBiquadFilter
{
public:
    SCMultibandBiquadFilter()
    {
        m_pZ = new float[numBands * 4];
        reset();
    }

    ~SCMultibandBiquadFilter()
    {
        delete [] m_pZ;
    }

    float process(float x)
    {
        float y = x;
        float * memory = m_pZ;

        for(int i = 0; i < numBands; i++)
        {
            y = m_coeff[i].a0 * x +
                m_coeff[i].a1 * memory[0] +
                m_coeff[i].a2 * memory[1] +
                m_coeff[i].b1 * memory[2] +
                m_coeff[i].b2 * memory[3];

            memory[1] = memory[0];
            memory[0] = x;

            x = y;
            memory += 2;
        }
        memory[1] = memory[0];
        memory[0] = y;
        return y;
    }
    
    void setCoeffsForBand(SCBiquadFilterCoeff coeff, int band)
    {
        m_coeff[band] = coeff;
    }

    void reset()
    {
        std::memset(m_pZ, 0, numBands * 2 + 2);
    }

private:
    SCBiquadFilterCoeff m_coeff[numBands];
    float * m_pZ;
};