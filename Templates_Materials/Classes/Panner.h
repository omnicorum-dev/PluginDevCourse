/*
  ==============================================================================

    Panner.h
    Created: 23 Feb 2026 6:05:56pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

namespace Panner
{

struct Gains { float L; float R; };

/* pan: -1.0 to 1.0 */
inline Gains constantPower(float pan) {
    constexpr float halfPi = M_PI / 2.0f;
    float angle = (pan + 1.0f) * halfPi / 2.0f;
    return {
        std::cos(angle) * 1.41421356f,
        std::sin(angle) * 1.41421356f
    };
}

/* pan: -1.0 to 1.0 */
inline Gains linear(float pan) {
    return {
        1.0f - std::max(0.0f, pan),
        1.0f + std::min(0.0f, pan)
    };
}

inline void applyGain(float* channel, int numSamples, float gain) {
    for (int i = 0; i < numSamples; ++i)
                channel[i] *= gain;
}

} // namespace Panner


