#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

class FilterEffect : public IEffect
{
public:
	FilterEffect(juce::String name) noexcept;
	~FilterEffect() override = default;

	void prepare(const juce::dsp::ProcessSpec& spec) override;
	void process(juce::dsp::ProcessContextNonReplacing<float> context) override;
	void reset() override;

	struct FilterParameters
	{
		float cutoffFrequency = 1000.0f; // Hz
		float resonance = 1.0f;           // Q factor
		bool bypass = false;
	};
	void setParameters(const FilterParameters& params);
private:
	using IIRFilter = juce::dsp::IIR::Filter<float>;
	using IIRCoeffs = juce::dsp::IIR::Coefficients<float>;

	juce::dsp::ProcessorDuplicator<IIRFilter, IIRCoeffs> filter;

	FilterParameters parameters;
	juce::dsp::ProcessSpec lastSpec;
};