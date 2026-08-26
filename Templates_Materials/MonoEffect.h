/*
  ==============================================================================

    MonoEffect.h
    Created: 11 Mar 2026 1:10:51pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

// A MonoEffect is an effect that applies to only one channel of audio
// It's a pretty thin base class that makes things like 'effect chains' possible
class MonoEffect {
public:
    virtual ~MonoEffect() = default;
    
    virtual float processSample (float xn) = 0;
    
    virtual void prepare (float _sampleRate, int _blockSize) {
        fs = _sampleRate;
        blockSize = _blockSize;
    }
    
    virtual void processBuffer (double* buffer, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = processSample(buffer[i]);
        }
    }
    
protected:
    float fs;
    int blockSize;
};
