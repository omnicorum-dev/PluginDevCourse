/*
  ==============================================================================

    LinearBuffer.h
    Created: 22 Feb 2026 11:15:15pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

template <typename T>
class LinearBuffer {
public:
    void loadSample(T* sample, unsigned int length) {
        buffer = sample;
        bufferLength = length;
    }
    
    T readBuffer(int index) {
        int i = std::clamp(index, 0, bufferLength);
        return buffer[i];
    }
    
    int getBufferLength() const {
        return bufferLength;
    }
    
protected:
    T* buffer;
    int bufferLength;
};
