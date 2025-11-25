#pragma once

#include "Effect.h"

class GainEffect : public IEffect
{
public:
    GainEffect (juce::AudioProcessorValueTreeState& vts, juce::String name, juce::String parameterId) noexcept;
    ~GainEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::dsp::ProcessContextNonReplacing<float> context) override;
    void reset() override;

private:
    juce::dsp::Gain<float> gainProcessor;
    juce::dsp::ProcessSpec lastSpec{};
    juce::String paramId;

    const float smoothingTimeSeconds = 0.02f;
};