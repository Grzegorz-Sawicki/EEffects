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

void GainEffect::process (juce::dsp::ProcessContextReplacing<float> context)
{
    if (!isActive())
        return;

    const float gainDb = *parameters.getRawParameterValue (paramId);
    gainProcessor.setGainDecibels (gainDb);

    gainProcessor.process (context);
}