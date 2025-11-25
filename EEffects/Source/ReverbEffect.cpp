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

void ReverbEffect::process(juce::dsp::ProcessContextReplacing<float> context)
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
    p.wetLevel = 1.0f;
    p.dryLevel = 0.0f;
    reverb.setParameters(p);

    auto inputBlock = context.getInputBlock();
    const size_t numChannels = inputBlock.getNumChannels();
    const size_t numSamples  = inputBlock.getNumSamples();

    if (tempBuffer.getNumChannels() != (int)numChannels || tempBuffer.getNumSamples() < (int)numSamples)
        tempBuffer.setSize((int)numChannels, (int)numSamples, false, true, true);

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        const float* src = inputBlock.getChannelPointer(ch);
        tempBuffer.copyFrom((int)ch, 0, src, (int)numSamples);
    }

    juce::dsp::AudioBlock<float> tempBlock(const_cast<float**>(tempBuffer.getArrayOfWritePointers()),
                                           numChannels, numSamples);
    juce::dsp::ProcessContextReplacing<float> tempCtx(tempBlock);
    reverb.process(tempCtx);

    const float dry = 1.0f - wet;
    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        float* dst = context.getOutputBlock().getChannelPointer(ch);
        const float* wetp = tempBuffer.getReadPointer((int)ch);
        for (size_t i = 0; i < numSamples; ++i)
            dst[i] = dst[i] * dry + wetp[i] * wet;
    }
}