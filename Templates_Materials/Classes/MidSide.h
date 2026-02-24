/*
  ==============================================================================

    MidSide.h
    Created: 23 Feb 2026 6:06:03pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

namespace MS
{

inline void encode (float* left, float* right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        float mid  = (left[i] + right[i]) * 0.5f;
        float side = (left[i] - right[i]) * 0.5f;
        left[i]  = mid;
        right[i] = side;
    }
}

inline void decode (float* mid, float* side, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        float left  = mid[i] + side[i];
        float right = mid[i] - side[i];
        mid[i]  = left;
        side[i] = right;
    }
}

} // namespace MS
