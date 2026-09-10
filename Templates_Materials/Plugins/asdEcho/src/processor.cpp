#include "processor.h"
#include "delay_line.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors_headless/juce_audio_processors_headless.h"
#include "juce_core/juce_core.h"

/* ======================================================== */

Processor::Processor()
    : juce::AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

Processor::~Processor() = default;

/* ======================================================== */

APVTS::ParameterLayout Processor::createParameterLayout() {
    APVTS::ParameterLayout layout;

    addFloat(layout,
             Params::inGain_ID,
             Params::inGain_name,
             Params::inGain_min,
             Params::inGain_max,
             Params::inGain_default,
             Params::inGain_stepSize,
             Params::inGain_skew,
             Params::inGain_suffix);

    addFloat(layout,
             Params::outGain_ID,
             Params::outGain_name,
             Params::outGain_min,
             Params::outGain_max,
             Params::outGain_default,
             Params::outGain_stepSize,
             Params::outGain_skew,
             Params::outGain_suffix);

    addFloat(layout,
             Params::mix_ID,
             Params::mix_name,
             Params::mix_min,
             Params::mix_max,
             Params::mix_default,
             Params::mix_stepSize,
             Params::mix_skew,
             Params::mix_suffix);

    addFloat(layout,
             Params::time_ID,
             Params::time_name,
             Params::time_min,
             Params::time_max,
             Params::time_default,
             Params::time_stepSize,
             Params::time_skew,
             Params::time_suffix);

    addFloat(layout,
             Params::feedback_ID,
             Params::feedback_name,
             Params::feedback_min,
             Params::feedback_max,
             Params::feedback_default,
             Params::feedback_stepSize,
             Params::feedback_skew,
             Params::feedback_suffix);

    addBool(layout, Params::sync_ID, Params::sync_name, Params::sync_default);

    addChoice(layout,
              Params::division_ID,
              Params::division_name,
              Params::division_choices,
              Params::division_default);

    addBool(
        layout, Params::bypass_ID, Params::bypass_name, Params::bypass_default);

    // use addInt and addChoice for ints and choices

    return layout;
}

/* ======================================================== */

void Processor::prepareToPlay(double sample_rate, int buffer_size) {
    outGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::outGain_ID);
    inGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::inGain_ID);
    mixSmooth.prepare(sample_rate, buffer_size, &apvts, Params::mix_ID);

    timeSmooth.prepare(sample_rate, buffer_size, &apvts, Params::time_ID);
    feedbackSmooth.prepare(
        sample_rate, buffer_size, &apvts, Params::feedback_ID);
    syncParam.prepare(sample_rate, buffer_size, &apvts, Params::sync_ID);
    divisionParam.prepare(
        sample_rate, buffer_size, &apvts, Params::division_ID);

    bypassParam.prepare(sample_rate, buffer_size, &apvts, Params::bypass_ID);

    delay_samples_smoothed.reset(sample_rate, 0.05);

    bool  sync = syncParam.getNextValue();
    float time_ms =
        timeSmooth
            .getRawAndUpdate(); // safe now — nothing cached yet to be stale

    double bpm = 120.0; // getPlayHead() is often null/unset this early — a sane
                        // fallback is fine
    float quarter_note_ms = 60000.f / (float)bpm;
    float synced_ms =
        quarter_note_ms * division_multipliers[divisionParam.getNextValue()];

    float target_ms       = sync ? synced_ms : time_ms;
    float initial_samples = std::clamp(target_ms * 0.001f * (float)sample_rate,
                                       1.f,
                                       (float)max_delay_samples - 2.f);

    delay_samples_smoothed.setCurrentAndTargetValue(initial_samples);

    // Prepare any objects here
    // e.g. Delay.prepare(sample_rate);

    for (DelayLine<float, max_delay_samples> &line : delay_lines) {
        line.clear();
    }
}

void Processor::releaseResources() {}

void Processor::processBlock(juce::AudioBuffer<float> &buffer,
                             juce::MidiBuffer         &messages) {

    juce::ScopedNoDenormals no_denormals;

    int total_input_channels  = getTotalNumInputChannels();
    int total_output_channels = getTotalNumOutputChannels();
    int num_samples           = buffer.getNumSamples();

    for (auto i = total_input_channels; i < total_output_channels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    /* ======================================================== */

    // Read all control-rate parameters
    bool bypass = bypassParam.getNextValue();

    if (bypass)
        return;

    /* ======================================================== */

    double bpm = 120.0;
    if (juce::AudioPlayHead *playhead = getPlayHead()) {
        if (auto position = playhead->getPosition()) {
            if (auto reported_bpm = position->getBpm()) {
                bpm = *reported_bpm;
            }
        }
    }

    float quarter_note_ms = 60000.f / (float)bpm;

    float synced_ms =
        quarter_note_ms * division_multipliers[divisionParam.getNextValue()];

    bool  sync      = syncParam.getNextValue();
    float target_ms = sync ? synced_ms : timeSmooth.getRawAndUpdate();
    float target_samples =
        std::clamp(target_ms * 0.001f * (float)getSampleRate(),
                   1.f,
                   (float)max_delay_samples - 2.f);

    delay_samples_smoothed.setTargetValue(target_samples);

    /* ======================================================== */

    // Update smoothers
    outGainSmooth.update();
    inGainSmooth.update();
    mixSmooth.update();

    feedbackSmooth.update();

    // Update objects for discrete changes
    // eg. if (filterTypeParam.changed()) filter.updateCoefficients();

    /* ======================================================== */

    constexpr int max_channels = 8;
    auto          num_channels = total_input_channels;

    std::array<float *, max_channels> channel_ptrs;

    for (int channel = 0; channel < num_channels; ++channel) {
        channel_ptrs[(size_t)channel] = buffer.getWritePointer(channel);
    }

    /* ======================================================== */

    // Process audio and midi messages

    for (int sample = 0; sample < num_samples; ++sample) {

        float in_gain  = std::pow(10.f, inGainSmooth.getNextValue() / 20.f);
        float out_gain = std::pow(10.f, outGainSmooth.getNextValue() / 20.f);
        float mix      = mixSmooth.getNextValue();

        // Update objects for continuous changes here
        float delay_samples = delay_samples_smoothed.getNextValue();
        float feedback_amt  = feedbackSmooth.getNextValue() * 0.01f;
        feedback_amt        = std::min(feedback_amt, 0.98f);

        for (DelayLine<float, max_delay_samples> &line : delay_lines) {
            line.setFeedback(feedback_amt);
            line.setDelaySamples(delay_samples);
        }

        for (int channel = 0; channel < num_channels; ++channel) {
            float *channel_data = channel_ptrs[(size_t)channel];
            float  dry          = channel_data[sample];
            float  xn           = dry * in_gain;

            /* ======================================================== */

            float yn = delay_lines[(size_t)channel].processSample(xn);

            /* ======================================================== */

            float mixed = (yn * mix * 0.01f) + (dry * (100.f - mix) * 0.01f);
            channel_data[sample] = mixed * out_gain;
        }
    }
}

/* ======================================================== */

void Processor::getStateInformation(juce::MemoryBlock &dest) {
    auto                              state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void Processor::setStateInformation(const void *data, int size_in_bytes) {
    std::unique_ptr<juce::XmlElement> xml_state(
        getXmlFromBinary(data, size_in_bytes));
    if (xml_state.get() != nullptr)
        if (xml_state->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml_state));
}

/* ======================================================== */

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new Processor();
}
