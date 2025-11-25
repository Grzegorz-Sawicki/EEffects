#pragma once

#include "Effect.h"

class GainEffect : public IEffect
{
public:
    GainEffect (juce::String name) noexcept;
    ~GainEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::dsp::ProcessContextNonReplacing<float> context) override;
    void reset() override;

    struct GainParameters
    {
        float gainDb = 0.0f; // in decibels
	};

	void setParameters(const GainParameters& params);

private:
    juce::dsp::Gain<float> gainProcessor;
    juce::dsp::ProcessSpec lastSpec{};

    const float smoothingTimeSeconds = 0.02f;
	GainParameters parameters;
};