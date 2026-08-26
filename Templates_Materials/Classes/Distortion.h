/*
  ==============================================================================

    Distortion.h
    Created: 11 Mar 2026 1:11:00pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"
#include "MonoEffect.h"

class Distortion : public MonoEffect {
public:
    virtual float distortion(float xn) = 0;
    
    float processSample(float xn) override {
        return distortion(applyDriveBias(xn));
    };
    
    void setDrive (float _driveMag) { driveMag = _driveMag; }
    void setBias  (float _bias)     { bias     = _bias; }
    
protected:
    float applyDriveBias(float xn) {
        return xn * driveMag + bias;
    }
    
    float driveMag = 1.f;
    float bias     = 0.f;
};





class RectifierFull : public Distortion {
public:
    float distortion(float xn) override {
        if (xn < 0.f) { return -xn; }
        return xn;
    }
};

class RectifierHalf : public Distortion {
public:
    float distortion(float xn) override {
        if (xn < 0.f) { return 0.f; }
        return xn;
    }
};

class HardClip : public Distortion {
public:
    float distortion(float xn) override {
        if (xn > 1.f)  { return 1.f;  }
        if (xn < -1.f) { return -1.f; }
        return xn;
    }
};

class TanhShaper : public Distortion {
public:
    float distortion(float xn) override {
        return std::tanh(xn);
    }
};


class SoftClipper : public Distortion {
public:
    void setThreshold_linear(double threshold) {
        threshold_linear = threshold;
    }
    
    void setThreshold_dB(double threshold) {
        threshold_linear = basics::db2mag(threshold);
    }
    
    void setKnee_dB(double knee) {
        knee_linear = std::min((2*threshold_linear),
                               (1.0 / basics::db2mag(knee)) - 1.0);
        halfKnee = 0.5 * knee_linear;
    }
    
    void setKnee_linear(double knee) {
        knee_linear = std::min(2*threshold_linear, knee);
        halfKnee = 0.5 * knee_linear;
    }
    
    float distortion (float xn) override {
        float abs = std::fabs(xn);
        int sign = xn >= 0 ? 1 : -1;
        
        if (abs <= threshold_linear - halfKnee) {
            return xn;
        } else if (abs <= threshold_linear + halfKnee) {
            float excess = abs - (threshold_linear - halfKnee);
            return sign * (abs - (excess * excess) / (2 * knee_linear));
        } else {
            return sign * threshold_linear;
        }
    }
    
private:
    double threshold_linear;
    double knee_linear;
    double halfKnee;
};


class Omni427 : public Distortion {
public:
    float distortion (float xn) override {
        float yn = xn;
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
