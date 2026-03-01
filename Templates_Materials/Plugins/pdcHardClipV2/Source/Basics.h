/*
  ==============================================================================
    Basics.h
    
    Basic useful functions for use in DSP
    
    Includes direct C++ equivalents to common MATLAB DSP functions.
    All functions are inline for zero overhead.
    
    Usage: #include "Basics.h" in your PluginProcessor.h or .cpp
  ==============================================================================
*/

#pragma once
#include <vector>

#ifndef M_PI
#define M_PI   3.14159265358979323846
#define M_PI_2 6.283185307179586
#endif

namespace basics
{

// =============================================================================
// DECIBEL CONVERSIONS (MATLAB Signal Processing Toolbox)
// =============================================================================

/**
 * Convert decibels to magnitude (voltage/amplitude gain)
 * MATLAB equivalent: db2mag(dB)
 * Formula: mag = 10^(dB/20)
 */
inline float db2mag(float dB)
{
    return std::pow(10.0f, dB / 20.0f);
}

/**
 * Convert magnitude to decibels
 * MATLAB equivalent: mag2db(mag)
 * Formula: dB = 20*log10(mag)
 */
inline float mag2db(float mag)
{
    return 20.0f * std::log10(mag);
}

/**
 * Convert decibels to power
 * MATLAB equivalent: db2pow(dB)
 * Formula: pow = 10^(dB/10)
 */
inline float db2pow(float dB)
{
    return std::pow(10.0f, dB / 10.0f);
}

/**
 * Convert power to decibels
 * MATLAB equivalent: pow2db(pow)
 * Formula: dB = 10*log10(pow)
 */
inline float pow2db(float pow)
{
    return 10.0f * std::log10(pow);
}


// =============================================================================
// SIGNAL OPERATIONS
// =============================================================================

/**
 * Absolute value
 * MATLAB equivalent: abs(x)
 */
inline float abs(float x)
{
    return std::abs(x);
}

/**
 * Sign function
 * MATLAB equivalent: sign(x)
 * Returns: -1 for negative, 0 for zero, +1 for positive
 */
inline float sign(float x)
{
    if (x > 0.0f) return 1.0f;
    if (x < 0.0f) return -1.0f;
    return 0.0f;
}

/**
 * Clip/saturate signal
 * MATLAB equivalent: max(min(x, upper), lower)
 */
inline float clip(float x, float lower, float upper)
{
    return std::clamp(x, lower, upper);
}

/**
 * Root mean square
 * MATLAB equivalent: rms(x)
 */
inline float rms(const float* data, int numSamples)
{
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        sum += data[i] * data[i];
    return std::sqrt(sum / numSamples);
}

/**
 * Peak (maximum absolute value)
 * MATLAB equivalent: max(abs(x))
 */
inline float peak(const float* data, int numSamples)
{
    float maxVal = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        maxVal = std::max(maxVal, std::abs(data[i]));
    return maxVal;
}


// =============================================================================
// NONLINEAR FUNCTIONS / WAVESHAPING
// =============================================================================

/**
 * Soft clipping with adjustable knee
 * MATLAB equivalent: Custom sigmoid function
 */
inline float sigmoid(float x, float gain = 1.0f)
{
    return x / (1.0f + std::abs(gain * x));
}

/**
 * Cubic soft clipping
 * MATLAB equivalent: Custom cubic waveshaper
 */
inline float cubicClip(float x)
{
    if (std::abs(x) > 1.0f)
        return sign(x);
    return x - (x * x * x) / 3.0f;
}

/**
 * Exponential waveshaper
 * MATLAB equivalent: (exp(gain*x) - exp(-gain*x)) / (exp(gain*x) + exp(-gain*x))
 */
inline float expoShaper(float x, float gain = 1.0f)
{
    float expPos = std::exp(gain * x);
    float expNeg = std::exp(-gain * x);
    return (expPos - expNeg) / (expPos + expNeg);
}


// =============================================================================
// INTERPOLATION
// =============================================================================

/**
 * Linear interpolation
 * MATLAB equivalent: interp1(x, 'linear')
 * blend: 0.0 = a, 1.0 = b
 */
inline float lerp(float a, float b, float frac)
{
    return a + frac * (b - a);
}

/**
 * Cubic interpolation (Hermite)
 * MATLAB equivalent: interp1(x, 'cubic')
 * For sample delay lines, wavetables, etc.
 */
inline float cubicInterp(float ym1, float y0, float y1, float y2, float frac)
{
    float c0 = y0;
    float c1 = 0.5f * (y1 - ym1);
    float c2 = ym1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
    float c3 = 0.5f * (y2 - ym1) + 1.5f * (y0 - y1);
    
    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}


// =============================================================================
// WINDOW FUNCTIONS (MATLAB Signal Processing Toolbox)
// =============================================================================

/**
 * Hann window - single sample
 * Usage: float sample = hann(n, N)
 * n: sample index (0 to N-1), N: window length
 */
inline float hann(int n, int N)
{
    return 0.5f * (1.0f - std::cos(2.0f * M_PI * n / (N - 1)));
}

/**
 * Hann window - full vector
 * MATLAB equivalent: w = hann(N)
 * Usage: std::vector<float> w = hann(512)
 */
inline std::vector<float> hann(int N)
{
    std::vector<float> window(N);
    for (int n = 0; n < N; ++n)
        window[n] = hann(n, N);
    return window;
}

/**
 * Hamming window - single sample
 */
inline float hamming(int n, int N)
{
    return 0.54f - 0.46f * std::cos(2.0f * M_PI * n / (N - 1));
}

/**
 * Hamming window - full vector
 * MATLAB equivalent: w = hamming(N)
 */
inline std::vector<float> hamming(int N)
{
    std::vector<float> window(N);
    for (int n = 0; n < N; ++n)
        window[n] = hamming(n, N);
    return window;
}

/**
 * Blackman window - single sample
 */
inline float blackman(int n, int N)
{
    float a0 = 0.42f;
    float a1 = 0.5f;
    float a2 = 0.08f;
    float arg = 2.0f * M_PI * n / (N - 1);
    return a0 - a1 * std::cos(arg) + a2 * std::cos(2.0f * arg);
}

/**
 * Blackman window - full vector
 * MATLAB equivalent: w = blackman(N)
 */
inline std::vector<float> blackman(int N)
{
    std::vector<float> window(N);
    for (int n = 0; n < N; ++n)
        window[n] = blackman(n, N);
    return window;
}

/**
 * Bartlett (triangular) window - single sample
 */
inline float bartlett(int n, int N)
{
    return 1.0f - std::abs(2.0f * n / (N - 1) - 1.0f);
}

/**
 * Bartlett window - full vector
 * MATLAB equivalent: w = bartlett(N)
 */
inline std::vector<float> bartlett(int N)
{
    std::vector<float> window(N);
    for (int n = 0; n < N; ++n)
        window[n] = bartlett(n, N);
    return window;
}


// =============================================================================
// FREQUENCY CONVERSIONS
// =============================================================================

/**
 * Convert frequency to MIDI note number
 * MATLAB equivalent: 69 + 12*log2(freq/440)
 */
inline float freq2midi(float freq)
{
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

/**
 * Convert MIDI note number to frequency
 * MATLAB equivalent: 440 * 2^((midi-69)/12)
 */
inline float midi2freq(float midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

/**
 * Convert frequency to angular frequency (radians/sample)
 * MATLAB equivalent: 2*pi*freq/sampleRate
 */
inline float freq2omega(float freq, float sampleRate)
{
    return 2.0f * M_PI * freq / sampleRate;
}

/**
 * Convert angular frequency to frequency (Hz)
 * MATLAB equivalent: omega*sampleRate/(2*pi)
 */
inline float omega2freq(float omega, float sampleRate)
{
    return omega * sampleRate / (2.0f * M_PI);
}


// =============================================================================
// FILTER COEFFICIENTS (MATLAB Signal Processing Toolbox)
// =============================================================================

/**
 * Calculate Q from bandwidth
 * MATLAB equivalent: Custom, used in filter design
 * centerFreq: Hz, bandwidth: Hz
 */
inline float bw2Q(float centerFreq, float bandwidth)
{
    return centerFreq / bandwidth;
}

/**
 * Calculate bandwidth from Q
 * MATLAB equivalent: Custom, used in filter design
 */
inline float Q2bw(float centerFreq, float Q)
{
    return centerFreq / Q;
}



// =============================================================================
// UTILITIES
// =============================================================================

/**
 * Convert samples to milliseconds
 * MATLAB equivalent: samples / sampleRate * 1000
 */
inline float samples2ms(float samples, float sampleRate)
{
    return (samples / sampleRate) * 1000.0f;
}

/**
 * Convert milliseconds to samples
 * MATLAB equivalent: ms * sampleRate / 1000
 */
inline float ms2samples(float ms, float sampleRate)
{
    return (ms / 1000.0f) * sampleRate;
}

/**
 * Normalize array to range [-1, 1]
 * MATLAB equivalent: x / max(abs(x))
 */
inline void normalize(float* data, int numSamples)
{
    float maxVal = peak(data, numSamples);
    if (maxVal > 0.0f)
    {
        for (int i = 0; i < numSamples; ++i)
            data[i] /= maxVal;
    }
}

inline float bipolar2uni(float xn) {
    return 0.5f*xn + 0.5f;
}
inline double bipolar2uni(double xn) {
    return 0.5*xn + 0.5;
}

inline float unipolar2bi(float xn) {
    return 2.0f*xn - 1.0f;
}
inline double unipolar2bi(double xn) {
    return 2.0*xn - 1.0;
}

inline float doUnipolarModulationFromMin (float modulator, float min, float max) {
    return modulator * (max - min) + min;
}
inline double doUnipolarModulationFromMin (double modulator, double min, double max) {
    return modulator * (max - min) + min;
}

inline float doUnipolarModulationFromMax (float modulator, float max, float min) {
    return max - (1.0f - modulator) * (max - min);
}
inline double doUnipolarModulationFromMax (double modulator, double max, double min) {
    return max - (1.0 - modulator) * (max - min);
}

inline float doBipolarModulationMinMax (float modulator, float min, float max) {
    float halfRange = (max - min) * 0.5f;
    float midpoint = halfRange + min;
    return modulator * halfRange + midpoint;
}
inline double doBipolarModulationMinMax (double modulator, double min, double max) {
    double halfRange = (max - min) * 0.5;
    double midpoint = halfRange + min;
    return modulator * halfRange + midpoint;
}
inline float doBipolarModulationCenterRange (float modulator, float center, float range) {
    float halfRange = range * 0.5f;
    return modulator * halfRange + center;
}
inline double doBipolarModulationCenterRange (double modulator, double center, double range) {
    double halfRange = range * 0.5;
    return modulator * halfRange + center;
}

/**
 * DC blocker (remove DC offset)
 * MATLAB equivalent: y = filter([1 -1], [1 -0.995], x)
 */
class DCBlocker
{
public:
    float process(float input)
    {
        float output = input - x1 + 0.995f * y1;
        x1 = input;
        y1 = output;
        return output;
    }
    
    void reset() { x1 = 0.0f; y1 = 0.0f; }
    
private:
    float x1 = 0.0f;
    float y1 = 0.0f;
};

/**
 * Denormal prevention
 * MATLAB equivalent: N/A (C++ specific issue)
 */
inline float addDenormalNoise(float value)
{
    static constexpr float dc = 1.0e-25f;
    return value + dc;
}

/**
 * Fast approximation of pow(2, x)
 * MATLAB equivalent: 2^x (but faster)
 * Good for exponential envelopes, frequency calculations
 */
inline float fastPow2(float x)
{
    // Fast approximation, not bit-exact with MATLAB but close
    return std::exp2(x);
}

} // namespace basics
