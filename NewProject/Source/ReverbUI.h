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

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> wetAttach, roomAttach, dampingAttach, widthAttach;
};