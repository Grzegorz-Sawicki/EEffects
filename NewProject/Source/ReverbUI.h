#pragma once

#include "EffectUI.h"

class ReverbUI : public EffectUI
{
public:
    ReverbUI (juce::AudioProcessorValueTreeState& vts);
    ~ReverbUI() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

    int preferredHeight() const noexcept override { return kComponentH; }
    int preferredWidth()  const noexcept { return kComponentW; }

private:
    static constexpr int kComponentW = 500;
    static constexpr int kComponentH = 140;
    static constexpr int kBypassW    = 24;
    static constexpr int kBypassH    = 24;
    static constexpr int kKnobSz     = 64;
    static constexpr int kLabelH     = 16;
    static constexpr int kPadding    = 8;

    juce::Slider wetSlider, roomSlider, dampingSlider, widthSlider;
    juce::Label wetLabel, roomLabel, dampingLabel, widthLabel;

    juce::ToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    juce::Label titleLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> wetAttach, roomAttach, dampingAttach, widthAttach;
};