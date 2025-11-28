#include "FilterEffect.h"

FilterEffect::FilterEffect(juce::String name) noexcept
	: IEffect(name)
{
}

void FilterEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
	lastSpec = spec;
	filter.prepare(spec);
}

void FilterEffect::reset()
{
	filter.reset();
}

void FilterEffect::process(juce::dsp::ProcessContextNonReplacing<float> context)
{
	if (!isActive() || parameters.bypass) {
		context.getOutputBlock().copyFrom(context.getInputBlock());
		return;
	}
	filter.process(context);
}

void FilterEffect::setParameters(const FilterParameters& params) {
	parameters = params;
	auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
		lastSpec.sampleRate,
		parameters.cutoffFrequency,
		parameters.resonance
	);

	*filter.state = *coefficients;
}