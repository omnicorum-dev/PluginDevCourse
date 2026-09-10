#pragma once

#include "oscillator.h"
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

        // freq (float)
        SCString freq_ID       = "freq";
        SCString freq_name     = "Frequency";
        SCString freq_suffix   = "Hz";
        SCFloat  freq_min      = 1.f;
        SCFloat  freq_max      = 20000.f;
        SCFloat  freq_default  = 100.f;
        SCFloat  freq_stepSize = 1.f;
        SCFloat  freq_skew     = Skew::logarithmic;

        // amp (float)
        SCString amp_ID       = "amp";
        SCString amp_name     = "Amplitude";
        SCString amp_suffix   = "";
        SCFloat  amp_min      = 0.f;
        SCFloat  amp_max      = 1.f;
        SCFloat  amp_default  = 0.f;
        SCFloat  amp_stepSize = 0.01f;
        SCFloat  amp_skew     = Skew::logarithmic;

        // bypass (bool)
        SCString bypass_ID      = "bypass";
        SCString bypass_name    = "Bypass";
        SCBool   bypass_default = false;
    };

    pFloat inGainSmooth;
    pFloat outGainSmooth;
    pFloat mixSmooth;

    pFloat freqSmooth;
    pFloat ampSmooth;

    pBool bypassParam;

  private:
    static APVTS::ParameterLayout createParameterLayout();

    std::array<Oscillator, 2> oscillator;
};
