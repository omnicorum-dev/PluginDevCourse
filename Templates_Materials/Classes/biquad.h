/*
  ==============================================================================
    biquad.h - Virtual class for a traditional biquad IIR filter.

    Part of the Intro to Plugin Development Course
    https://github.com/omnicorum-dev/PluginDevCourse

    Copyright (c) 2026 Nicolas Russo
    SPDX-License-Identifier: MIT
  ==============================================================================
*/

#pragma once

class Biquad {
  public:
    virtual void updateCoeffs() = 0;

    Biquad() { reset(); }

    void prepare(int _sample_rate, int _buffer_size) {
        fs          = _sample_rate;
        buffer_size = _buffer_size;
    }

    void reset() {
        f0    = 1000.f;
        Q     = 0.7071f;
        A     = 1.f;
        w0    = 0;
        alpha = 0;
        b0    = 1.f;
        b1    = 0.f;
        b2    = 0.f;
        a0    = 1.f;
        a1    = 0.f;
        a2    = 0.f;
    }

    void setFreq(float new_freq) {
        f0 = new_freq;
        updateCoeffs();
    }

    void setQ(float new_Q) {
        Q = new_Q;
        updateCoeffs();
    }

    void setA(float new_A) {
        A = new_A;
        updateCoeffs();
    }

    void setAll(float new_freq, float new_Q, float new_A) {
        f0 = new_freq;
        Q  = new_Q;
        A  = new_A;
        updateCoeffs();
    }

    float processSample(float xn) {
        float feedforward = (b0 * xn) + (b1 * xnm1) + (b2 * xnm2);
        float feedback    = (a1 * ynm1) + (a2 * ynm2);
        float yn          = (1.f / a0) * (feedforward - feedback);

        xnm2 = xnm1;
        xnm1 = xn;
        ynm2 = ynm1;
        ynm1 = yn;

        return yn;
    }

  protected:
    float fs;
    int   buffer_size;

    float f0, Q, A;
    float w0, alpha;
    float a0, a1, a2, b0, b1, b2;
    float xnm1, xnm2, ynm1, ynm2;
};
