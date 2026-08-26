/*
  ==============================================================================

    Biquad.h
    Created: 11 Mar 2026 1:11:12pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"
#include "MonoEffect.h"

enum class FilterType {
    LOWPASS = 0,
    HIGHPASS,
    BANDPASS,
    NOTCH,
    PEAKING,
    HIGHSHELF,
    LOWSHELF,
    ALLPASS
};

class Biquad : public MonoEffect {
public:
    virtual void updateCoeffs() {};
    
    Biquad() { reset(); }
    
    void reset() {
        f0 = 1000.0f;
        Q = 0.7071f;
        A = 1.0f;
        w0 = 0; a = 0; a0 = 0; a1 = 0; a2 = 0;
        b0 = 1; b1 = 0; b2 = 0;
        xnm1 = 0; xnm2 = 0; ynm1 = 0; ynm2 = 0;
    }
    
    void setFreq(float newFreq) {
        f0 = newFreq;
        updateCoeffs();
    }
    void setQ(float newQ) {
        Q = newQ;
        updateCoeffs();
    }
    void setA(float newA) {
        A = newA;
        updateCoeffs();
    }
    void setAll(float newFreq, float newQ, float newA = 1.0f) {
        f0 = newFreq; Q = newQ; A = newA;
        updateCoeffs();
    }
    
    float processSample(float xn) override {
        float yn = (a0/b0)*xn + (a1/b0)*xnm1 + (a2/b0)*xnm2
                              - (b1/b0)*ynm1 - (b2/b0)*ynm2;
        xnm2 = xnm1;
        xnm1 = xn;
        ynm2 = ynm1;
        ynm1 = yn;
        return yn;
    }
    
protected:
    float f0, Q, A;
    float w0, a, a0, a1, a2, b0, b1, b2;
    float xnm1, xnm2, ynm1, ynm2;
};

class RBJ : public Biquad {
public:
    void setFilterType(FilterType newFilterType) {
        filterType = newFilterType;
        updateCoeffs();
    }
    
    void updateCoeffs() override {
        w0 = (2*M_PI*f0)/fs;
        a  = sin(w0)/(2*Q);
        
        float cosw0 = cos(w0);
        
        switch (filterType) {
            case FilterType::LOWPASS:
                a0 = (1-cosw0)/2;
                a1 = 1-cosw0;
                a2 = a0;
                b0 = 1+a;
                b1 = -2*cosw0;
                b2 = 1-a;
                break;
            case FilterType::HIGHPASS:
                a0 = (1+cosw0)/2;
                a1 = -1 * (1+cosw0);
                a2 = a0;
                b0 = 1+a;
                b1 = -2*cosw0;
                b2 = 1-a;
                break;
            case FilterType::BANDPASS:
                a0 = A * a;
                a1 = 0;
                a2 = -A * a;
                b0 = 1 + a;
                b1 = -2 * cosw0;
                b2 = 1 - a;
                break;
            case FilterType::NOTCH:
                a0 = 1;
                a1 = -2 * cosw0;
                a2 = 1;
                b0 = 1 + a;
                b1 = a1;
                b2 = 1 - a;
                break;
            case FilterType::PEAKING:
                a0 = 1 + A * a;
                a1 = -2 * cosw0;
                a2 = 1 - A * a;
                b0 = 1 + (a/A);
                b1 = a1;
                b2 = 1 - (a/A);
                break;
            case FilterType::HIGHSHELF:
                a0 = A * ((A+1)+(A-1) * cosw0 + 2*sqrt(A)*a);
                a1 = -2*A*((A-1)+(A+1)*cosw0);
                a2 = A * ((A+1)+(A-1) * cosw0 - 2*sqrt(A)*a);
                b0 = (A+1) - (A-1) * cosw0 + 2*sqrt(A)*a;
                b1 = 2 * ((A-1)-(A+1)*cosw0);
                b2 = (A+1) - (A-1) * cosw0 - 2*sqrt(A)*a;
                break;
            case FilterType::LOWSHELF:
                a0 = A * ((A+1)-(A-1) * cosw0 + 2*sqrt(A)*a);
                a1 = 2*A*((A-1)-(A+1)*cosw0);
                a2 = A * ((A+1)-(A-1) * cosw0 - 2*sqrt(A)*a);
                b0 = (A+1) + (A-1) * cosw0 + 2*sqrt(A)*a;
                b1 = -2 * ((A-1)+(A+1)*cosw0);
                b2 = (A+1) + (A-1) * cosw0 - 2*sqrt(A)*a;
                break;
            case FilterType::ALLPASS:
                a0 = 1-a;
                a1 = -2*cosw0;
                a2 = 1+a;
                b0 = a2;
                b1 = a1;
                b2 = a0;
                break;
            default:
                a0 = (1-cosw0)/2;
                a1 = 1-cosw0;
                a2 = a0;
                b0 = 1+a;
                b1 = -2*cosw0;
                b2 = 1-a;
                break;
        }
    }
private:
    FilterType filterType = FilterType::LOWPASS;
};



class LR4 : public MonoEffect {
public:
    
    void prepare(float sampleRate, int blockSize) override {
        stage1.prepare(sampleRate, blockSize);
        stage2.prepare(sampleRate, blockSize);
        
        stage1.setQ(0.7071);
        stage2.setQ(0.7071);
    }
    
    void setFilterType(FilterType newFilterType) {
        stage1.setFilterType(newFilterType);
        stage2.setFilterType(newFilterType);
    }
    
    void setFreq(float newFreq) {
        stage1.setFreq(newFreq);
        stage2.setFreq(newFreq);
    }
    
    float processSample(float xn) override {
        float wn = stage1.processSample(xn);
        return stage2.processSample(wn);
    }

private:
    RBJ stage1, stage2;
};
