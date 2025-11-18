#pragma once

#include <JuceHeader.h>

class IEffect
{
public:
    IEffect(juce::AudioProcessorValueTreeState& vts) noexcept : parameters(vts) {}
    virtual ~IEffect() = default;

    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;

    void setActive(bool a) noexcept { active = a; }
    bool isActive() const noexcept { return active; }

    virtual juce::String getName() const = 0;

protected:
    juce::AudioProcessorValueTreeState& parameters;
    bool active = true;
};