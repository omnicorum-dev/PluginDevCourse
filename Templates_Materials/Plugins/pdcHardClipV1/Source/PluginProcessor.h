/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class PdcHardClipV1AudioProcessor  : public juce::AudioProcessor
{
public:
    
    // APVTS for automation/presets/state
    juce::AudioProcessorValueTreeState apvts;
    
    // =============================================================================
    // Parameter IDs and Setup
    // =============================================================================
    struct Params
    {
        // Format: {id, name, min, max, default, skew, stepSize, suffix}
        
        // EXAMPLE PARAMETER TYPES
        
        // Linear Float Parameter
        static constexpr const char* threshold_ID   = "threshold";
        static constexpr const char* threshold_Name = "Threshold";
        static constexpr float threshold_Min        = 0.0f;
        static constexpr float threshold_Max        = 1.5f;
        static constexpr float threshold_Default    = 1.0f;
        static constexpr float threshold_StepSize   = 0.001f;
        
        // Logarithmic Float Parameter
        /*
        static constexpr const char* freq_ID   = "freq";
        static constexpr const char* freq_Name = "Frequency";
        static constexpr float freq_Min        = 20.0f;
        static constexpr float freq_Max        = 20000.0f;
        static constexpr float freq_Default    = 1000.0f;
        static constexpr float freq_Skew       = 0.3f; // log scale
        static constexpr float freq_StepSize   = 1.0f;
        */
        
        // Exponential Float Parameter
        /*
        static constexpr const char* inGain_ID     = "inGain";
        static constexpr const char* inGain_Name   = "Input Gain";
        static constexpr float inGain_Min          = -60.0f;
        static constexpr float inGain_Max          = 12.0f;
        static constexpr float inGain_Default      = 0.0f;
        static constexpr float inGain_StepSize     = 0.1f;
        static constexpr float inGain_Skew         = 3.f;
        static constexpr const char* inGain_Suffix = " dB";
        */
        
        // Dropdown Menu
        /*
        static constexpr const char* filterType_ID   = "filterType";
        static constexpr const char* filterType_Name = "Filter Type";
        static constexpr int filterType_Default      = 0;
        // Choices defined in createParameterLayout()
        */
        
        // Linear Integer Parameter
        /*
        static constexpr const char* voices_ID   = "voices";
        static constexpr const char* voices_Name = "Voices";
        static constexpr int voices_Min          = 1;
        static constexpr int voices_Max          = 16;
        static constexpr int voices_Default      = 4;
        */
        
        // Tickbox/Boolean Parameter
        /*
        static constexpr const char* bypass_ID   = "bypass";
        static constexpr const char* bypass_Name = "Bypass";
        static constexpr bool bypass_Default     = false;
        */
        
        static constexpr const char* inGain_ID     = "inGain";
        static constexpr const char* inGain_Name   = "Input Gain";
        static constexpr float inGain_Min          = -60.0f;
        static constexpr float inGain_Max          = 12.0f;
        static constexpr float inGain_Default      = 0.0f;
        static constexpr float inGain_StepSize     = 0.1f;
        static constexpr float inGain_Skew         = 3.f;
        static constexpr const char* inGain_Suffix = " dB";
        
        static constexpr const char* outGain_ID     = "outGain";
        static constexpr const char* outGain_Name   = "Output Gain";
        static constexpr float outGain_Min          = -60.0f;
        static constexpr float outGain_Max          = 12.0f;
        static constexpr float outGain_Default      = 0.0f;
        static constexpr float outGain_StepSize     = 0.1f;
        static constexpr float outGain_Skew         = 3.f;
        static constexpr const char* outGain_Suffix = " dB";
        
        static constexpr const char* mix_ID         = "mix";
        static constexpr const char* mix_Name       = "Mix";
        static constexpr float mix_Min              = 0.0f;
        static constexpr float mix_Max              = 100.0f;
        static constexpr float mix_Default          = 100.0f;
        static constexpr float mix_StepSize         = 0.1f;
        static constexpr const char* mix_Suffix     = "%";
        
        static constexpr const char* bypass_ID   = "bypass";
        static constexpr const char* bypass_Name = "Bypass";
        static constexpr bool bypass_Default     = false;
    };
    

    // =============================================================================
    // Smoothed values for audio-rate parameters (change continuously)
    //     - These are generally your float-based parameters
    // =============================================================================

    juce::SmoothedValue<float> inGainSmooth;
    juce::SmoothedValue<float> outGainSmooth;
    juce::SmoothedValue<float> mixSmooth;
    
    juce::SmoothedValue<float> thresholdSmooth;

    // =============================================================================
    // Previous values for parameters that require recalculation when changed
    // =============================================================================

    // Examples:
    // float prevFreq = -1.0f;
    // int prevFilterType = -1;

   
    // =============================================================================
    // Direct access for control-rate parameters (change discretely)
    //     - These are generally boolean, integer, or dropdown parameters
    // =============================================================================

    std::atomic<float>* bypassParam = nullptr;
    // std::atomic<float>* filterTypeParam = nullptr;
    // std::atomic<float>* voicesParam = nullptr;
    
    
    
    
    
    //==============================================================================
    PdcHardClipV1AudioProcessor();
    ~PdcHardClipV1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // =============================================================================
    // Add any internal functions, objects, etc. here
    // =============================================================================

    // Example:
    // DelayLine<48000> DelayL, DelayR;


    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PdcHardClipV1AudioProcessor)
};
