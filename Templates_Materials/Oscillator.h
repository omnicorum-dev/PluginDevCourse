/*
  ==============================================================================

    Oscillator.h
    Created: 23 Feb 2026 3:03:42pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

enum class Waveform {
    Sine,
    Pulse,
    Saw
};

enum class Polarity {
    Unipolar,
    Bipolar
};


class Oscillator {
public:
    
    void prepare(float _sampleRate, int _samplesPerBlock) {
        fs = _sampleRate;
        samplesPerBlock = _samplesPerBlock;
        reset();
    }
    
    void reset() {
        if (fs > 0.0f)
            phaseIncrement = frequency / fs;
        phase = 0.0;
    }
    
    void setFrequency(double hz) {
        frequency = hz;
        if (fs > 0.0f)
            phaseIncrement = frequency / fs;
    }
    
    void setWaveform(Waveform w)       { waveform = w; }
    void setPolarity(Polarity p)       { polarity = p; }
    void setBLEP(bool on)              { useBLEP = on; }
    
    /*
     * Pulse: Duty Cycle/Pulse Width (0.5 = Square)
     * Saw:   Ramp up % (0.5 = Triangle)
     */
    void setWidth(double w)            { width = std::clamp(w, 0.001, 0.999); }
    
    /* Phase offset 0-1 */
    void setPhaseOffset(double offset) { phaseOffset = offset; }
    
    /* Advance phase and return next sample of osc */
    double processSample() {
        double sample = generateSample(phase);
        advancePhase();
        return applyPolarity(sample);
    }
    
    double getPhase()     { return phase; }
    double getFrequency() { return frequency; }
    
protected:
    
    double generateSample(double p) const {
        // apply phase offset
        double ph = std::fmod(p + phaseOffset, 1.0);
        if (ph < 0.0) ph += 1.0;
        
        double t = ph;
        
        switch (waveform) {
            case Waveform::Sine:
                return std::sin(ph * M_PI * 2);
                
            case Waveform::Pulse: {
                double sample = (ph < width) ? 1.0 : -1.0;
                if (useBLEP) {
                    sample += polyBLEP(t);
                    sample -= polyBLEP(std::fmod(t - width + 1.0, 1.0));
                }
                return sample;
            }
                
            case Waveform::Saw: {
                double sample = generateSaw(ph);
                if (useBLEP) {
                    sample -= polyBLEP(t);                          // wrap discontinuity at 0
                    sample += polyBLEP(fmod(t - width + 1.0, 1.0)); // kink at width
                }
                return sample;
            }
                
            default:
                return 0.0;
        }
    }
    
    double polyBLEP(double t) const {
        double dt = phaseIncrement;
        // 0 <= t < 1
        if (t < dt) {
            t /= dt;
            return t + t - t*t - 1.0;
        }
        // -1 < t < 0
        else if (t > 1.0 - dt) {
            t = (t - 1.0) / dt;
            return t*t + t + t + 1.0;
        }
        // 0 otherwise
        else return 0.0;
    }
    
    double generateSaw(double ph) const {
        if (ph < width) {
            // rising
            return -1.0 + 2.0 * (ph / width);
        } else {
            // falling
            return 1.0 - 2.0 * ((ph - width) / (1.0 - width));
        }
    }
    
    double applyPolarity(double sample) const {
        if (polarity == Polarity::Unipolar)
            return (sample + 1.0) * 0.5;
        return sample;
    }
    
    void advancePhase() {
        phase += phaseIncrement;
        if (phase >= 1.0) phase -= 1.0;
    }
    
    double phaseIncrement = 0.;
    double phase = 0.;
    double width = 0.5;
    double phaseOffset = 0.;
    double frequency = 10.;
    
    bool useBLEP = false;
    
    Waveform waveform = Waveform::Sine;
    Polarity polarity = Polarity::Bipolar;
    
    float fs = 0.f;
    int samplesPerBlock = 512;
};
