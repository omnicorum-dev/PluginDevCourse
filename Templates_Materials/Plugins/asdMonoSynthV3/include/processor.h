#pragma once

#include "envelope.h"
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

    bool acceptsMidi() const override { return true; }
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

        // amp (float)
        SCString amp_ID       = "amp";
        SCString amp_name     = "Amplitude";
        SCString amp_suffix   = "";
        SCFloat  amp_min      = 0.f;
        SCFloat  amp_max      = 1.f;
        SCFloat  amp_default  = 0.1f;
        SCFloat  amp_stepSize = 0.01f;
        SCFloat  amp_skew     = Skew::logarithmic;

        // attack (float)
        SCString attack_ID       = "attack";
        SCString attack_name     = "Attack Time";
        SCString attack_suffix   = "s";
        SCFloat  attack_min      = 0.f;
        SCFloat  attack_max      = 1.f;
        SCFloat  attack_default  = 0.1f;
        SCFloat  attack_stepSize = 0.001f;
        SCFloat  attack_skew     = Skew::logarithmic;

        // hold (float)
        SCString hold_ID       = "hold";
        SCString hold_name     = "Hold Time";
        SCString hold_suffix   = "s";
        SCFloat  hold_min      = 0.f;
        SCFloat  hold_max      = 1.f;
        SCFloat  hold_default  = 0.0f;
        SCFloat  hold_stepSize = 0.01f;
        SCFloat  hold_skew     = Skew::logarithmic;

        // decay (float)
        SCString decay_ID       = "decay";
        SCString decay_name     = "Decay Time";
        SCString decay_suffix   = "s";
        SCFloat  decay_min      = 0.f;
        SCFloat  decay_max      = 3.f;
        SCFloat  decay_default  = 1.f;
        SCFloat  decay_stepSize = 0.001f;
        SCFloat  decay_skew     = Skew::logarithmic;

        // sustain (float)
        SCString sustain_ID       = "sustain";
        SCString sustain_name     = "Sustain Level";
        SCString sustain_suffix   = "";
        SCFloat  sustain_min      = 0.f;
        SCFloat  sustain_max      = 1.f;
        SCFloat  sustain_default  = 0.3f;
        SCFloat  sustain_stepSize = 0.001f;
        SCFloat  sustain_skew     = Skew::logarithmic;

        // release (float)
        SCString release_ID       = "release";
        SCString release_name     = "Release Time";
        SCString release_suffix   = "s";
        SCFloat  release_min      = 0.f;
        SCFloat  release_max      = 5.f;
        SCFloat  release_default  = 0.5f;
        SCFloat  release_stepSize = 0.001f;
        SCFloat  release_skew     = Skew::logarithmic;

        // ramp type (choice)
        SCString    ramp_ID      = "ramp";
        SCString    ramp_name    = "Ramp Type";
        SCStringArr ramp_choices = {"Linear", "Exponential"};
        SCInt       ramp_default = 0;

        // bypass (bool)
        SCString bypass_ID      = "bypass";
        SCString bypass_name    = "Bypass";
        SCBool   bypass_default = false;
    };

    pFloat inGainSmooth;
    pFloat outGainSmooth;
    pFloat mixSmooth;

    pFloat attackSmooth;
    pFloat holdSmooth;
    pFloat decaySmooth;
    pFloat sustainSmooth;
    pFloat releaseSmooth;

    pFloat ampSmooth;

    pInt  rampParam;
    pBool bypassParam;

  private:
    static APVTS::ParameterLayout createParameterLayout();

    std::array<Oscillator, 2> oscillator;
    // std::array<AHDSR, 2>      envelope;
    AHDSR envelope;

    int  midiNote        = 69;
    bool midiNoteChanged = true;

    bool isNoteOn = false;
};
