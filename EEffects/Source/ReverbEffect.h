#pragma once

#include "Effect.h"

class ReverbEffect : public IEffect
{
public:
    ReverbEffect(juce::String name) noexcept;
    ~ReverbEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::ProcessContextNonReplacing<float> context) override; // dostosowane
    void reset() override;

    struct ReverbParameters
    {
        juce::Reverb::Parameters reverbParameters;
        bool bypass = false;
    };

    void setParameters(ReverbParameters& params);

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> tempBuffer;
    juce::dsp::ProcessSpec lastSpec{};

    ReverbParameters parameters;
};