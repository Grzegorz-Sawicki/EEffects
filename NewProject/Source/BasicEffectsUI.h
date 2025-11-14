#pragma once

#include "EffectUI.h"

class BasicEffectsUI : public EffectUI
{
public:
    BasicEffectsUI(juce::AudioProcessorValueTreeState& vts);
    ~BasicEffectsUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    int preferredHeight() const noexcept override { return 120; }

private:
    juce::Slider inputSlider;
    juce::Slider panSlider;
    juce::Slider outputSlider;

    juce::Label inputLabel;
    juce::Label panLabel;
    juce::Label outputLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> inputAttach;
    std::unique_ptr<Attachment> panAttach;
    std::unique_ptr<Attachment> outputAttach;
};