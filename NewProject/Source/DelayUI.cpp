#include "DelayUI.h"

DelayUI::DelayUI(juce::AudioProcessorValueTreeState& vts)
    : EffectUI(vts, "Delay", "")
{
    auto setup = [this](juce::Slider& s)
        {
            s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            s.setWantsKeyboardFocus(false);
            addAndMakeVisible(s);
        };

    setup(timeSlider);
    setup(feedbackSlider);
    setup(wetSlider);

    timeLabel.setText("Delay ms", juce::dontSendNotification);
    timeLabel.attachToComponent(&timeSlider, false);
    addAndMakeVisible(timeLabel);

    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    addAndMakeVisible(feedbackLabel);

    wetLabel.setText("Delay Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, false);
    addAndMakeVisible(wetLabel);

    bypassButton.setButtonText("Bypass");
    bypassButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
    bypassButton.setWantsKeyboardFocus(false);
    addAndMakeVisible(bypassButton);

    timeAttach = std::make_unique<SliderAttachment>(parameters, "delayTimeMs", timeSlider);
    feedbackAttach = std::make_unique<SliderAttachment>(parameters, "delayFeedback", feedbackSlider);
    wetAttach = std::make_unique<SliderAttachment>(parameters, "delayWet", wetSlider);
	bypassAttach = std::make_unique<ButtonAttachment>(parameters, "delayBypass", bypassButton);
}

void DelayUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void DelayUI::resized()
{
    auto r = getLocalBounds().reduced(6);

    // reserve a small top area for the bypass button
    const int btnH = 24;
    auto topArea = r.removeFromTop(btnH + 4);
    const int btnW = 90;
    bypassButton.setBounds(topArea.removeFromRight(btnW).reduced(2));

    int w = r.getWidth() / 3;
    timeSlider.setBounds(r.removeFromLeft(w).reduced(6));
    feedbackSlider.setBounds(r.removeFromLeft(w).reduced(6));
    wetSlider.setBounds(r.reduced(6));
}