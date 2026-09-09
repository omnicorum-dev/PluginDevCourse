#pragma once

#include "biquad.h"
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
    count
};

// The Robert Bristow-Johnston 2-pole EQ-Cookbook Filters

class RBJ : public Biquad {
  public:
    void setFilterType(FilterType new_filter_type) {
        filter_type = new_filter_type;
        updateCoeffs();
    }

    void updateCoeffs() override {
        w0          = M_2_PI * f0 / fs;
        alpha       = sin(w0) / (2 * Q);
        float coswo = cos(w0);
        float sinwo = sin(w0);

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
