#pragma once

#include "Effect.h"

class PanEffect : public IEffect
{
public:
    PanEffect (juce::AudioProcessorValueTreeState& vts, juce::String name, juce::String parameterId = "pan") noexcept;
    ~PanEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::AudioBuffer<float>& buffer) override;
    void reset() override;

private:
    juce::dsp::ProcessSpec lastSpec{};
    const float smoothingTimeSeconds = 0.02f;
    juce::SmoothedValue<float> panSmoothed; // -1..1
    juce::String paramId;
};