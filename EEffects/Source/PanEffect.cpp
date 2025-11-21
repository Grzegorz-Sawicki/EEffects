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

void PanEffect::process (juce::AudioBuffer<float>& buffer)
{
    if (! isActive())
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    const float sr = static_cast<float> (lastSpec.sampleRate);

    // Set smoothing target once per block
    const float panParam = *parameters.getRawParameterValue (paramId);
    panSmoothed.setTargetValue (juce::jlimit (-1.0f, 1.0f, panParam));

    // If mono -> do nothing (or expand to stereo if you want)
    if (numChannels < 2)
        return;

    // per-sample equal-power panning
    for (int i = 0; i < numSamples; ++i)
    {
        const float pan = panSmoothed.getNextValue(); // -1..1
        const float angle = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f; // 0..pi/2
        const float leftGain  = std::cos (angle);
        const float rightGain = std::sin (angle);

        float* leftPtr  = buffer.getWritePointer (0);
        float* rightPtr = buffer.getWritePointer (1);

        leftPtr[i]  *= leftGain;
        rightPtr[i] *= rightGain;

        // if more channels (surround), keep others unchanged (or copy one channel)
        for (int ch = 2; ch < numChannels; ++ch)
        {
            // optional: apply center/pan algorithm for additional channels;
            // for now, leave channel as is
        }
    }
}