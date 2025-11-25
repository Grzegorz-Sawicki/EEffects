#include "ReverbEffect.h"

ReverbEffect::ReverbEffect(juce::AudioProcessorValueTreeState& vts, juce::String name) noexcept
    : IEffect(vts, name)
{
}

void ReverbEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    lastSpec = spec;
    reverb.reset();
    reverb.prepare(spec);
    tempBuffer.setSize((int)spec.numChannels, (int)spec.maximumBlockSize);
}

void ReverbEffect::reset()
{
    reverb.reset();
    tempBuffer.clear();
}

void ReverbEffect::process(juce::dsp::ProcessContextNonReplacing<float> context)
{
    if (!isActive())
        return;

    const bool bypass = *parameters.getRawParameterValue("reverbBypass");
    if (bypass)
        return;

    // read parameters
    const float wetParam = *parameters.getRawParameterValue("reverbWet");
    const float roomParam = *parameters.getRawParameterValue("reverbRoom");
    const float dampParam = *parameters.getRawParameterValue("reverbDamping");
    const float widthParam = *parameters.getRawParameterValue("reverbWidth");

    const float wet  = juce::jlimit(0.0f, 1.0f, wetParam);
    const float room = juce::jlimit(0.0f, 1.0f, roomParam);
    const float damp = juce::jlimit(0.0f, 1.0f, dampParam);
    const float width = juce::jlimit(0.0f, 1.0f, widthParam);

    juce::dsp::Reverb::Parameters p;
    p.roomSize = room;
    p.damping = damp;
    p.width = width;
    p.wetLevel = wet;
    p.dryLevel = 1 - wet;
    reverb.setParameters(p);

    reverb.process(context);
}