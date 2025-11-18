#pragma once

#include <JuceHeader.h>

class EffectsRackUI : public juce::Component
{
public:
    EffectsRackUI() {}
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::grey);
    }
};