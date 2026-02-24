/*
  ==============================================================================

    JStereo.h
    Created: 23 Feb 2026 6:14:54pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Panner.h"
#include "MidSide.h"

namespace JStereo
{

inline void encodeMS(juce::AudioBuffer<float>& buffer) {
    MS::encode(buffer.getWritePointer(0),
               buffer.getWritePointer(1),
               buffer.getNumSamples());
}

inline void decodeMS(juce::AudioBuffer<float>& buffer) {
    MS::decode(buffer.getWritePointer(0),
               buffer.getWritePointer(1),
               buffer.getNumSamples());
}

/* pan: -1.0 to 1.0 */
inline void applyConstantPowerPan(juce::AudioBuffer<float>& buffer, float pan) {
    auto gains = Panner::constantPower(pan);
    Panner::applyGain(buffer.getWritePointer(0), buffer.getNumSamples(), gains.L);
    Panner::applyGain(buffer.getWritePointer(1), buffer.getNumSamples(), gains.R);
}

/* pan: -1.0 to 1.0 */
inline void applyLinearPan(juce::AudioBuffer<float>& buffer, float pan) {
    auto gains = Panner::linear(pan);
    Panner::applyGain(buffer.getWritePointer(0), buffer.getNumSamples(), gains.L);
    Panner::applyGain(buffer.getWritePointer(1), buffer.getNumSamples(), gains.R);
}

} // namespace JStereo
