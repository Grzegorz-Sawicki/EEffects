#include "DelayEffect.h"

DelayEffect::DelayEffect(juce::AudioProcessorValueTreeState& vts, float maxDelaySeconds_) noexcept
    : IEffect(vts), maxDelaySeconds(maxDelaySeconds_)
{
}

void DelayEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    const int maxSamples = static_cast<int> (std::ceil(maxDelaySeconds * spec.sampleRate)) + 2;
    delayLine = decltype(delayLine) (maxSamples);
    delayLine.prepare(spec);
    delayLine.reset();
}

void DelayEffect::reset()
{
    delayLine.reset();
}

void DelayEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isActive())
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float sr = static_cast<float> (lastSpec.sampleRate);

    for (int i = 0; i < numSamples; ++i)
    {
        const float delayMs = *parameters.getRawParameterValue("delayTimeMs");
        const float fbParam = *parameters.getRawParameterValue("delayFeedback");
        const float wetParam = *parameters.getRawParameterValue("delayWet");

        const float delaySamples = juce::jlimit(0.0f, maxDelaySeconds * sr, delayMs * sr * 0.001f);
        const float fb = juce::jlimit(0.0f, 0.99f, fbParam);
        const float wet = juce::jlimit(0.0f, 1.0f, wetParam);
        const float dry = 1.0f - wet;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* ptr = buffer.getWritePointer(ch);
            const float inSample = ptr[i];

            // read delayed sample (DelayLine does internal interpolation)
            const float delayed = delayLine.popSample(ch, delaySamples, true);

            const float outSample = inSample * dry + delayed * wet;
            const float writeSample = inSample + delayed * fb;

            delayLine.pushSample(ch, writeSample);

            ptr[i] = outSample;
        }
    }
}