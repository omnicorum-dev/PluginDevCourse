#pragma once

#include <algorithm>

class HardClip {
  public:
    void prepare(float _sample_rate, int _buffer_size) {
        fs          = _sample_rate;
        buffer_size = _buffer_size;
        threshold   = 1.f;
    }

    void setThreshold(float _threshold) { threshold = _threshold; }

    float processSample(float xn) {
        return std::clamp(xn, -threshold, threshold);
    }

  private:
    float fs;
    int   buffer_size;

    float threshold;
};
