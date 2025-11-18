#include "BasicEffectsUI.h"

BasicEffectsUI::BasicEffectsUI(juce::AudioProcessorValueTreeState& vts)
{
    auto setupSlider = [this](juce::Slider& s)
        {
            s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            s.setWantsKeyboardFocus(false);
            addAndMakeVisible(s);
        };

    setupSlider(inputSlider);
    setupSlider(panSlider);
    setupSlider(outputSlider);

    inputLabel.setText("Input dB", juce::dontSendNotification);
    inputLabel.attachToComponent(&inputSlider, false);
    addAndMakeVisible(inputLabel);

    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.attachToComponent(&panSlider, false);
    addAndMakeVisible(panLabel);

    outputLabel.setText("Output dB", juce::dontSendNotification);
    outputLabel.attachToComponent(&outputSlider, false);
    addAndMakeVisible(outputLabel);

    inputAttach = std::make_unique<Attachment>(vts, "inputGain", inputSlider);
    panAttach = std::make_unique<Attachment>(vts, "pan", panSlider);
    outputAttach = std::make_unique<Attachment>(vts, "outputGain", outputSlider);
}

void BasicEffectsUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void BasicEffectsUI::resized()
{
    auto r = getLocalBounds().reduced(6);
    int w = r.getWidth() / 3;

    inputSlider.setBounds(r.removeFromLeft(w).reduced(30));
    panSlider.setBounds(r.removeFromLeft(w).reduced(30));
    outputSlider.setBounds(r.reduced(30));
}