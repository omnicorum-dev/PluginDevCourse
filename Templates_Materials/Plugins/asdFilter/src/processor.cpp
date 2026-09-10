#include "processor.h"
#include "filter.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors_headless/juce_audio_processors_headless.h"
#include "juce_core/juce_core.h"
#include "midi_cursor.h"
#include "parameter_layout.h"

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
             Params::freq_ID,
             Params::freq_name,
             Params::freq_min,
             Params::freq_max,
             Params::freq_default,
             Params::freq_stepSize,
             Params::freq_skew,
             Params::freq_suffix);

    addFloat(layout,
             Params::q_ID,
             Params::q_name,
             Params::q_min,
             Params::q_max,
             Params::q_default,
             Params::q_stepSize,
             Params::q_skew,
             Params::q_suffix);

    addFloat(layout,
             Params::gain_ID,
             Params::gain_name,
             Params::gain_min,
             Params::gain_max,
             Params::gain_default,
             Params::gain_stepSize,
             Params::gain_skew,
             Params::gain_suffix);

    addChoice(layout,
              Params::filter_ID,
              Params::filter_name,
              Params::filter_choices,
              Params::filter_default);

    addChoice(layout,
              Params::type_ID,
              Params::type_name,
              Params::type_choices,
              Params::type_default);

    addInt(layout,
           Params::stages_ID,
           Params::stages_name,
           Params::stages_min,
           Params::stages_max,
           Params::stages_default);

    addBool(
        layout, Params::bypass_ID, Params::bypass_name, Params::bypass_default);

    return layout;
}

/* ======================================================== */

void Processor::prepareToPlay(double sample_rate, int buffer_size) {
    outGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::outGain_ID);
    inGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::inGain_ID);
    mixSmooth.prepare(sample_rate, buffer_size, &apvts, Params::mix_ID);

    freqSmooth.prepare(sample_rate, buffer_size, &apvts, Params::freq_ID);
    qSmooth.prepare(sample_rate, buffer_size, &apvts, Params::q_ID);
    gainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::gain_ID);

    typeParam.prepare(sample_rate, buffer_size, &apvts, Params::type_ID);
    filterParam.prepare(sample_rate, buffer_size, &apvts, Params::filter_ID);
    stagesParam.prepare(sample_rate, buffer_size, &apvts, Params::stages_ID);

    bypassParam.prepare(sample_rate, buffer_size, &apvts, Params::bypass_ID);

    // Prepare any objects here
    // e.g. Delay.prepare(sample_rate);

    for (RBJ &f : rbj)
        f.prepare((float)sample_rate, buffer_size);
    for (LR4 &f : lr4)
        f.prepare((float)sample_rate, buffer_size);
    for (HighOrderFilter<8> &f : highOrder)
        f.prepare((float)sample_rate, buffer_size);
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

    int type   = typeParam.getNextValue();
    int filter = filterParam.getNextValue();
    int stages = stagesParam.getNextValue();

    // Update smoothers
    outGainSmooth.update();
    inGainSmooth.update();
    mixSmooth.update();

    freqSmooth.update();
    qSmooth.update();
    gainSmooth.update();

    // Update objects for discrete changes
    // eg. if (filterTypeParam.changed()) filter.updateCoefficients();
    if (typeParam.changed()) {
        float freqNow = freqSmooth.getCurrentValue();
        float qNow    = qSmooth.getCurrentValue();
        float gainNow = std::pow(10.f, gainSmooth.getCurrentValue() / 20.f);

        switch (type) {
        case 0:
            for (RBJ &f : rbj) {
                f.setAll(freqNow, qNow, gainNow);
            }
        case 1:
            for (HighOrderFilter<8> &f : highOrder) {
                f.setFilterType((FilterType)filter);
                f.setStages(stages);
                f.setFreqAndQ(freqNow, qNow);
            }
            break;
        case 2:
            for (LR4 &f : lr4) {
                f.setFilterType((FilterType)filter);
                f.setFreq(freqNow);
            }
            break;
        }
    }

    if (stagesParam.changed()) {
        switch (type) {
        case 1:
            for (HighOrderFilter<8> &f : highOrder)
                f.setStages(stages);
            break;
        }
    }

    if (filterParam.changed()) {
        switch (type) {
        case 0:
            for (RBJ &f : rbj)
                f.setFilterType((FilterType)filter);
            break;
        case 1:
            for (HighOrderFilter<8> &f : highOrder)
                f.setFilterType((FilterType)filter);
            break;
        case 2:
            for (LR4 &f : lr4)
                f.setFilterType((FilterType)filter);
            break;
        }
    }

    /* ======================================================== */

    MidiCursor midi(messages);

    constexpr int max_channels = 8;
    auto          num_channels = total_input_channels;

    std::array<float *, max_channels> channel_ptrs;

    for (int channel = 0; channel < num_channels; ++channel) {
        channel_ptrs[(size_t)channel] = buffer.getWritePointer(channel);
    }

    /* ======================================================== */

    // Process audio and midi messages

    for (int sample = 0; sample < num_samples; ++sample) {

        while (midi.hasEvent() && midi.event().samplePosition == sample) {
            juce::MidiMessage message = midi.event().getMessage();

            // apply changes for this sample based on the midi message received
            // this sample

            midi.advance();
        }

        float in_gain  = std::pow(10.f, inGainSmooth.getNextValue() / 20.f);
        float out_gain = std::pow(10.f, outGainSmooth.getNextValue() / 20.f);
        float mix      = mixSmooth.getNextValue();

        // Update objects for continuous changes here
        float freq        = freqSmooth.getNextValue();
        float q           = qSmooth.getNextValue();
        float gain_linear = std::pow(10.f, gainSmooth.getNextValue() / 20.f);

        if (freqSmooth.changed() || qSmooth.changed() || gainSmooth.changed()) {
            switch (type) {
            case 0:
                for (RBJ &f : rbj)
                    f.setAll(freq, q, gain_linear);
                break;
            case 1:
                for (HighOrderFilter<8> &f : highOrder)
                    f.setFreqAndQ(freq, q);
                break;
            case 2:
                for (LR4 &f : lr4)
                    f.setFreq(freq);
                break;
            }
        }

        for (int channel = 0; channel < num_channels; ++channel) {
            float *channel_data = channel_ptrs[(size_t)channel];
            float  dry          = channel_data[sample];
            float  xn           = dry * in_gain;

            /* ======================================================== */

            float yn = xn;
            switch (type) {
            case 0:
                yn = rbj[(size_t)channel].processSample(xn);
                break;
            case 1:
                yn = highOrder[(size_t)channel].processSample(xn);
                break;
            case 2:
                yn = lr4[(size_t)channel].processSample(xn);
                break;
            default:
                break;
            }

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
