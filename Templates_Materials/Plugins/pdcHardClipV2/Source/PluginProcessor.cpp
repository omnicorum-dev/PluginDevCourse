/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PdcHardClipV2AudioProcessor::PdcHardClipV2AudioProcessor()
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

PdcHardClipV2AudioProcessor::~PdcHardClipV2AudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PdcHardClipV2AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(Params::threshold_ID, 1),
        Params::threshold_Name,
        juce::NormalisableRange<float>(Params::threshold_Min, Params::threshold_Max, Params::threshold_StepSize),
        Params::threshold_Default));
    
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

//==============================================================================
const juce::String PdcHardClipV2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PdcHardClipV2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PdcHardClipV2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PdcHardClipV2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PdcHardClipV2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PdcHardClipV2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PdcHardClipV2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void PdcHardClipV2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PdcHardClipV2AudioProcessor::getProgramName (int index)
{
    return {};
}

void PdcHardClipV2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PdcHardClipV2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // =============================================================================
    // Initialize smoothers
    // =============================================================================

    outGainSmooth.reset(sampleRate, 0.05);
    inGainSmooth.reset(sampleRate, 0.05);
    mixSmooth.reset(sampleRate, 0.05);
    
    thresholdSmooth.reset(sampleRate, 0.05);
    
    
    // =============================================================================
    // Set to current values
    // =============================================================================
    
    outGainSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::outGain_ID)->load());
    inGainSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::inGain_ID)->load());
    mixSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::mix_ID)->load());
    
    thresholdSmooth.setCurrentAndTargetValue(apvts.getRawParameterValue(Params::threshold_ID)->load());
    
    // =============================================================================
    // Prepare any objects with sample rate or other setup
    // =============================================================================
    
    // Example:
    // DelayL.prepare(sampleRate); DelayR.prepare(sampleRate);
    
    clipper.prepare(sampleRate, samplesPerBlock);
}

void PdcHardClipV2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PdcHardClipV2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PdcHardClipV2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    float rawThreshold = apvts.getRawParameterValue(Params::threshold_ID)->load();
    
    // =============================================================================
    // Update smoothed parameters
    // =============================================================================
    
    outGainSmooth.setTargetValue(rawOutGain);
    inGainSmooth.setTargetValue(rawinGain);
    mixSmooth.setTargetValue(rawMix);
    
    thresholdSmooth.setTargetValue(rawThreshold);
    
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
            
            float threshold = thresholdSmooth.getNextValue();
            
            clipper.setThreshold(threshold);
            yn = clipper.process(xn);
            
            //==============================================================================
            // MAKE SURE yn IS SET TO THE OUTPUT SAMPLE AT THIS POINT
            //==============================================================================
            
            float mixed = (yn*mix*0.01)+(dry*(100-mix)*0.01);
            
            channelData[sample] = mixed * outGain;
        }
    }
}

//==============================================================================
bool PdcHardClipV2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PdcHardClipV2AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    //return new PdcHardClipV2AudioProcessorEditor (*this);
}

//==============================================================================
void PdcHardClipV2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PdcHardClipV2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PdcHardClipV2AudioProcessor();
}
