#include "ReverbEffect.h"

ReverbEffect::ReverbEffect(juce::AudioProcessorValueTreeState& vts) noexcept
    : IEffect(vts)
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

void ReverbEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isActive())
        return;

    const bool bypass = *parameters.getRawParameterValue("reverbBypass");
    if (bypass)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

	const float wetParam = *parameters.getRawParameterValue("reverbWet");
	const float roomParam = *parameters.getRawParameterValue("reverbRoom");
	const float dampParam = *parameters.getRawParameterValue("reverbDamping");
	const float widthParam = *parameters.getRawParameterValue("reverbWidth");

    // read parameters
    const float wet = juce::jlimit(0.0f, 1.0f, wetParam);
    const float room = juce::jlimit(0.0f, 1.0f, roomParam);
    const float damp = juce::jlimit(0.0f, 1.0f, dampParam);
    const float width = juce::jlimit(0.0f, 1.0f, widthParam);

    juce::dsp::Reverb::Parameters p;
    p.roomSize = room;
    p.damping = damp;
    p.width = width;
    p.wetLevel = 1.0f;
    p.dryLevel = 0.0f;
    reverb.setParameters(p);

    // ensure temp buffer size
    if (tempBuffer.getNumChannels() != numChannels || tempBuffer.getNumSamples() < numSamples)
        tempBuffer.setSize(numChannels, numSamples, false, true, true);

    // copy dry -> temp, process reverb on temp, then mix according to wet
    for (int ch = 0; ch < numChannels; ++ch)
        tempBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    {
        juce::dsp::AudioBlock<float> block(const_cast<float**>(tempBuffer.getArrayOfWritePointers()),
            (size_t)numChannels,
            (size_t)numSamples);

        juce::dsp::ProcessContextReplacing<float> ctx(block);
        reverb.process(ctx);
    }

    const float dry = 1.0f - wet;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* dst = buffer.getWritePointer(ch);
        auto* wetp = tempBuffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
            dst[i] = dst[i] * dry + wetp[i] * wet;
    }
}