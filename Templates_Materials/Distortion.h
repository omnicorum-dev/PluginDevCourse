/*
  ==============================================================================

    Distortion.h
    Created: 23 Feb 2026 10:51:50am
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

class Distortion {
public:
    Distortion() {
        reset();
    }
    virtual ~Distortion() = default;
    
    virtual float processSample(float xn) = 0;
    
    void processBlock(float* buffer, int numSamples) {
        for (int i = 0; i < numSamples; ++i)
            buffer[i] = processSample(buffer[i]);
    }
    
    virtual void reset() {
        prevInput = 0.f;
        prevOutput = 0.f;
        hysteresisInputDirection = 0;
        hysteresisOutputDirection = 0;
    }
    
    void setDrive (float _driveMag) { driveMag = _driveMag; }
    void setBias  (float _bias)     { bias     = _bias;  }
    
    void updateHysteresis(float xn, float yn) {
        hysteresisInputDirection  = basics::sign(prevInput  - xn);
        hysteresisOutputDirection = basics::sign(prevOutput - yn);
        prevInput  = xn;
        prevOutput = yn;
    }
    
protected:
    float applyDriveBias(float xn) {
        return xn*driveMag + bias;
    }
    
    float prevInput  = 0.f;
    float prevOutput = 0.f;
    
    int   hysteresisInputDirection  = 0;
    int   hysteresisOutputDirection = 0;
    
    float driveMag = 1.f;
    float bias     = 0.f;
    
};

class DistortionADAA : public Distortion {
public:
    virtual double F0 (float xn) = 0;
    virtual double F1 (float xn) = 0;
    
    float processSample(float xn) override {
        float wn    = applyDriveBias(xn);
        double f1_wn = F1(wn);
        double delta = wn - prevInput;

        double yn = (std::abs(delta) < 0.00001f)
                 ? F0(wn)
                 : (f1_wn - prevF1) / delta;

        prevInput = wn;
        prevF1    = f1_wn;
        return yn;
    }
    
    void reset() override {
        prevInput = 0.f;
        prevOutput = 0.f;
        hysteresisInputDirection = 0;
        hysteresisOutputDirection = 0;
        prevF1 = 0.f;
    }
protected:
    float prevF1 = 0.f;
};







class DriveBias : public Distortion {
public:
    float processSample(float xn) override {
        return applyDriveBias(xn);
    }
};

class RectifierFull : public Distortion {
public:
    float processSample(float xn) override {
        float yn;
        float wn = applyDriveBias(xn);
        
        if (wn < 0.f) { yn = -wn; }
        else { yn = wn; }
        
        return yn;
    }
};

class RectifierHalf : public Distortion {
public:
    float processSample(float xn) override {
        float yn;
        float wn = applyDriveBias(xn);
        
        if (wn < 0.f) { yn = 0.f; }
        else { yn = wn; }
        
        return yn;
    }
};

class HardClip : public Distortion {
public:
    float processSample(float xn) override {
        float yn;
        yn = applyDriveBias(xn);
        
        if      (yn >  1.0f)  { yn = 1.0f;  }
        else if (yn < -1.0f)  { yn = -1.0f; }
        
        return yn;
    }
};

class TanhShaper : public Distortion {
public:
    float processSample(float xn) override {
        float yn;
        yn = applyDriveBias(xn);
        
        yn = std::tanh(yn * shape);
        
        return yn;
    }
    
    void setShape(double _shape) {
        shape = _shape;
    }
    
protected:
    double shape = 1.0;
};

class Omni427 : public Distortion {
public:
    float processSample (float xn) override {
        float yn = applyDriveBias(xn);
        double crossover = shape / (shape - 1);
        double n = shape;
        
        if (yn >= 0 && yn <= crossover) {
            yn = yn - ((std::pow(n-1, n-1)/std::pow(n, n)) * std::pow(yn,n));
        } else if ( -crossover <= yn && yn <= 0) {
            yn = yn + ((std::pow(n-1, n-1)/std::pow(n, n)) * std::pow(-yn,n));
        } else if ( yn < -crossover ) {
            yn = -1.0f;
        } else {
            yn = 1.0f;
        }
        
        return yn;
    }
    
    void setShape(double _shape) {
        shape = std::clamp(_shape, 1.1, 100.);
    }
    
protected:
    double shape = 2.0;
};





class TanhADAA : public DistortionADAA {
public:
    double F0 (float xn) override {
        return std::tanh(xn);
    }
    
    double F1 (float xn) override {
        double ax = std::abs(xn);
        return ax + std::log(0.5f + 0.5f * std::exp(-2.0f * ax));
    }
};

class HardClipADAA : public DistortionADAA {
    public:
    double F0 (float xn) override {
        if      (xn >  1.0f)  { return 1.0;  }
        else if (xn < -1.0f)  { return -1.0; }
        return xn;
    }
    
    double F1 (float xn) override {
        double yn;
        
        if (xn <= 1.0f && xn >= -1.0f) { yn = 0.5 * xn * xn; }
        else { yn = xn * basics::sign(xn) - 0.5; }
        
        return yn;
    }
};
