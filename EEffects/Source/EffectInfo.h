#pragma once

#include <JuceHeader.h>

class EffectInfo
{
public:
    juce::String name;
    bool active;

    EffectInfo() = default;
    EffectInfo(const juce::String& n, const bool a) : name(n), active(a) {}
};
