#pragma once

#include "Effect.h"

class DelayEffect : public IEffect
{
public:
    DelayEffect (juce::AudioProcessorValueTreeState& vts, juce::String name, float maxDelaySeconds = 2.0f) noexcept;
    ~DelayEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::dsp::ProcessContextReplacing<float> context) override;
    void reset() override;

private:
    const float maxDelaySeconds;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 0 };
    juce::dsp::ProcessSpec lastSpec{};

    const float smoothingTimeSeconds = 0.02f;
    juce::SmoothedValue<float> delayTimeSmoothed;     
    juce::SmoothedValue<float> delayFeedbackSmoothed; 
    juce::SmoothedValue<float> delayWetSmoothed;    
};