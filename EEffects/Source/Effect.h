#pragma once

#include <JuceHeader.h>

class IEffect
{
public:
    IEffect(juce::String name) noexcept
        : name(std::move(name)) {}

    virtual ~IEffect() = default;

    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::dsp::ProcessContextNonReplacing<float> context) = 0;
    virtual void reset() = 0;

    void setActive(bool a) noexcept { active = a; }
    bool isActive() const noexcept { return active; }
    juce::String getName() const noexcept { return name; }

protected:
    juce::String name;
    bool active = true;
};