#pragma once

#include "delay_line.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "parameter_layout.h"
#include <juce_audio_processors/juce_audio_processors.h>

static constexpr int max_delay_samples = 192000 * 2; // 2 seconds at 192kHz

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
        SCFloat  mix_default  = 50.f;
        SCFloat  mix_stepSize = 0.1f;
        SCFloat  mix_skew     = Skew::linear;

        // bypass (bool)
        SCString bypass_ID      = "bypass";
        SCString bypass_name    = "Bypass";
        SCBool   bypass_default = false;

        // time (float, ms) - used when 'Sync' is off
        SCString time_ID       = "time";
        SCString time_name     = "Time";
        SCString time_suffix   = "ms";
        SCFloat  time_min      = 1.f;
        SCFloat  time_max      = 2000.f;
        SCFloat  time_default  = 350.f;
        SCFloat  time_stepSize = 1.f;
        SCFloat  time_skew     = Skew::logarithmic;

        // feedback (float)
        SCString feedback_ID       = "feedback";
        SCString feedback_name     = "Feedback";
        SCString feedback_suffix   = "%";
        SCFloat  feedback_min      = 0.f;
        SCFloat  feedback_max      = 100.f;
        SCFloat  feedback_default  = 35.f;
        SCFloat  feedback_stepSize = 0.1f;
        SCFloat  feedback_skew     = Skew::linear;

        // sync (bool)
        SCString sync_ID      = "sync";
        SCString sync_name    = "Sync to Tempo";
        SCBool   sync_default = false;

        // division (choice) - note value used when sync enabled
        SCString    division_ID      = "division";
        SCString    division_name    = "Division";
        SCStringArr division_choices = {
            "1/1",
            "1/2",
            "1/4",
            "1/4T",
            "1/8",
            "1/8T",
            "1/16",
            "1/16T",
        };
        SCInt division_default = 2; // 1/4
    };

    pFloat inGainSmooth;
    pFloat outGainSmooth;
    pFloat mixSmooth;

    pFloat timeSmooth;
    pFloat feedbackSmooth;
    pBool  syncParam;
    pInt   divisionParam;

    pBool bypassParam;

    static constexpr float division_multipliers[] = {
        4.f, 2.f, 1.f, 2.f / 3.f, 0.5f, 1.f / 3.f, 0.25f, 1.f / 6.f};

  private:
    static APVTS::ParameterLayout createParameterLayout();

    std::array<DelayLine<float, max_delay_samples>, 2> delay_lines;

    juce::SmoothedValue<float> delay_samples_smoothed;
};
