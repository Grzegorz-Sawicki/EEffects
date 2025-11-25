#pragma once

#include "Effect.h"

class DelayEffect : public IEffect
{
public:
    DelayEffect (juce::String name, float maxDelaySeconds = 2.0f) noexcept;
    ~DelayEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::dsp::ProcessContextNonReplacing<float> context) override;
    void reset() override;

    struct DelayParameters
    {
        float delayTimeMs = 1000.0f; 
        float feedback = 0.5f;        
        float wetLevel = 0.5f;   
		bool bypass = false;
	};

	void setParameters(const DelayParameters& params);

private:
    const float maxDelaySeconds;
	const int sampleRate = 44100;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 0 };
    juce::dsp::ProcessSpec lastSpec{};

    const float smoothingTimeSeconds = 0.02f;
    juce::SmoothedValue<float> delayTimeSmoothed;     
    juce::SmoothedValue<float> delayFeedbackSmoothed; 
    juce::SmoothedValue<float> delayWetSmoothed;  

    DelayParameters parameters;
};