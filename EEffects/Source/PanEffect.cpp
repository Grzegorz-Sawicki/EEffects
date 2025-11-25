#include "PanEffect.h"

PanEffect::PanEffect (juce::AudioProcessorValueTreeState& vts, juce::String name, juce::String parameterIdIn) noexcept
    : IEffect (vts, name), paramId (std::move (parameterIdIn))
{
}

void PanEffect::prepare (const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    panSmoothed.reset (spec.sampleRate, smoothingTimeSeconds);

    // initial value from parameter
    const float panVal = *parameters.getRawParameterValue (paramId);
    panSmoothed.setCurrentAndTargetValue (juce::jlimit (-1.0f, 1.0f, panVal));
}

void PanEffect::reset()
{
	panSmoothed.setCurrentAndTargetValue(panSmoothed.getCurrentValue());
}

void PanEffect::process (juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (! isActive())
        return;

	auto inputBlock = context.getInputBlock();
	auto outputBlock = context.getOutputBlock();
    const int numChannels = inputBlock.getNumChannels();
    const int numSamples  = inputBlock.getNumSamples();
    const float sr = static_cast<float> (lastSpec.sampleRate);

    const float panParam = *parameters.getRawParameterValue (paramId);
    panSmoothed.setTargetValue (juce::jlimit (-1.0f, 1.0f, panParam));

    if (numChannels < 2)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        const float pan = panSmoothed.getNextValue(); // -1..1
        const float angle = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f; // 0..pi/2
        const float leftGain  = std::cos (angle);
        const float rightGain = std::sin (angle);

	    const float* leftIn = inputBlock.getChannelPointer(0);
		const float* rightIn = inputBlock.getChannelPointer(1);
		float* leftOut = outputBlock.getChannelPointer(0);
        float* rightOut = outputBlock.getChannelPointer(1);

        leftOut[i] = leftIn[i] * leftGain;
        rightOut[i] = rightIn[i] * rightGain;

        for (int ch = 2; ch < numChannels; ++ch)
        {
            // optional: apply center/pan algorithm for additional channels;
            // for now, leave channel as is
        }
    }
}