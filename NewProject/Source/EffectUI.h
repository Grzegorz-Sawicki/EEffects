#pragma once

#include <JuceHeader.h>

class EffectUI : public juce::Component
{
public:
    EffectUI(juce::AudioProcessorValueTreeState& vts) noexcept : parameters(vts) {}
    ~EffectUI() override = default;

    virtual int preferredHeight() const noexcept = 0;

protected:
    juce::AudioProcessorValueTreeState& parameters;
};