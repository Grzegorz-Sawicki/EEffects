#include "DelayUI.h"

DelayUI::DelayUI(juce::AudioProcessorValueTreeState& vts)
    : EffectUI(vts)
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

    timeAttach = std::make_unique<Attachment>(parameters, "delayTimeMs", timeSlider);
    feedbackAttach = std::make_unique<Attachment>(parameters, "delayFeedback", feedbackSlider);
    wetAttach = std::make_unique<Attachment>(parameters, "delayWet", wetSlider);
}

void DelayUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void DelayUI::resized()
{
    auto r = getLocalBounds().reduced(6);
    int w = r.getWidth() / 3;
    timeSlider.setBounds(r.removeFromLeft(w).reduced(6));
    feedbackSlider.setBounds(r.removeFromLeft(w).reduced(6));
    wetSlider.setBounds(r.reduced(6));
}