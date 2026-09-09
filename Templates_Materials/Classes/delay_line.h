#pragma once

#include "ring_buffer.h"

template <typename T, size_t max_delay_samples>
class DelayLine : public RingBuffer<T, max_delay_samples> {
  public:
    /*
     * Process one sample: write input and return delayed output.
     * Uses simple linear interpolation for fractional delay values.
     */
    T processSample(T xn, size_t delay_samples);
    T processSample(T xn, float delay_samples);

    // Process a block with constant integer delay
    void processBlock(const T *input, T *output, size_t num_samples,
                      size_t delay_samples);
};

/* ======================================================== */
/* IMPLEMENTATIONS ======================================== */
/* ======================================================== */

template <typename T, size_t maxDelaySamples>
T DelayLine<T, maxDelaySamples>::processSample(T xn, size_t delaySamples) {
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
void DelayLine<T, maxDelaySamples>::processBlock(const T *input, T *output,
                                                 size_t numSamples,
                                                 size_t delaySamples) {
    for (int i = 0; i < numSamples; ++i)
        output[i] = processSample(input[i], delaySamples);
}
