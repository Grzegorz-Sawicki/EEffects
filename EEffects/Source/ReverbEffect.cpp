#include "ReverbEffect.h"

ReverbEffect::ReverbEffect(juce::String name) noexcept
    : IEffect(name)
{
}

void ReverbEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);
}

void ReverbEffect::reset()
{
    reverb.reset();
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