#pragma once

#include "Effect.h"

class ConvolutionEffect : public IEffect
{
public:
	ConvolutionEffect(juce::String name) noexcept;
	~ConvolutionEffect() override = default;

	void prepare(const juce::dsp::ProcessSpec& spec) override;
	void process(juce::dsp::ProcessContextNonReplacing<float> context) override;
	void reset() override;

	struct ConvolutionParameters
	{
		float mix = 1.0f;
		bool bypass = false;
	};

	void setParameters(ConvolutionParameters& params);
	int getLatency() const noexcept { return convolution.getLatency(); }
private:
	juce::dsp::Convolution convolution;
	ConvolutionParameters parameters;
	dsp::DryWetMixer<float> mixer;
};
