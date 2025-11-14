#pragma once

#include "EffectUI.h"

class DelayUI : public EffectUI
{
public:
    DelayUI(juce::AudioProcessorValueTreeState& vts);
    ~DelayUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    int preferredHeight() const noexcept override { return 120; }

private:
    juce::Slider timeSlider, feedbackSlider, wetSlider;
    juce::Label timeLabel, feedbackLabel, wetLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> timeAttach, feedbackAttach, wetAttach;
};