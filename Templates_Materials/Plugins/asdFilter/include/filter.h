/*
  ==============================================================================
    filter.h - Collection of common filter implementations,
        including the Robert Bristow-Johnston filters, Linkwitz-Riley
        crossover filters, a standalone Butterworth filter, a
        Butterworth-based Higher-Order filter with kHs Filter-style
        adjustable resonance on its last cascade stage, and a DC blocker.

    Part of the Intro to Plugin Development Course
    https://github.com/omnicorum-dev/PluginDevCourse

    Copyright (c) 2026 Nicolas Russo
    SPDX-License-Identifier: MIT
  ==============================================================================
*/

#pragma once

#include "biquad.h"
#include <algorithm>
#include <array>
#include <cmath>

enum class FilterType {
    LOWPASS = 0,
    HIGHPASS,
    BANDPASS_SKIRT,
    BANDPASS_PEAK,
    NOTCH,
    BELL,
    HIGHSHELF,
    LOWSHELF,
    ALLPASS,
};

// The Robert Bristow-Johnston 2-pole EQ-Cookbook Filters

class RBJ : public Biquad {
  public:
    void setFilterType(FilterType new_filter_type) {
        filter_type = new_filter_type;
        updateCoeffs();
    }

    void updateCoeffs() override {
        w0          = 2.f * (float)M_PI * f0 / fs;
        alpha       = sin(w0) / (2 * Q);
        float coswo = cos(w0);

        switch (filter_type) {
        case FilterType::LOWPASS:
            b0 = (1.f - coswo) / 2.f;
            b1 = 1.f - coswo;
            b2 = (1.f - coswo) / 2.f;
            a0 = 1.f + alpha;
            a1 = -2.f * coswo;
            a2 = 1.f - alpha;
            break;
        case FilterType::HIGHPASS:
            b0 = (1.f + coswo) / 2.f;
            b1 = -(1.f + coswo);
            b2 = (1.f + coswo) / 2.f;
            a0 = 1.f + alpha;
            a1 = -2.f * coswo;
            a2 = 1.f - alpha;
            break;
        case FilterType::BANDPASS_SKIRT:
            b0 = Q * alpha;
            b1 = 0;
            b2 = -Q * alpha;
            a0 = 1.f + alpha;
            a1 = -2.f * coswo;
            a2 = 1.f - alpha;
            break;
        case FilterType::BANDPASS_PEAK:
            b0 = alpha;
            b1 = 0;
            b2 = -alpha;
            a0 = 1.f + alpha;
            a1 = -2.f * coswo;
            a2 = 1.f - alpha;
            break;
        case FilterType::NOTCH:
            b0 = 1.f;
            b1 = -2.f * coswo;
            b2 = 1.f;
            a0 = 1.f + alpha;
            a1 = -2.f * coswo;
            a2 = 1.f - alpha;
            break;
        case FilterType::BELL:
            b0 = 1.f + (alpha * A);
            b1 = -2.f * coswo;
            b2 = 1.f - (alpha * A);
            a0 = 1.f + (alpha / A);
            a1 = -2.f * coswo;
            a2 = 1.f - (alpha / A);
            break;
        case FilterType::HIGHSHELF: {
            float sqrtA = std::sqrt(A);

            b0 = A * ((A + 1.f) + ((A - 1.f) * coswo) + (2 * alpha * sqrtA));
            b1 = -2 * A * ((A - 1.f) + ((A + 1.f) * coswo));
            b2 = A * ((A + 1.f) + ((A - 1.f) * coswo) - (2 * alpha * sqrtA));
            a0 = (A + 1.f) - ((A - 1.f) * coswo) + (2 * alpha * sqrtA);
            a1 = 2 * ((A - 1.f) - ((A + 1.f) * coswo));
            a2 = (A + 1.f) - ((A - 1.f) * coswo) - (2 * alpha * sqrtA);
            break;
        }
        case FilterType::LOWSHELF: {
            float sqrtA = std::sqrt(A);

            b0 = A * ((A + 1.f) - ((A - 1.f) * coswo) + (2 * alpha * sqrtA));
            b1 = 2 * A * ((A - 1.f) - ((A + 1.f) * coswo));
            b2 = A * ((A + 1.f) - ((A - 1.f) * coswo) - (2 * alpha * sqrtA));
            a0 = (A + 1.f) + ((A - 1.f) * coswo) + (2 * alpha * sqrtA);
            a1 = -2 * ((A - 1.f) + ((A + 1.f) * coswo));
            a2 = (A + 1.f) + ((A - 1.f) * coswo) - (2 * alpha * sqrtA);
            break;
        }
        case FilterType::ALLPASS:
            b0 = 1.f - alpha;
            b1 = -2 * coswo;
            b2 = 1.f + alpha;
            a0 = 1.f + alpha;
            a1 = -2 * coswo;
            a2 = 1 - alpha;
            break;
        default:
            break;
        }
    }

