/*
  ==============================================================================

    AudioDetector.h
    Created: 23 Feb 2026 6:53:33pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

enum class ADMode {
    Peak,
    MS,
    RMS
};

class AudioDetector {
public:
    void prepare(float _sampleRate, int _blockSize) {
        fs = _sampleRate;
        blockSize = _blockSize;
    }
    
    double processSample(double xn) {
        /* Full-wave rectification */
        double input = std::fabs(xn);
        
        /* Square for MS and RMS */
        if (detectMode == ADMode::MS || detectMode == ADMode::RMS) {
            input *= input;
        }
        
        double currEnvelope = 0.0;
        
        /* Simulate RC */
        if (input > lastEnvelope) {
            currEnvelope = attack * (lastEnvelope-input) + input;
        } else {
            currEnvelope = release * (lastEnvelope-input) + input;
        }
        
        //checkFloatUnderflow();
        
        if (clampToUnityMax)
            currEnvelope = std::fmin(currEnvelope, 1.0);
        
        /* Envelope cannot be negative */
        currEnvelope = fmax(currEnvelope, 0.0);
        
        lastEnvelope = currEnvelope;
        
        if (detectMode == ADMode::RMS) {
            currEnvelope = std::pow(currEnvelope, 0.5);
        }
        
        if (!detect_dB) {
            return currEnvelope;
        }
        
        if (currEnvelope <= 0) return -96.0;
        
        return basics::mag2db(currEnvelope);
    }
    
    void setAttackTime(double ms) {
        if (attackTime_ms == ms) return;
        
        attackTime_ms = ms;
        attack = std::exp(timeConstant / (attackTime_ms * fs * 0.001));
    }
    
    void setReleaseTime(double ms) {
        if (releaseTime_ms == ms) return;
        
        releaseTime_ms = ms;
        release = std::exp(timeConstant / (releaseTime_ms * fs * 0.001));
    }
    
    void setDetectionMode (ADMode mode) {
        detectMode = mode;
    }
    
    void setDetectDb (bool on) {
        detect_dB = on;
    }
    
    void setClampToUnityMax (bool on) {
        clampToUnityMax = on;
    }
    
    void reset() {
        lastEnvelope = 0.0;
    }
    
protected:
    float fs;
    int blockSize;
    
    double attackTime_ms  = 0.0;
    double releaseTime_ms = 0.0;
    
    double attack  = 0.0;
    double release = 0.0;
    
    ADMode detectMode = ADMode::Peak;
    bool detect_dB = false; /* Default to linear */
    bool clampToUnityMax = false;
    
    double lastEnvelope   = 0.0;
    
    const double timeConstant = -0.99967234081320612357829304641019;
    //const double timeConstant = std::log(0.001);
};
