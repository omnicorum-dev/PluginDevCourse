#pragma once

#include "biquad.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

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
        w0          = 2 * M_PI * f0 / fs;
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

// Higher-Order RBJ Filter Base

template <size_t max_stages> class HighOrderRBJBase {
  public:
    virtual ~HighOrderRBJBase() = default;

    void prepare(float _sample_rate, int _buffer_size) {
        sample_rate = _sample_rate;
        for (auto &s : stages)
            s.prepare(_sample_rate, _buffer_size);
        updateCoeffs();
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

    void setOrder(int order) {
        order      = std::max(2, order - (order % 2));
        num_stages = std::min(order / 2, (int)max_stages);
        for (int i = 0; i < num_stages; ++i)
            stages[i].setFilterType(filter_type);
        updateCoeffs();
    }

    void setFreq(float freq) {
        target_freq = freq;
        updateCoeffs();
    }

    float processSample(float xn) {
        float yn = xn;
        for (int i = 0; i < num_stages; ++i)
            yn = stages[i].processSample(yn);
        return yn;
    }

    int   getNumStages() const { return num_stages; }
    float getStageFreq(int i) const { return stage_freq[i]; }
    float getStageQ(int i) const { return stage_q[i]; }

  protected:
    virtual void updateCoeffs() = 0;

    void computeAndApply(const std::array<float, max_stages> &relFreq,
                         const std::array<float, max_stages> &q,
                         bool                                 solveFor3dB) {
        if (sample_rate <= 0.f || num_stages <= 0)
            return;

        float scale = solveFor3dB ? solveScaleFor3db(relFreq, q) : 1.f;

        for (int m = 0; m < num_stages; ++m) {
            stage_freq[m] = target_freq * scale * relFreq[m];
            stage_q[m]    = q[m];
            stages[m].setFreq(stage_freq[m]);
            stages[m].setQ(stage_q[m]);
        }
    }

  private:
    double magSquaredAt(float f0_stage, float Q_stage, float f_eval) const {
        double w0    = 2 * M_PI * (double)f0_stage / (double)sample_rate;
        double alpha = std::sin(w0) / (2.0 * (double)Q_stage);
        double coswo = std::cos(w0);

        double b0, b1, b2;
        if (filter_type == FilterType::HIGHPASS) {
            b0 = (1.0 + coswo) / 2.0;
            b1 = -(1.0 + coswo);
            b2 = (1.0 + coswo) / 2.0;
        } else { // LOWPASS
            b0 = (1.0 - coswo) / 2.0;
            b1 = 1.0 - coswo;
            b2 = (1.0 - coswo) / 2.0;
        }
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * coswo;
        double a2 = 1.0 - alpha;

        double w = 2 * M_PI * (double)f_eval / (double)sample_rate;
        std::complex<double> z1 = std::polar(1.0, -w);
        std::complex<double> z2 = std::polar(1.0, -2.0 * w);

        std::complex<double> num = b0 + b1 * z1 + b2 * z2;
        std::complex<double> den = a0 + a1 * z1 + a2 * z2;
        return std::norm(num / den);
    }

    double compositeMagSq(float s, const std::array<float, max_stages> &relFreq,
                          const std::array<float, max_stages> &q) const {
        double prod = 1.0;
        for (int m = 0; m < num_stages; ++m) {
            prod *=
                magSquaredAt(target_freq * s * relFreq[m], q[m], target_freq);
        }
        return prod;
    }

    float solveScaleFor3db(const std::array<float, max_stages> &relFreq,
                           const std::array<float, max_stages> &q) const {
        float  s_lo = 1e-3f, s_hi = 1e3f;
        double f_lo = compositeMagSq(s_lo, relFreq, q) - 0.5;

        for (int i = 0; i < 60; ++i) {
            float  mid   = 0.5f * (s_lo + s_hi);
            double f_mid = compositeMagSq(mid, relFreq, q) - 0.5;
            if ((f_mid > 0) == (f_lo > 0)) {
                s_lo = mid;
                f_lo = f_mid;
            } else {
                s_hi = mid;
            }
        }

        return 0.5f * (s_lo + s_hi);
    }

  protected:
    FilterType filter_type = FilterType::LOWPASS;
    int        num_stages  = 2; // 4th order by default
    float      sample_rate = 48000.f;
    float      target_freq = 1000.f;

  private:
    std::array<RBJ, max_stages>   stages;
    std::array<float, max_stages> stage_freq{};
    std::array<float, max_stages> stage_q{};
};

// Uniform-Q Higher-Order Filters
// every stage shares one Q (real resonance control)
// can peak / approach self-oscillation at high Q, similar to an analog ladder
// filter. f0 is always the numerically-corrected true -3dB point.

template <size_t max_stages>
class UniformQRBJ : public HighOrderRBJBase<max_stages> {
  public:
    void setQ(float q) { uniform_q = std::max(q, 0.01f); }

  protected:
    void updateCoeffs() override {
        std::array<float, max_stages> relFreq{};
        std::array<float, max_stages> q{};
        for (int m = 0; m < this->num_stages; ++m) {
            relFreq[m] = 1.f;
            q[m]       = uniform_q;
        }
        computeAndApply(relFreq, q, true);
    }

  private:
    float uniform_q = 0.7071f;
};

// Butterworth Higher-Order Filters
// maximally flat passband. no Q control.
// each stage's Q is fully determined by filter order.
// all stages share f0 exactly, so -3dB at f0 is exact.

template <size_t max_stages>
class ButterworthFilter : public HighOrderRBJBase<max_stages> {
  protected:
    void updateCoeffs() override {
        std::array<float, max_stages> relFreq{};
        std::array<float, max_stages> q{};

        int Nord = this->num_stages * 2;
        for (int m = 0; m < this->num_stages; ++m) {
            float theta = (2.f * (m + 1) - 1.f) * (float)M_PI / (2.f * Nord);
            relFreq[m]  = 1.f;
            q[m]        = 1.f / (2.f * std::cos(theta));
        }

        computeAndApply(relFreq, q, false);
    }
};

// Chebychev Higher-Order Filters
// equiripple passband. steeper rolloff than butterworth for
// the same order at the cost of passband ripple.
// each stage gets its own Q and its own relative freq.
//
// Textbook Chebychev cutoff is conventionally the 'ripple-edge frequency,'
// NOT the -3dB point.'useTrue3dB' will have it use the true -3dB point
// anyway, but this can be disabled for traditional Chebychev behaviour

template <size_t max_stages>
class ChebychevFilter : public HighOrderRBJBase<max_stages> {
  protected:
    void updateCoeffs() override {
        std::array<float, max_stages> relFreq{};
        std::array<float, max_stages> q{};

        int    Nord = this->num_stages * 2;
        double eps  = std::sqrt(std::pow(10.0, ripple_db / 10.0) - 1.0);
        double v0   = std::asinh(1.0 / eps) / Nord;
        double sh = std::sinh(v0), ch = std::cosh(v0);

        for (int m = 0; m < this->num_stages; ++m) {
            double theta = (2.0 * (m + 1) - 1.0) * M_PI / (2.0 * Nord);
            double re    = -sh * std::sin(theta);
            double im    = ch * std::cos(theta);
            double r     = std::sqrt(re * re + im * im);
            relFreq[m]   = (float)r;
            q[m]         = (float)(r / (-2.0 * re));
        }

        computeAndApply(relFreq, q, use_true_3dB);
    }

  private:
    float ripple_db    = 1.f;
    bool  use_true_3dB = true;
};
