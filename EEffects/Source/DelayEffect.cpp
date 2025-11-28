#include "DelayEffect.h"

DelayEffect::DelayEffect (juce::String name, float maxDelaySeconds_) noexcept
    : IEffect (name), maxDelaySeconds (maxDelaySeconds_)
{
}

void DelayEffect::prepare (const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    const int maxSamples = static_cast<int> (std::ceil (maxDelaySeconds * spec.sampleRate)) + 2;
    delayLine = decltype (delayLine) (maxSamples);
    delayLine.prepare (spec);
    delayLine.reset();

    delayTimeSmoothed.reset (spec.sampleRate, smoothingTimeSeconds);
    delayFeedbackSmoothed.reset (spec.sampleRate, smoothingTimeSeconds);
    delayWetSmoothed.reset (spec.sampleRate, smoothingTimeSeconds);
}

void DelayEffect::reset()
{
    delayLine.reset();
    delayTimeSmoothed.setCurrentAndTargetValue (delayTimeSmoothed.getCurrentValue());
    delayFeedbackSmoothed.setCurrentAndTargetValue (delayFeedbackSmoothed.getCurrentValue());
    delayWetSmoothed.setCurrentAndTargetValue (delayWetSmoothed.getCurrentValue());
}

void DelayEffect::process (juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (!isActive() || parameters.bypass) {
        context.getOutputBlock().copyFrom(context.getInputBlock());
        return;
    }

	auto inputBlock = context.getInputBlock();
	auto outputBlock = context.getOutputBlock();
    const int numChannels = inputBlock.getNumChannels();
    const int numSamples  = inputBlock.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        const float delaySamples = delayTimeSmoothed.getNextValue();
        const float fb = delayFeedbackSmoothed.getNextValue();
        const float wet = delayWetSmoothed.getNextValue();
        const float dry = 1.0f - wet;

        for (int ch = 0; ch < numChannels; ++ch)
        {
			auto* in = inputBlock.getChannelPointer(ch);
			auto* out = outputBlock.getChannelPointer(ch);
            const float inSample = in[i];

            const float delayed = delayLine.popSample (ch, delaySamples, true);

            const float outSample = inSample * dry + delayed * wet;
            const float writeSample = inSample + delayed * fb;

            delayLine.pushSample (ch, writeSample);

            out[i] = outSample;
        }
    }
}

void DelayEffect::setParameters(const DelayParameters& params)
{
    parameters = params;
    delayTimeSmoothed.setTargetValue(parameters.delayTimeMs * sampleRate * 0.001f);
	delayFeedbackSmoothed.setTargetValue(parameters.feedback);
	delayWetSmoothed.setTargetValue(parameters.wetLevel);
}