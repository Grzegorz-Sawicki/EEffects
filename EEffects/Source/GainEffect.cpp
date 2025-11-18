#include "GainEffect.h"

GainEffect::GainEffect (juce::AudioProcessorValueTreeState& vts, juce::String parameterIdIn) noexcept
    : IEffect (vts), paramId (std::move (parameterIdIn))
{
}

void GainEffect::prepare (const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    gainProcessor.reset();
    gainProcessor.prepare (spec);
    gainProcessor.setRampDurationSeconds (smoothingTimeSeconds);

    // initialize gain from parameter (in dB)
    if (auto* val = parameters.getParameter (paramId))
        gainProcessor.setGainDecibels (val->getValue() * (val->getNormalisableRange().end - val->getNormalisableRange().start) + val->getNormalisableRange().start);
    // safer: read raw parameter value as dB
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

    // read parameter (dB) once per block
    const float gainDb = *parameters.getRawParameterValue (paramId);
    gainProcessor.setGainDecibels (gainDb);

    // build AudioBlock from buffer and process
    juce::dsp::AudioBlock<float> block (const_cast<float**> (buffer.getArrayOfWritePointers()),
                                        static_cast<size_t> (numChannels),
                                        static_cast<size_t> (numSamples));

    juce::dsp::ProcessContextReplacing<float> ctx (block);
    gainProcessor.process (ctx);
}