/*
  ==============================================================================

    ImpulseConvolver.h
    Created: 22 Feb 2026 11:24:41pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"
#include "RingBuffer.h"
#include "LinearBuffer.h"
#include "Samples.h"

template <typename T>
class ImpulseConvolver {
public:
    
    void loadIR(T* buffer, unsigned int bufferLength) {
        irBuffer.loadSample(buffer, bufferLength);
    }
    
    double processSample(T xn) {
        double output = 0.0;
        
        signalBuffer.push(xn);
        
        for (unsigned int i = 0; i < irBuffer.getBufferLength(); ++i) {
            double signal = signalBuffer.read((int)i);
            double ir     = irBuffer.readBuffer((int)i);
            output += signal*ir;
        }
        
        return output;
    }
    
protected:
    RingBuffer<double, 480000> signalBuffer;
    LinearBuffer<double>       irBuffer;
};
