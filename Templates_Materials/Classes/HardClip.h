/*
  ==============================================================================

    HardClip.h
    Created: 1 Mar 2026 4:44:28pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once

class HardClip {
public:
    
    void prepare(float _sampleRate, int _blockSize) {
        fs = _sampleRate;
        blockSize = _blockSize;
        threshold = 1.0f;
    }
    
    void setThreshold (float _threshold) {
        threshold = _threshold;
    }
    
    float process (float xn) {
        if (xn > threshold)
            return threshold;
        else if (xn < -threshold)
            return -threshold;
        else
            return xn;
    }
    
private:
    float fs;
    int blockSize;
    
    float threshold;
};
