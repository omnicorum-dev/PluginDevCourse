/*
  ==============================================================================

    DelayLine.h
    Created: 22 Feb 2026 10:10:26pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"
#include "RingBuffer.h"

template <typename T, size_t maxDelaySamples>
class DelayLine : public RingBuffer<T, maxDelaySamples> {
public:
    
    /*
     * Process one sample: Write input and return delayed output
     * 'float delaySamples' uses linear interpolation for fractional delay values
     */
    T processSample (T xn, int delaySamples);
    T processSample (T xn, float delaySamples);
    
    /* Process a block with constant integer delay */
    void processBlock (const T* input, T* output, int numSamples, int delaySamples);
    
};





template <typename T, size_t maxDelaySamples>
T DelayLine<T, maxDelaySamples>::processSample(T xn, int delaySamples) {
    T delayed = this->read(delaySamples);
    this->push(xn);
    return delayed;
}

template <typename T, size_t maxDelaySamples>
T DelayLine<T, maxDelaySamples>::processSample(T xn, float delaySamples) {
    int   d0      = static_cast<int>(delaySamples);
    float frac    = delaySamples - static_cast<float>(d0);
    T     s0      = this->read(d0);
    T     s1      = this->read(d0 + 1);
    T     delayed = s0 + static_cast<T>(frac * (s1 - s0));
    this->push(xn);
    return delayed;
}

template <typename T, size_t maxDelaySamples>
void DelayLine<T, maxDelaySamples>::processBlock(const T *input, T *output, int numSamples, int delaySamples) {
    for (int i = 0; i < numSamples; ++i)
        output[i] = processSample(input[i], delaySamples);
}