  private:
    FilterType filter_type = FilterType::LOWPASS;
};

// 4-pole Linkwitz-Riley Crossover Filters

class LR4 {
  public:
    void prepare(float _sample_rate, int _buffer_size) {
        stage1.prepare(_sample_rate, _buffer_size);
        stage2.prepare(_sample_rate, _buffer_size);

        stage1.setQ(0.7071f);
        stage2.setQ(0.7071f);
    }

    void setFilterType(FilterType new_filter_type) {
        if (new_filter_type != FilterType::LOWPASS &&
            new_filter_type != FilterType::HIGHPASS) {
            new_filter_type = FilterType::LOWPASS;
        }

        stage1.setFilterType(new_filter_type);
        stage2.setFilterType(new_filter_type);
    }

    void setFreq(float new_freq) {
        stage1.setFreq(new_freq);
        stage2.setFreq(new_freq);
    }

    float processSample(float xn) {
        float wn = stage1.processSample(xn);
        return stage2.processSample(wn);
    }

  private:
    RBJ stage1, stage2;
};

// Simple DC Blocker

class DCBlocker {
  public:
    float processSample(float xn) {
        float yn = xn - xnm1 + R * ynm1;

        xnm1 = xn;
        ynm1 = yn;

        return yn;
    }

  private:
    static constexpr float R = 0.9999f;

    float xnm1 = 0.f;
    float ynm1 = 0.f;
};

// High-Order Filter. At 1 stage, it behaves like an RBJ filter.
// At higher stages, a Q of 0.7071 makes it behave like a Butterworth.
// Modifying Q results in natural behaviour

template <size_t max_stages> class HighOrderFilter {
  public:
    void prepare(float _sample_rate, int _buffer_size) {
        sample_rate = _sample_rate;
        for (auto &s : stages)
            s.prepare(_sample_rate, _buffer_size);
        updateCoeffs();
    }

    float processSample(float xn) {
        float yn = xn;
        for (int i = 0; i < num_stages; ++i)
            yn = stages[i].processSample(yn);
        return yn;
    }

    void setFilterType(FilterType type) {
        if (type != FilterType::LOWPASS && type != FilterType::HIGHPASS) {
            type = FilterType::LOWPASS;
        }
        filter_type = type;
        for (int i = 0; i < num_stages; ++i)
            stages[i].setFilterType(filter_type);
        updateCoeffs();
    }

    void setStages(int _stages) {
        num_stages = std::max(1, std::min(_stages, (int)max_stages));
        for (int i = 0; i < num_stages; ++i)
            stages[i].setFilterType(filter_type);
        updateCoeffs();
    }

    void setFreq(float freq) {
        target_freq = freq;
        updateCoeffs();
    }

    void setQ(float q) {
        resonance_q = std::max(q, 0.01f);
        updateCoeffs();
    }

    void setFreqAndQ(float freq, float q) {
        target_freq = freq;
        resonance_q = std::max(q, 0.01f);
        updateCoeffs();
    }

    int   getNumStages() const { return num_stages; }
    float getStageFreq(int i) const { return stage_freq[i]; }
    float getStageQ(int i) const { return stage_q[i]; }

  protected:
    void updateCoeffs() {
        if (sample_rate <= 0.f || num_stages <= 0)
            return;

        std::array<float, max_stages> relFreq{};
        std::array<float, max_stages> q{};

        int   n    = num_stages;
        float Nord = (float)n * 2;

        for (int m = 0; m < n; ++m) {
            float theta =
                (2.f * ((float)m + 1.f) - 1.f) * (float)M_PI / (2.f * Nord);
            relFreq[m] = 1.f;
            q[m]       = 1.f / (2.f * std::cos(theta));
        }

        constexpr float neutralQ = 0.7071f;
        q[n - 1] *= (resonance_q / neutralQ);

        for (int m = 0; m < num_stages; ++m) {
            stage_freq[m] = target_freq * relFreq[m];
            stage_q[m]    = q[m];
            stages[m].setFreq(stage_freq[m]);
            stages[m].setQ(stage_q[m]);
        }
    }

  protected:
    FilterType filter_type = FilterType::LOWPASS;
    int        num_stages  = 2;
    float      sample_rate = 48000.f;
    float      target_freq = 1000.f;

  private:
    float resonance_q = 0.7071f;

    std::array<RBJ, max_stages>   stages;
    std::array<float, max_stages> stage_freq{};
    std::array<float, max_stages> stage_q{};
};
