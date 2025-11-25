#include "ReverbEffect.h"

ReverbEffect::ReverbEffect(juce::String name) noexcept
    : IEffect(name)
{
}

void ReverbEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    reverb.reset();
    reverb.prepare(spec);
    tempBuffer.setSize((int)spec.numChannels, (int)spec.maximumBlockSize);
}

void ReverbEffect::reset()
{
    reverb.reset();
    tempBuffer.clear();
}

void ReverbEffect::process(juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (!isActive() || parameters.bypass) {
		context.getOutputBlock().copyFrom(context.getInputBlock());
        return;
    }
    reverb.process(context);
}

void ReverbEffect::setParameters(ReverbParameters& params) {
    parameters = params;
	reverb.setParameters(parameters.reverbParameters);
}