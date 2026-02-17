/*
  =================================================================================================================

    JUCE STARTUP TEMPLATE
    Created: 17 Feb 2026 05:08:23pm
    Author:  Nico Russo

  =================================================================================================================
*/

/*
 =================================================================================================================
 HOW TO USE:
 1. Create a new JUCE Audio Plugin project using the Projucer
 2. Copy SNIPPET 1 into PluginProcessor.h
 3. Copy SNIPPETS 2-6 into PluginProcessor.cpp
 4. To add or remove parameters, follow instructions detailed below snippets
 =================================================================================================================
*/


// ================================================================================================================
//
// SNIPPET 1: COPY TO TOP OF public: AND private: SECTIONS OF PluginProcessor.h
//
// ================================================================================================================

// Add any #include's here above the class

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
        /*
        static constexpr const char* gain_ID   = "gain";
        static constexpr const char* gain_Name = "Gain";
        static constexpr float gain_Min        = 0.0f;
        static constexpr float gain_Max        = 1.0f;
        static constexpr float gain_Default    = 0.5f;
        static constexpr float gain_StepSize   = 0.01f;
        */
        
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

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // =============================================================================
    // Add any internal functions, objects, etc. here
    // =============================================================================

    // Example:
    // DelayLine<48000> DelayL, DelayR;




// ================================================================================================================
//
// SNIPPET 2: ADD MISSING ELEMENTS TO CONSTRUCTOR IN PluginProcessor.cpp
//     - Comma after )
//     - apvts function call
//     - control rate parameter pointers
//
// ================================================================================================================

YourPluginAudioProcessor::YourPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Get pointers to control-rate parameters for fast access
    bypassParam = apvts.getRawParameterValue(Params::bypass_ID);
}


// ================================================================================================================
//
// SNIPPET 3: DEFINE YOUR PARAMETER LAYOUT IN PluginProcessor.cpp
//     - Copy/Paste directly below desctructor
//     - Change "YourPlugin" to the name of your plugin
//
// ================================================================================================================

juce::AudioProcessorValueTreeState::ParameterLayout YourPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
   layout.add(std::make_unique<juce::AudioParameterFloat>(
       juce::ParameterID(Params::outGain_ID, 1),
       Params::outGain_Name,
       juce::NormalisableRange<float>(Params::outGain_Min, Params::outGain_Max, Params::outGain_StepSize, Params::outGain_Skew),
       Params::outGain_Default,
       Params::outGain_Suffix));
   
   layout.add(std::make_unique<juce::AudioParameterFloat>(
       juce::ParameterID(Params::inGain_ID, 1),
       Params::inGain_Name,
       juce::NormalisableRange<float>(Params::inGain_Min, Params::inGain_Max, Params::inGain_StepSize, Params::inGain_Skew),
       Params::inGain_Default,
       Params::inGain_Suffix));
   
   layout.add(std::make_unique<juce::AudioParameterFloat>(
       juce::ParameterID(Params::mix_ID, 1),
       Params::mix_Name,
       juce::NormalisableRange<float>(Params::mix_Min, Params::mix_Max, Params::mix_StepSize),
       Params::mix_Default,
       Params::mix_Suffix));
   
   layout.add(std::make_unique<juce::AudioParameterBool>(
       juce::ParameterID(Params::bypass_ID, 1),
       Params::bypass_Name,
       Params::bypass_Default));
    
    // MULTIPLE CHOICE EXAMPLE
    /*
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(Params::filterType_ID, 1),
        Params::filterType_Name,
        juce::StringArray{"Low Pass", "High Pass", "Band Pass", "Notch"},
        Params::filterType_Default));
    */
    
    // INTEGER EXAMPLE
    /*
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(Params::voices_ID, 1),
        Params::voices_Name,
        Params::voices_Min,
        Params::voices_Max,
        Params::voices_Default));
    */
    
    return layout;
}



// ================================================================================================================
//
// SNIPPET 4: prepareToPlay()
//     - Replace only the CONTENTS of the prepareToPlay() function
//     - Do not replace the declaration
//
// ================================================================================================================

void YourPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // =============================================================================
    // Initialize smoothers
    // =============================================================================

    outGainSmooth.reset(sampleRate, 0.05);
    inGainSmooth.reset(sampleRate, 0.05);
    mixSmooth.reset(sampleRate, 0.05);
    
    
    // =============================================================================
    // Set to current values
    // =============================================================================
    
    outGainSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::outGain_ID)->load());
    inGainSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::inGain_ID)->load());
    mixSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::mix_ID)->load());
    
    // =============================================================================
    // Prepare any objects with sample rate or other setup
    // =============================================================================
    
    // Example:
    // DelayL.prepare(sampleRate); DelayR.prepare(sampleRate);
    
}


// ================================================================================================================
//
// SNIPPET 5: processBlock()
//     - Replace only the CONTENTS of the processBlock() function
//     - Do not replace the declaration
//
// ================================================================================================================

void YourPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples             = buffer.getNumSamples();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // =============================================================================
    // Read control parameters
    // =============================================================================
    
    bool bypass = bypassParam->load() > 0.5f;
    
    // Multiple choice and integer examples:
    // int filterType = static_cast<int>(filterTypeParam->load());
    // int numVoices = static_cast<int>(voicesParam->load());
    
    // =============================================================================
    // True Bypass
    // =============================================================================
    
    if (bypass) return;
    
    // =============================================================================
    // Get raw parameter values
    // =============================================================================
    
    float rawOutGain = apvts.getRawParameterValue(Params::outGain_ID)->load();
    float rawinGain  = apvts.getRawParameterValue(Params::inGain_ID)->load();
    float rawMix     = apvts.getRawParameterValue(Params::mix_ID)->load();
    
    // =============================================================================
    // Update smoothed parameters
    // =============================================================================
    
    outGainSmooth.setTargetValue(rawOutGain);
    inGainSmooth.setTargetValue(rawinGain);
    mixSmooth.setTargetValue(rawMix);
    
    // =============================================================================
    // Update objects for discrete changes
    // =============================================================================

    /*
     EXAMPLE:
    if (filterType != prevFilterType) { prevFilterType = filterType; }
    updateCoefficients();
    */
    
    // =============================================================================
    // Process audio
    // =============================================================================
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float dry     = channelData[sample];
            float inGain  = std::pow(10.0f, inGainSmooth.getNextValue()  / 20.f);
            float outGain = std::pow(10.0f, outGainSmooth.getNextValue() / 20.f);
            float mix     = mixSmooth.getNextValue();
            
            float xn = dry * inGain;
            
            //==============================================================================
            // SAMPLE-BY-SAMPLE DSP GOES HERE
            //==============================================================================
            
            float yn = xn;
            
            //==============================================================================
            // MAKE SURE yn IS SET TO THE OUTPUT SAMPLE AT THIS POINT
            //==============================================================================
            
            float mixed = (yn*mix*0.01)+(dry*(100-mix)*0.01);
            
            channelData[sample] = mixed * outGain;
        }
    }
}

// ================================================================================================================
//
// SNIPPET 6: State Saving/Loading & Generic UI
//     - Replace only the CONTENTS of getStateInformation() and setStateInformation() functions
//     - Comment out the current line in createEditor() and add the line in this template instead
//
// ================================================================================================================

void YourPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void YourPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void YourPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}


/*
 =================================================================================================================
 HOW TO ADD A NEW PARAMETER - Example: "Drive"
 
 Step 1: Add to Params struct in PluginProcessor.h
 static constexpr const char* drive_ID   = "drive";
 static constexpr const char* drive_Name = "Drive";
 static constexpr float drive_Min        = 1.0f;
 static constexpr float drive_Max        = 10.0f;
 static constexpr float drive_Default    = 1.0f;
 static constexpr float drive_StepSize   = 0.01f;
 static constexpr float drive_Skew       = 0.3f; // optional for log scale
 static constexpr float drive_Suffix     = "dB"; // optional for suffix
 
 Step 2A: Add smoother to PluginProcessor.h (if it is a continuous parameter)
 juce::SmoothedValue<float> driveSmooth;
 
 Step 2B: Add atomic access to PluginProcessor.h (if it is a discrete parameter)
 std::atomic<float>* driveParam = nullptr;
 
 Step 3: Add to createParameterLayout() in PluginProcessor.cpp
    - I recommend just copying one of the layouts already in the template and modifying names
    - All values were set in Step 1, so you just need to link them here
 
 layout.add(std::make_unique<juce::AudioParameterFloat>(
     juce::ParameterID(Params::drive_ID, 1),
     Params::drive_Name,
     juce::NormalisableRange<float>(Params::drive_Min, Params::drive_Max, Params::drive_StepSize, Params::drive_Skew),
     Params::drive_Default,
     Params::drive_Suffix));
 
 Step 4A: Initialize in prepareToPlay() (if it is a continuous parameter)
 driveSmooth.reset(sampleRate, 0.05);
 driveSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::drive_ID)->load());
 
 Step 4B: Get pointer for fast access in PluginProcessor.cpp constructor
 driveParam = apvts.getRawParameterValue(Params::drive_ID);
 
 Step 5: Update in processBlock() (if it is a smoothed parameter)
 driveSmooth.setTargetValue(apvts.getRawParameterValue(Params::drive_ID)->load());
 
 Step 6: Use in audio loop
 - If smoothed:
     float drive = driveSmooth.getNextValue();
 - If it's a discrete control value
     float drive = driveParam->load();
 =================================================================================================================
*/

/*
 =================================================================================================================
 HOW TO REMOVE A PARAMETER
 
 1.  Delete from Params struct in PluginProcessor.h
 2.  Delete smoother or pointer from PluginProcessor.h
 3.  Delete from createParameterLayout() in PluginProcessor.cpp
 4A. Delete from constructor (if using pointer)
 4B. Delete from prepareToPlay() (if smoothed)
 5.  Delete from processBlock()
 =================================================================================================================
*/
