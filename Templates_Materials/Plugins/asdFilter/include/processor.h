#pragma once

#include "filter.h"
#include "parameter_layout.h"
#include <juce_audio_processors/juce_audio_processors.h>

class Processor final : public juce::AudioProcessor {
  public:
    /* ======================================================== */

    // Constructor and destructor
    Processor();
    ~Processor() override;

    /* ======================================================== */

    const juce::String getName() const override { return JucePlugin_Name; }
    // juce::StringArray getAlternateDisplayNames() const override;

    void prepareToPlay(double sampleRate, int expectedBlockSize) override;

    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer         &messages) override;

    double getTailLengthSeconds() const override { return 0; }

    bool hasEditor() const override { return true; }

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    /* ======================================================== */

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }

    int                getNumPrograms() override { return 1; }
    int                getCurrentProgram() override { return 0; }
    void               setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void               changeProgramName(int, const juce::String &) override {}

    /* ======================================================== */

  private:
    // If you want a custom editor, remove the generic editor and write the
    // definition in the cpp. It should instead return your editor.
    juce::AudioProcessorEditor *createEditor() override {
        return new juce::GenericAudioProcessorEditor(*this);
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)

    /* ======================================================== */

  public:
    APVTS apvts;

    struct Params {
        // inGain (float)
        SCString inGain_ID       = "inGain";
        SCString inGain_name     = "Input Gain";
        SCString inGain_suffix   = " dB";
        SCFloat  inGain_min      = -60.f;
        SCFloat  inGain_max      = 12.f;
        SCFloat  inGain_default  = 0.f;
        SCFloat  inGain_stepSize = 0.1f;
        SCFloat  inGain_skew     = Skew::exponential;

        // outGain (float)
        SCString outGain_ID       = "outGain";
        SCString outGain_name     = "Output Gain";
        SCString outGain_suffix   = " dB";
        SCFloat  outGain_min      = -60.f;
        SCFloat  outGain_max      = 12.f;
        SCFloat  outGain_default  = 0.f;
        SCFloat  outGain_stepSize = 0.1f;
        SCFloat  outGain_skew     = Skew::exponential;

        // mix (float)
        SCString mix_ID       = "mix";
        SCString mix_name     = "Mix";
        SCString mix_suffix   = "%";
        SCFloat  mix_min      = 0.f;
        SCFloat  mix_max      = 100.f;
        SCFloat  mix_default  = 100.f;
        SCFloat  mix_stepSize = 0.1f;
        SCFloat  mix_skew     = Skew::linear;

        // bypass (bool)
        SCString bypass_ID      = "bypass";
        SCString bypass_name    = "Bypass";
        SCBool   bypass_default = false;

        // frequency (float)
        SCString freq_ID       = "freq";
        SCString freq_name     = "Frequency";
        SCString freq_suffix   = "Hz";
        SCFloat  freq_min      = 20.f;
        SCFloat  freq_max      = 20000.f;
        SCFloat  freq_default  = 1000.f;
        SCFloat  freq_stepSize = 1.f;
        SCFloat  freq_skew     = Skew::logarithmic;

        // Q (float)
        SCString q_ID       = "q";
        SCString q_name     = "Q";
        SCString q_suffix   = "";
        SCFloat  q_min      = 0.1f;
        SCFloat  q_max      = 10.f;
        SCFloat  q_default  = 0.7071f;
        SCFloat  q_stepSize = 0.01f;
        SCFloat  q_skew     = Skew::logarithmic;

        // gain (float dB)
        SCString gain_ID       = "gain";
        SCString gain_name     = "Gain";
        SCString gain_suffix   = "dB";
        SCFloat  gain_min      = -24.f;
        SCFloat  gain_max      = 24.f;
        SCFloat  gain_default  = 0.f;
        SCFloat  gain_stepSize = 0.1f;
        SCFloat  gain_skew     = Skew::linear;

        // filter type (choide)
        SCString    filter_ID      = "filter";
        SCString    filter_name    = "Filter Kind";
        SCStringArr filter_choices = {
            "Lowpass",
            "Highpass",
            "Bandpass Skirt",
            "Bandpass Peak",
            "Notch",
            "Bell",
            "High Shelf",
            "Low Shelf",
            "All-pass",
        };
        SCInt filter_default = 0;

        // type (choice)
        SCString    type_ID      = "type";
        SCString    type_name    = "Filter Type";
        SCStringArr type_choices = {
            "Robert Bristow-Johnston",
            "High Order Filter",
            "Linkwitz-Riley",
        };
        SCInt type_default = 0;

        // stages (int)
        SCString stages_ID      = "stages";
        SCString stages_name    = "Filter Stages";
        SCString stages_suffix  = "";
        SCInt    stages_min     = 1;
        SCInt    stages_max     = 8;
        SCInt    stages_default = 1;
    };

    pFloat inGainSmooth;
    pFloat outGainSmooth;
    pFloat mixSmooth;

    pFloat freqSmooth;
    pFloat qSmooth;
    pFloat gainSmooth;

    pInt typeParam;
    pInt filterParam;
    pInt stagesParam;

    pBool bypassParam;

  private:
    static APVTS::ParameterLayout createParameterLayout();

    std::array<RBJ, 2>                rbj;
    std::array<LR4, 2>                lr4;
    std::array<HighOrderFilter<8>, 2> highOrder;
};
