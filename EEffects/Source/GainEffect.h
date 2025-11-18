#pragma once

#include "Effect.h"

class GainEffect : public IEffect
{
public:
    // parameterId e.g. "inputGain" or "outputGain"
    GainEffect (juce::AudioProcessorValueTreeState& vts, juce::String parameterId = "inputGain") noexcept;
    ~GainEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    juce::String getName() const override { return "Gain"; }

private:
    juce::dsp::Gain<float> gainProcessor;
    juce::dsp::ProcessSpec lastSpec{};
    juce::String paramId;

    // fallback smoothing time for dsp::Gain ramp
    const float smoothingTimeSeconds = 0.02f;
};