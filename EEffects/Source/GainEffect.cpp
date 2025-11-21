#include "GainEffect.h"

GainEffect::GainEffect (juce::AudioProcessorValueTreeState& vts, juce::String name, juce::String parameterIdIn) noexcept
    : IEffect (vts, name), paramId (std::move (parameterIdIn))
{
}

void GainEffect::prepare (const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    gainProcessor.reset();
    gainProcessor.prepare (spec);
    gainProcessor.setRampDurationSeconds (smoothingTimeSeconds);

    if (auto* raw = parameters.getRawParameterValue (paramId))
        gainProcessor.setGainDecibels (raw->load());
}

void GainEffect::reset()
{
    gainProcessor.reset();
}

void GainEffect::process (juce::AudioBuffer<float>& buffer)
{
    if (! isActive())
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    const float gainDb = *parameters.getRawParameterValue (paramId);
    gainProcessor.setGainDecibels (gainDb);

    juce::dsp::AudioBlock<float> block (const_cast<float**> (buffer.getArrayOfWritePointers()),
                                        static_cast<size_t> (numChannels),
                                        static_cast<size_t> (numSamples));

    juce::dsp::ProcessContextReplacing<float> ctx (block);
    gainProcessor.process (ctx);
}