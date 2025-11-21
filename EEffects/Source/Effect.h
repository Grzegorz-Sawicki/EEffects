#pragma once

#include <JuceHeader.h>

class IEffect
{
public:
    IEffect(juce::AudioProcessorValueTreeState& vts, juce::String name) noexcept : 
        parameters(vts),
		name(name)
    {}

    virtual ~IEffect() = default;

    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;

    void setActive(bool a) noexcept { active = a; }
    bool isActive() const noexcept { return active; }

    virtual juce::String getName() { return name; }

protected:
    juce::AudioProcessorValueTreeState& parameters;
    bool active = true;
    juce::String name = "";
};