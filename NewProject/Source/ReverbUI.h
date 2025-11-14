#pragma once

#include "EffectUI.h"

class ReverbUI : public EffectUI
{
public:
    ReverbUI(juce::AudioProcessorValueTreeState& vts);
    ~ReverbUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    int preferredHeight() const noexcept override { return 140; }

private:
    juce::Slider wetSlider, roomSlider, dampingSlider, widthSlider;
    juce::Label wetLabel, roomLabel, dampingLabel, widthLabel;
	juce::ToggleButton bypassButton;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> wetAttach, roomAttach, dampingAttach, widthAttach;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
	std::unique_ptr<ButtonAttachment> bypassAttach;
};