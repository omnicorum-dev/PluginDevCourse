/*
  ==============================================================================

    Biquad.h
    Created: 21 Feb 2026 9:14:54pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once

#include "Basics.h"

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

class Biquad {
public:
    virtual ~Biquad() = default;
    
    virtual void updateCoeffs() = 0;
    
    Biquad() { reset(); }
    
    void prepare(float sampleRate, int blockSize) {
        fs = sampleRate;
        this->blockSize = blockSize;
        reset();
        updateCoeffs();
    }
    
    void reset() {
        f0 = 1000.0f;
        Q = 0.707f;
        A = 1.0f;
        w0 = 0; a = 0; a0 = 0; a1 = 0; a2 = 0;
        b0 = 1; b1 = 0; b2 = 0;
        wn = 0; wnm1 = 0; wnm2 = 0;
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
    
    float processSample(float xn) {
        wn = (1/b0) * (xn - (b1*wnm1) - (b2*wnm2));
        float yn = (a0*wn) + (a1*wnm1) + (a2*wnm2);
        
        wnm2 = wnm1;
        wnm1 = wn;
        
        return yn;
    }
    
protected:
    float fs, f0, Q, A;
    float w0, a, a0, a1, a2, b0, b1, b2;
    float wn, wnm1, wnm2;
    int blockSize;
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

class LWR : public Biquad {
public:
    void setFilterType(FilterType newFilterType) {
        filterType = newFilterType;
        updateCoeffs();
    }
    
    void updateCoeffs() override {
        double omega_c = M_PI * f0;
        double omega_c2 = omega_c*omega_c;
        double theta_c = omega_c / fs;
        double k = omega_c / tan(theta_c);
        double k2 = k*k;
        double delta = k2 + omega_c2 + (2*k*omega_c);
        
        switch (filterType) {
            case FilterType::LOWPASS:
                a0 = omega_c2/delta;
                a1 = 2 * a0;
                a2 = a0;
                b1 = (-2*k2 + 2*omega_c2) / delta;
                b2 = (-2*k*omega_c + k2 + omega_c2) / delta;
                break;
            case FilterType::HIGHPASS:
                a0 = k2/delta;
                a1 = -2 * a0;
                a2 = a0;
                b1 = (-2*k2 + 2*omega_c2) / delta;
                b2 = (-2*k*omega_c + k2 + omega_c2) / delta;
                break;
            default:
                reset();
                break;
        };
    }
private:
    FilterType filterType;
};
