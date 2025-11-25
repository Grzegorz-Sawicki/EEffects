#pragma once

#include "Effect.h"

class PanEffect : public IEffect
{
public:
    PanEffect (juce::String name) noexcept;
    ~PanEffect() override = default;

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void process (juce::dsp::ProcessContextNonReplacing<float> context) override;
    void reset() override;

    struct PanParameters
    {
		float panParam = 0.0f; // -1..1
    };

	void setParameters(const PanParameters& params);

private:
    juce::dsp::ProcessSpec lastSpec{};
    const float smoothingTimeSeconds = 0.02f;
    juce::SmoothedValue<float> panSmoothed;

	PanParameters parameters;
};