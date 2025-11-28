#include "ConvolutionEffect.h"
#include <JuceHeader.h>

ConvolutionEffect::ConvolutionEffect(juce::String name) noexcept
    : IEffect(name)
{

    if (BinaryData::reverb_ir_wav != nullptr)
    {
        convolution.loadImpulseResponse (BinaryData::reverb_ir_wav,
                                         BinaryData::reverb_ir_wavSize,
                                         juce::dsp::Convolution::Stereo::yes,
                                         juce::dsp::Convolution::Trim::yes,
                                         0,
                                         juce::dsp::Convolution::Normalise::yes);
    }
    mixer.setMixingRule(dsp::DryWetMixingRule::balanced);
}

void ConvolutionEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
	convolution.prepare(spec);
	mixer.prepare(spec);
}

void ConvolutionEffect::reset()
{
	convolution.reset();
    mixer.reset();
}

void ConvolutionEffect::process(juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (!isActive() || parameters.bypass)
    {
        context.getOutputBlock().copyFrom(context.getInputBlock());
        return;
    }

	mixer.pushDrySamples(context.getInputBlock());
    convolution.process(context);
    const auto& outputBlock = context.getOutputBlock();
    outputBlock.multiplyBy(4.0f);
    mixer.mixWetSamples(outputBlock);
}

void ConvolutionEffect::setParameters(ConvolutionParameters& params)
{
    parameters = params;
	mixer.setWetMixProportion(parameters.mix);
}

