#pragma once

#include "Effect.h"

class ReverbEffect : public IEffect
{
public:
    ReverbEffect(juce::AudioProcessorValueTreeState& vts, juce::String name) noexcept;
    ~ReverbEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::ProcessContextNonReplacing<float> context) override; // dostosowane
    void reset() override;

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> tempBuffer;
    juce::dsp::ProcessSpec lastSpec{};
};