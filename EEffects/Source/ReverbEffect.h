#pragma once

#include "Effect.h"

class ReverbEffect : public IEffect
{
public:
    ReverbEffect(juce::AudioProcessorValueTreeState& vts) noexcept;
    ~ReverbEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    juce::String getName() const override { return "Reverb"; }

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> tempBuffer;
    juce::dsp::ProcessSpec lastSpec{};
};