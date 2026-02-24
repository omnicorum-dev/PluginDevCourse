/*
  ==============================================================================

    EnvelopeFollower.h
    Created: 23 Feb 2026 6:40:26pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "AudioDetector.h"
#include "Basics.h"
#include "Biquad.h"

class AutoWah {
public:
    
    void prepare(float sampleRate, int samplesPerBlock) {
        fs = sampleRate;
        blockSize = samplesPerBlock;
        filter.prepare(fs, blockSize);
        detector.prepare(fs, blockSize);
        
        filter.setFilterType(FilterType::BANDPASS);
        filter.setAll(baseFreq, Q);
        
        detector.setAttackTime(attackTime_ms);
        detector.setReleaseTime(releaseTime_ms);
        detector.setDetectionMode(ADMode::RMS);
        detector.setDetectDb(true);
        detector.setClampToUnityMax(false);
    }
    
    float processSample (float xn) {
        double threshValue = basics::db2mag(threshold_dB);
        
        double detect_dB = detector.processSample(xn);
        double detectValue = basics::db2mag(detect_dB);
        double deltaValue = detectValue - threshValue;
        
        if (deltaValue > 0.0) {
            double modulatorValue = 0.0;
            
            modulatorValue = std::clamp(deltaValue * sensitivity, 0.0, 1.0);
            
            double newFreq = basics::doUnipolarModulationFromMin(modulatorValue, baseFreq, peakFreq);
            
            filter.setFreq(newFreq);
        }
        
        return filter.processSample(xn);
    }
    
    void setBaseFreq(double freq)     { baseFreq = freq; }
    void setPeakFreq(double freq)     { peakFreq = freq; }
    void setAttackTime(double ms)     { attackTime_ms = ms; detector.setAttackTime(attackTime_ms); }
    void setReleaseTime(double ms)    { releaseTime_ms = ms; detector.setReleaseTime(releaseTime_ms); }
    void setThresh(double dB)         { threshold_dB = dB; }
    void setSensitivity (double sens) { sensitivity = sens; }
    
private:
    float fs;
    int blockSize;
    
    double baseFreq = 1000.0;
    double peakFreq = 15000.0;
    double Q = 1.5;
    double attackTime_ms  = 10.0;
    double releaseTime_ms = 10.0;
    double threshold_dB   = 0.0;
    double sensitivity    = 1.0;
    
    RBJ filter;
    AudioDetector detector;
};
