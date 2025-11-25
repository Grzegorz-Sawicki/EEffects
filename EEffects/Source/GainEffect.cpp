#include "GainEffect.h"

GainEffect::GainEffect (juce::String name) noexcept
    : IEffect (name)
{
}

void GainEffect::prepare (const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    gainProcessor.reset();
    gainProcessor.prepare (spec);
    gainProcessor.setRampDurationSeconds (smoothingTimeSeconds);
}

void GainEffect::reset()
{
    gainProcessor.reset();
}

void GainEffect::process (juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (!isActive()) return;
    gainProcessor.process (context);
}

void GainEffect::setParameters(const GainParameters& params)
{
    parameters = params;
	gainProcessor.setGainDecibels(parameters.gainDb);
}