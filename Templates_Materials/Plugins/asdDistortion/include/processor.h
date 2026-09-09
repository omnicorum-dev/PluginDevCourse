#pragma once

#include "distortion.h"
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
        SCFloat  mix_skew     = 3.f;

        // bypass (bool)
        SCString bypass_ID      = "bypass";
        SCString bypass_name    = "Bypass";
        SCBool   bypass_default = false;

        // drive (float)
        SCString drive_ID       = "drive";
        SCString drive_name     = "Drive";
        SCString drive_suffix   = "dB";
        SCFloat  drive_min      = -24.f;
        SCFloat  drive_max      = 24.f;
        SCFloat  drive_default  = 0.f;
        SCFloat  drive_stepSize = 0.1f;
        SCFloat  drive_skew     = Skew::linear;

        // bias (float)
        SCString bias_ID       = "bias";
        SCString bias_name     = "Bias";
        SCString bias_suffix   = "";
        SCFloat  bias_min      = -1.f;
        SCFloat  bias_max      = 1.f;
        SCFloat  bias_default  = 0.f;
        SCFloat  bias_stepSize = 0.01f;
        SCFloat  bias_skew     = Skew::linear;

        // dcBlock (bool)
        SCString dcblock_ID      = "dcblock";
        SCString dcblock_name    = "DC Block";
        SCBool   dcblock_default = true;

        // type (choice)
        SCString    type_ID      = "type";
        SCString    type_name    = "Distortion Type";
        SCStringArr type_choices = {
            "Hard Clip", "Tanh Shaper", "Full Rectifier", "Half Rectifier"};
        SCInt type_default = 0;
    };

    pFloat inGainSmooth;
    pFloat outGainSmooth;
    pFloat mixSmooth;

    pFloat driveSmooth;
    pFloat biasSmooth;
    pBool  dcBlockParam;
    pInt   typeParam;

    pBool bypassParam;

  private:
    static APVTS::ParameterLayout createParameterLayout();

    std::array<HardClip, 2>      hard_clip;
    std::array<TanhShaper, 2>    tanh_shaper;
    std::array<RectifierFull, 2> full_rectifier;
    std::array<RectifierHalf, 2> half_rectifier;
};
