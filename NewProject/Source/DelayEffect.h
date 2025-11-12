#pragma once

#include "Effect.h"

class DelayEffect : public IEffect
{
public:
    DelayEffect (juce::AudioProcessorValueTreeState& vts, float maxDelaySeconds = 2.0f) noexcept;
    ~DelayEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    juce::String getName() const override { return "Delay"; }

private:
    const float maxDelaySeconds;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 0 };
    juce::dsp::ProcessSpec lastSpec{};

    // Smoothing
    const float smoothingTimeSeconds = 0.02f;
    juce::SmoothedValue<float> delayTimeSmoothed;     // delay in samples
    juce::SmoothedValue<float> delayFeedbackSmoothed; // feedback 0..1
    juce::SmoothedValue<float> delayWetSmoothed;      // wet 0..1
};