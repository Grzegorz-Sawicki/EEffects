#include "DelayEffect.h"

DelayEffect::DelayEffect (juce::AudioProcessorValueTreeState& vts, float maxDelaySeconds_) noexcept
    : IEffect (vts), maxDelaySeconds (maxDelaySeconds_)
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

    const float delayMs = *parameters.getRawParameterValue ("delayTimeMs");
    const float sr = static_cast<float> (spec.sampleRate);
    const float delaySamples = juce::jlimit (0.0f, maxDelaySeconds * sr, delayMs * sr * 0.001f);
    delayTimeSmoothed.setCurrentAndTargetValue (delaySamples);

	const float fbVal = *parameters.getRawParameterValue("delayFeedback");

    const float fb = juce::jlimit (0.0f, 0.99f, fbVal);
    delayFeedbackSmoothed.setCurrentAndTargetValue (fb);

	const float wetVal = *parameters.getRawParameterValue("delayWet");

    const float wet = juce::jlimit (0.0f, 1.0f, wetVal);
    delayWetSmoothed.setCurrentAndTargetValue (wet);
}

void DelayEffect::reset()
{
    delayLine.reset();
    delayTimeSmoothed.setCurrentAndTargetValue (delayTimeSmoothed.getCurrentValue());
    delayFeedbackSmoothed.setCurrentAndTargetValue (delayFeedbackSmoothed.getCurrentValue());
    delayWetSmoothed.setCurrentAndTargetValue (delayWetSmoothed.getCurrentValue());
}

void DelayEffect::process (juce::AudioBuffer<float>& buffer)
{
    if (! isActive())
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    const float sr = static_cast<float> (lastSpec.sampleRate);

    const float delayMsParam = *parameters.getRawParameterValue ("delayTimeMs");
    const float targetDelaySamples = juce::jlimit (0.0f, maxDelaySeconds * sr, delayMsParam * sr * 0.001f);
    delayTimeSmoothed.setTargetValue (targetDelaySamples);

    const float fbParam = *parameters.getRawParameterValue ("delayFeedback");
    delayFeedbackSmoothed.setTargetValue (juce::jlimit (0.0f, 0.99f, fbParam));

    const float wetParam = *parameters.getRawParameterValue ("delayWet");
    delayWetSmoothed.setTargetValue (juce::jlimit (0.0f, 1.0f, wetParam));

    for (int i = 0; i < numSamples; ++i)
    {
        const float delaySamples = delayTimeSmoothed.getNextValue();
        const float fb = delayFeedbackSmoothed.getNextValue();
        const float wet = delayWetSmoothed.getNextValue();
        const float dry = 1.0f - wet;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* ptr = buffer.getWritePointer (ch);
            const float inSample = ptr[i];

            const float delayed = delayLine.popSample (ch, delaySamples, true);

            const float outSample = inSample * dry + delayed * wet;
            const float writeSample = inSample + delayed * fb;

            delayLine.pushSample (ch, writeSample);

            ptr[i] = outSample;
        }
    }
}