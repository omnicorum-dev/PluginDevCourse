/*
  ==============================================================================
    distortion.h - Suite of common distoriton types.

    Part of the Intro to Plugin Development Course
    https://github.com/omnicorum-dev/PluginDevCourse

    Copyright (c) 2026 Nicolas Russo
    SPDX-License-Identifier: MIT
  ==============================================================================
*/

#pragma once

#include "filter.h"
#include <algorithm>

class Distortion {
  public:
    virtual float distortion(float xn) = 0;

    float processSample(float xn) {
        float drive_bias = xn * driveMag + bias;
        float yn         = distortion(drive_bias);
        if (dcBlock) {
            yn = dc_block.processSample(yn);
        }
        return yn;
    }

    void setDrive(float _driveMag) { driveMag = _driveMag; }
    void setBias(float _bias) { bias = _bias; }
    void useDcBlock(bool _dcBlock) { dcBlock = _dcBlock; }

  protected:
    float driveMag = 1.f;
    float bias     = 0.f;
    bool  dcBlock  = false;

    DCBlocker dc_block;
};

/* ======================================================== */

class RectifierFull : public Distortion {
  public:
    float distortion(float xn) override {
        if (xn < 0.f)
            return -xn;
        return xn;
    }
};

class RectifierHalf : public Distortion {
  public:
    float distortion(float xn) override {
        if (xn < 0.f)
            return 0.f;
        return xn;
    }
};

class HardClip : public Distortion {
  public:
    float distortion(float xn) override { return std::clamp(xn, -1.f, 1.f); }
};

class TanhShaper : public Distortion {
  public:
    float distortion(float xn) override { return std::tanh(xn); }
};

class SoftClipper : public Distortion {
  public:
    void setThreshold_linear(double threshold) { threshold_linear = threshold; }

    void setThreshold_dB(double threshold) {
        threshold_linear = std::pow(10.0, threshold / 20.0);
    }

    void setKnee_linear(double knee) {
        knee_linear = std::min(2 * threshold_linear, knee);
        halfKnee    = 0.5 * knee_linear;
    }

    void setKnee_dB(double knee) {
        knee_linear = std::min((2 * threshold_linear),
                               (1.0 / std::pow(10.0, knee / 20.0)));
        halfKnee    = 0.5 * knee_linear;
    }

    float distortion(float xn) override {
        float abs  = std::fabs(xn);
        int   sign = xn >= 0 ? 1 : -1;

        if (abs <= threshold_linear - halfKnee) {
            return xn;
        } else if (abs <= threshold_linear - halfKnee) {
            float excess = abs - (threshold_linear - halfKnee);
            return sign * (abs - (excess * excess) / (2 * knee_linear));
        } else {
            return sign * threshold_linear;
        }
    }

  private:
    double threshold_linear;
    double knee_linear;
    double halfKnee;
};
