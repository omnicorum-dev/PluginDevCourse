/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PdcHardClipV2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PdcHardClipV2AudioProcessorEditor (PdcHardClipV2AudioProcessor&);
    ~PdcHardClipV2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PdcHardClipV2AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PdcHardClipV2AudioProcessorEditor)
};
