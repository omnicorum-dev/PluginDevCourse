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
#include "Basics.h"
#include "ParameterLayout.h"

public:
    // APVTS for automation/presets/state
    juce::AudioProcessorValueTreeState apvts;
    
    // =============================================================================
    // Parameter Setup
    // =============================================================================
    struct Params
    {
        // inGain (float)
        SCString inGain_ID        = "inGain";
        SCString inGain_Name      = "Input Gain";
        SCFloat  inGain_Min       = -60.0f;
        SCFloat  inGain_Max       = 12.0f;
        SCFloat  inGain_Default   = 0.0f;
        SCFloat  inGain_StepSize  = 0.1f;
        SCFloat  inGain_Skew      = Skew::exponential;
        SCString inGain_Suffix    = " dB";
        
        // outGain (float)
        SCString outGain_ID       = "outGain";
        SCString outGain_Name     = "Output Gain";
        SCFloat  outGain_Min      = -60.0f;
        SCFloat  outGain_Max      = 12.0f;
        SCFloat  outGain_Default  = 0.0f;
        SCFloat  outGain_StepSize = 0.1f;
        SCFloat  outGain_Skew     = Skew::exponential;
        SCString outGain_Suffix   = " dB";
        
        // mix (float)
        SCString mix_ID           = "mix";
        SCString mix_Name         = "Mix";
        SCFloat  mix_Min          = 0.0f;
        SCFloat  mix_Max          = 100.0f;
        SCFloat  mix_Default      = 100.0f;
        SCFloat  mix_StepSize     = 0.1f;
        SCFloat  mix_Skew         = 3.f;
        SCString mix_Suffix       = "%";
        
        // bypass (bool)
        SCString bypass_ID        = "bypass";
        SCString bypass_Name      = "Bypass";
        SCBool   bypass_Default   = false;
        
        // example (int)
        /*
         SCString example_ID      = "example";
         SCString example_Name    = "Example";
         SCInt    example_Min     = 0;
         SCInt    example_Max     = 0;
         SCInt    example_Default = 0;
         SCString example_Suffix  = "";
         */
        
        // example (choice)
        /*
         SCString example_ID      = "example";
         SCString example_Name    = "Example";
         juce::StringArray example_Choices = {
             "Choice A", // 0
             "Choice B", // 1
             "Choice C", // 2
         };
         SCInt    example_Default = 0;
         */
        
    };

    // =============================================================================
    // Parameters for audio-rate parameters (change continuously)
    //     - These are generally your float-based parameters
    // =============================================================================

    pdcFloat inGainSmooth;
    pdcFloat outGainSmooth;
    pdcFloat mixSmooth;
   
    // =============================================================================
    // Parameters for control-rate parameters (change discretely)
    //     - These are generally boolean, integer, or choice parameters
    // =============================================================================

    pdcBool bypassParam;

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
    
    addFloat(layout,
             Params::inGain_ID,
             Params::inGain_Name,
             Params::inGain_Min,
             Params::inGain_Max,
             Params::inGain_Default,
             Params::inGain_StepSize,
             Params::inGain_Skew,
             Params::inGain_Suffix);
    
    addFloat(layout,
             Params::outGain_ID,
             Params::outGain_Name,
             Params::outGain_Min,
             Params::outGain_Max,
             Params::outGain_Default,
             Params::outGain_StepSize,
             Params::outGain_Skew,
             Params::outGain_Suffix);
    
    addFloat(layout,
             Params::mix_ID,
             Params::mix_Name,
             Params::mix_Min,
             Params::mix_Max,
             Params::mix_Default,
             Params::mix_StepSize,
             Params::mix_Skew,
             Params::mix_Suffix);
    
    addBool(layout,
            Params::bypass_ID,
            Params::bypass_Name,
            Params::bypass_Default);
    
    // use addInt and addChoice for ints and choices
    
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
    // Prepare smoothers
    // =============================================================================
    
    outGainSmooth.prepare(sampleRate, samplesPerBlock, &apvts, Params::outGain_ID);
    inGainSmooth.prepare(sampleRate, samplesPerBlock, &apvts, Params::inGain_ID);
    mixSmooth.prepare(sampleRate, samplesPerBlock, &apvts, Params::mix_ID);
    
    bypassParam.prepare(sampleRate, samplesPerBlock, &apvts, Params::bypass_ID);
    
    // =============================================================================
    // Prepare any objects here
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
    
    bool bypass = bypassParam.getNextValue();
    
    // =============================================================================
    // True Bypass
    // =============================================================================
    
    if (bypass) return;
    
    // =============================================================================
    // Update smoothers
    // =============================================================================
    
    outGainSmooth.update();
    inGainSmooth.update();
    mixSmooth.update();
    
    // =============================================================================
    // Update objects for discrete changes
    // =============================================================================

    /*
     EXAMPLE:
     if (filterTypeParam.changed()) {
        updateCoefficients();
     }
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
            float inGain  = std::pow(10.0f, inGainSmooth.getNextValue() / 20.f);
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

void YourPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

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


/*
 =================================================================================================================
 HOW TO ADD A NEW PARAMETER
 
 Step 1: Add to Params struct in PluginProcessor.h
 See examples
 
 Step 2: Add parameter to PluginProcessor.h
 pdcFloat, pdcInt, pdcBool, pdcChoice
 
 Step 3: Add to createParameterLayout() in PluginProcessor.cpp
 addFloat(...), addInt(...), addBool(...), addChoice(...)
 
 Step 4: Prepare in prepareToPlay()
 exampleSmooth.reset(sampleRate, samplesPerBlock, &apvts, Params::example_ID);
 
 Step 5: Update in processBlock() with getNextValue()
 should be done ONCE PER BLOCK for discrete controls
 should be done ONCE PER SAMPLE (or with a skip amount if anything else)
 (you can use getCurrentValue() after you've called getNextValue())
 
 Step 6: Use in audio loop!
 =================================================================================================================
*/

/*
 =================================================================================================================
 HOW TO REMOVE A PARAMETER
 
 1.  Delete from Params struct in PluginProcessor.h
 2.  Delete param from PluginProcessor.h
 3.  Delete from createParameterLayout() in PluginProcessor.cpp
 4.  Delete from prepareToPlay()
 5.  Delete from processBlock()
 =================================================================================================================
*/
