#include "ReverbUI.h"

ReverbUI::ReverbUI (juce::AudioProcessorValueTreeState& vts)
    : EffectUI (vts)
{
    setSize (kComponentW, kComponentH);

    auto setup = [this] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 20);
        s.setWantsKeyboardFocus (false);
        addAndMakeVisible (s);
    };

    setup (wetSlider);
    setup (roomSlider);
    setup (dampingSlider);
    setup (widthSlider);

    wetLabel.setJustificationType (juce::Justification::centred);
    roomLabel.setJustificationType (juce::Justification::centred);
    dampingLabel.setJustificationType (juce::Justification::centred);
    widthLabel.setJustificationType (juce::Justification::centred);

    wetLabel.setText ("Wet", juce::dontSendNotification);
    roomLabel.setText ("Room", juce::dontSendNotification);
    dampingLabel.setText ("Damp", juce::dontSendNotification);
    widthLabel.setText ("Width", juce::dontSendNotification);

    addAndMakeVisible (wetLabel);
    addAndMakeVisible (roomLabel);
    addAndMakeVisible (dampingLabel);
    addAndMakeVisible (widthLabel);

    bypassButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    bypassButton.setWantsKeyboardFocus (false);
    addAndMakeVisible (bypassButton);

	titleLabel.setText("Reverb", juce::dontSendNotification);
	titleLabel.setWantsKeyboardFocus(false);
	addAndMakeVisible(titleLabel);

    // Attachments
    wetAttach     = std::make_unique<SliderAttachment> (parameters, "reverbWet",     wetSlider);
    roomAttach    = std::make_unique<SliderAttachment> (parameters, "reverbRoom",    roomSlider);
    dampingAttach = std::make_unique<SliderAttachment> (parameters, "reverbDamping", dampingSlider);
    widthAttach   = std::make_unique<SliderAttachment> (parameters, "reverbWidth",   widthSlider);

    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (parameters, "reverbBypass", bypassButton);
}

void ReverbUI::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::transparentBlack);
}

void ReverbUI::resized()
{
    // keep everything fixed-sized and aligned
    auto r = getLocalBounds().reduced (kPadding);

    // bypass on the rightmost side, vertically centered
    int bx = r.getRight() - kBypassW;
    int by = r.getY() + (r.getHeight() - kBypassH) / 2;
    bypassButton.setBounds (bx, by, kBypassW, kBypassH);

    // remaining area for knobs: place to the left of bypass with padding
    int knobsX = r.getX();
    int knobsW = bx - knobsX - kPadding;
    int knobAreaW = juce::jmax (0, knobsW);
    int knobCount = 5;
    int spacing = (knobAreaW - knobCount * kKnobSz) / (knobCount + 1);
    if (spacing < 6) spacing = 6;

    int x = knobsX + spacing;
    int knobY = r.getY() + kPadding; // top margin
    int labelY = knobY + kKnobSz + 4;

    // Set font for labels to fixed size
    juce::Font lblFont ((float) 12.0f, juce::Font::plain);
    wetLabel.setFont (lblFont);
    roomLabel.setFont (lblFont);
    dampingLabel.setFont (lblFont);
    widthLabel.setFont (lblFont);

    titleLabel.setBounds(x, knobY, kKnobSz, kKnobSz);
	x += kKnobSz + spacing;

    wetSlider.setBounds     (x, knobY, kKnobSz, kKnobSz);
    wetLabel.setBounds      (x, labelY, kKnobSz, kLabelH);
    x += kKnobSz + spacing;

    roomSlider.setBounds    (x, knobY, kKnobSz, kKnobSz);
    roomLabel.setBounds     (x, labelY, kKnobSz, kLabelH);
    x += kKnobSz + spacing;

    dampingSlider.setBounds (x, knobY, kKnobSz, kKnobSz);
    dampingLabel.setBounds  (x, labelY, kKnobSz, kLabelH);
    x += kKnobSz + spacing;

    widthSlider.setBounds   (x, knobY, kKnobSz, kKnobSz);
    widthLabel.setBounds    (x, labelY, kKnobSz, kLabelH);
}