/*
  ==============================================================================
    delay_line.h - Basic linear-interpolation delay line.

    Part of the Intro to Plugin Development Course
    https://github.com/omnicorum-dev/PluginDevCourse

    Copyright (c) 2026 Nicolas Russo
    SPDX-License-Identifier: MIT
  ==============================================================================
*/

#pragma once

#include "ring_buffer.h"

template <typename T, size_t max_delay_samples>
class DelayLine : public RingBuffer<T, max_delay_samples> {
  public:
    void setFeedback(float feedback) { feedback_amt = feedback; }

    void setDelaySamples(float delay) { delay_samples = delay; }

    T readFractional(float _delay_samples) {
        int   d0   = (int)_delay_samples;
        float frac = _delay_samples - (float)d0;
        float s0   = this->read(d0);
        float s1   = this->read(d0 + 1);
        return s0 + frac * (s1 - s0);
    }

    T processSample(T xn) {
        float delayed = readFractional(delay_samples);
        float toStore = xn + delayed * feedback_amt;
        this->push(toStore);
        return delayed;
    }

  private:
    float feedback_amt  = 0.f;
    float delay_samples = 100.f;
};
