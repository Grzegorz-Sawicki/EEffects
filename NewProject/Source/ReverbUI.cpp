#include "ReverbUI.h"

ReverbUI::ReverbUI(juce::AudioProcessorValueTreeState& vts)
    : EffectUI(vts)
{
    auto setup = [this](juce::Slider& s)
        {
            s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            s.setWantsKeyboardFocus(false);
            addAndMakeVisible(s);
        };

    setup(wetSlider);
    setup(roomSlider);
    setup(dampingSlider);
    setup(widthSlider);

    wetLabel.setText("Reverb Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, false);
    addAndMakeVisible(wetLabel);

    roomLabel.setText("Room Size", juce::dontSendNotification);
    roomLabel.attachToComponent(&roomSlider, false);
    addAndMakeVisible(roomLabel);

    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.attachToComponent(&dampingSlider, false);
    addAndMakeVisible(dampingLabel);

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.attachToComponent(&widthSlider, false);
    addAndMakeVisible(widthLabel);

    wetAttach = std::make_unique<Attachment>(parameters, "reverbWet", wetSlider);
    roomAttach = std::make_unique<Attachment>(parameters, "reverbRoom", roomSlider);
    dampingAttach = std::make_unique<Attachment>(parameters, "reverbDamping", dampingSlider);
    widthAttach = std::make_unique<Attachment>(parameters, "reverbWidth", widthSlider);
}

void ReverbUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void ReverbUI::resized()
{
    auto r = getLocalBounds().reduced(6);
    int w = r.getWidth() / 4;
    wetSlider.setBounds(r.removeFromLeft(w).reduced(6));
    roomSlider.setBounds(r.removeFromLeft(w).reduced(6));
    dampingSlider.setBounds(r.removeFromLeft(w).reduced(6));
    widthSlider.setBounds(r.reduced(6));
}