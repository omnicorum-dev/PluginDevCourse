#include "processor.h"
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
             Params::drive_ID,
             Params::drive_name,
             Params::drive_min,
             Params::drive_max,
             Params::drive_default,
             Params::drive_stepSize,
             Params::drive_skew,
             Params::drive_suffix);

    addFloat(layout,
             Params::bias_ID,
             Params::bias_name,
             Params::bias_min,
             Params::bias_max,
             Params::bias_default,
             Params::bias_stepSize,
             Params::bias_skew,
             Params::bias_suffix);

    addBool(layout,
            Params::dcblock_ID,
            Params::dcblock_name,
            Params::dcblock_default);

    addChoice(layout,
              Params::type_ID,
              Params::type_name,
              Params::type_choices,
              Params::type_default);

    addBool(
        layout, Params::bypass_ID, Params::bypass_name, Params::bypass_default);

    return layout;
}

/* ======================================================== */

void Processor::prepareToPlay(double sample_rate, int buffer_size) {
    outGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::outGain_ID);
    inGainSmooth.prepare(sample_rate, buffer_size, &apvts, Params::inGain_ID);
    mixSmooth.prepare(sample_rate, buffer_size, &apvts, Params::mix_ID);

    driveSmooth.prepare(sample_rate, buffer_size, &apvts, Params::drive_ID);
    biasSmooth.prepare(sample_rate, buffer_size, &apvts, Params::bias_ID);

    dcBlockParam.prepare(sample_rate, buffer_size, &apvts, Params::dcblock_ID);
    typeParam.prepare(sample_rate, buffer_size, &apvts, Params::type_ID);

    bypassParam.prepare(sample_rate, buffer_size, &apvts, Params::bypass_ID);

    // Prepare any objects here
    // e.g. Delay.prepare(sample_rate);
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

    bool dcblock = dcBlockParam.getNextValue();
    int  type    = typeParam.getNextValue();

    // Update smoothers
    outGainSmooth.update();
    inGainSmooth.update();
    mixSmooth.update();

    driveSmooth.update();
    biasSmooth.update();

    // Update objects for discrete changes
    // eg. if (filterTypeParam.changed()) filter.updateCoefficients();

    if (dcBlockParam.changed()) {
        for (auto &d : hard_clip)
            d.useDcBlock(dcblock);
        for (auto &d : tanh_shaper)
            d.useDcBlock(dcblock);
        for (auto &d : half_rectifier)
            d.useDcBlock(dcblock);
        for (auto &d : full_rectifier)
            d.useDcBlock(dcblock);
    }

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

        float drive = std::pow(10.f, driveSmooth.getNextValue() / 20.f);
        float bias  = biasSmooth.getNextValue();

        for (auto &d : hard_clip) {
            d.setDrive(drive);
            d.setBias(bias);
        }
        for (auto &d : tanh_shaper) {
            d.setDrive(drive);
            d.setBias(bias);
        }
        for (auto &d : full_rectifier) {
            d.setDrive(drive);
            d.setBias(bias);
        }
        for (auto &d : half_rectifier) {
            d.setDrive(drive);
            d.setBias(bias);
        }

        for (int channel = 0; channel < num_channels; ++channel) {
            float *channel_data = channel_ptrs[(size_t)channel];
            float  dry          = channel_data[sample];
            float  xn           = dry * in_gain;

            /* ======================================================== */

            float yn = xn;

            switch (type) {
            case 0:
                yn = hard_clip[(size_t)channel].processSample(xn);
                break;
            case 1:
                yn = tanh_shaper[(size_t)channel].processSample(xn);
                break;
            case 2:
                yn = full_rectifier[(size_t)channel].processSample(xn);
                break;
            case 3:
                yn = half_rectifier[(size_t)channel].processSample(xn);
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
