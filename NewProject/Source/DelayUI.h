#pragma once

#include "EffectUI.h"

class DelayUI : public EffectUI
{
public:
    DelayUI(juce::AudioProcessorValueTreeState& vts);
    ~DelayUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;


private:
    juce::Slider timeSlider, feedbackSlider, wetSlider;
    juce::Label timeLabel, feedbackLabel, wetLabel;
	juce::ToggleButton bypassButton;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> timeAttach, feedbackAttach, wetAttach;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttach;
};