/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ============================================================================== 
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto setupSlider = [this] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 20);
        s.setWantsKeyboardFocus (false);
        addAndMakeVisible (s);
    };

    setupSlider (inputGainSlider);
    setupSlider (panSlider);
    setupSlider (outputGainSlider);

    inputLabel.setText ("Input dB", juce::dontSendNotification);
    inputLabel.attachToComponent (&inputGainSlider, false);
    inputLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (inputLabel);

    panLabel.setText ("Pan", juce::dontSendNotification);
    panLabel.attachToComponent (&panSlider, false);
    panLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (panLabel);

    outputLabel.setText ("Output dB", juce::dontSendNotification);
    outputLabel.attachToComponent (&outputGainSlider, false);
    outputLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (outputLabel);
    
    setupSlider (reverbWetSlider);
    setupSlider (reverbRoomSlider);
    setupSlider (reverbDampingSlider);
    setupSlider (reverbWidthSlider);

    reverbWetLabel.setText ("Reverb Wet", juce::dontSendNotification);
    reverbWetLabel.attachToComponent (&reverbWetSlider, false);
    reverbWetLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (reverbWetLabel);

    reverbRoomLabel.setText ("Room Size", juce::dontSendNotification);
    reverbRoomLabel.attachToComponent (&reverbRoomSlider, false);
    reverbRoomLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (reverbRoomLabel);

    reverbDampingLabel.setText ("Damping", juce::dontSendNotification);
    reverbDampingLabel.attachToComponent (&reverbDampingSlider, false);
    reverbDampingLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (reverbDampingLabel);

    reverbWidthLabel.setText ("Width", juce::dontSendNotification);
    reverbWidthLabel.attachToComponent (&reverbWidthSlider, false);
    reverbWidthLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (reverbWidthLabel);

    setupSlider (delayTimeSlider);
    setupSlider (delayFeedbackSlider);
    setupSlider (delayWetSlider);

    delayTimeLabel.setText ("Delay ms", juce::dontSendNotification);
    delayTimeLabel.attachToComponent (&delayTimeSlider, false);
    delayTimeLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (delayTimeLabel);

    delayFeedbackLabel.setText ("Feedback", juce::dontSendNotification);
    delayFeedbackLabel.attachToComponent (&delayFeedbackSlider, false);
    delayFeedbackLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (delayFeedbackLabel);

    delayWetLabel.setText ("Delay Wet", juce::dontSendNotification);
    delayWetLabel.attachToComponent (&delayWetSlider, false);
    delayWetLabel.setWantsKeyboardFocus (false);
    addAndMakeVisible (delayWetLabel);

    inputAttach  = std::make_unique<Attachment> (audioProcessor.parameters, "inputGain",  inputGainSlider);
    panAttach    = std::make_unique<Attachment> (audioProcessor.parameters, "pan",        panSlider);
    outputAttach = std::make_unique<Attachment> (audioProcessor.parameters, "outputGain", outputGainSlider);

    reverbWetAttach     = std::make_unique<Attachment> (audioProcessor.parameters, "reverbWet",     reverbWetSlider);
    reverbRoomAttach    = std::make_unique<Attachment> (audioProcessor.parameters, "reverbRoom",    reverbRoomSlider);
    reverbDampingAttach = std::make_unique<Attachment> (audioProcessor.parameters, "reverbDamping", reverbDampingSlider);
    reverbWidthAttach   = std::make_unique<Attachment> (audioProcessor.parameters, "reverbWidth",   reverbWidthSlider);

    delayTimeAttach     = std::make_unique<Attachment> (audioProcessor.parameters, "delayTimeMs",   delayTimeSlider);
    delayFeedbackAttach = std::make_unique<Attachment> (audioProcessor.parameters, "delayFeedback", delayFeedbackSlider);
    delayWetAttach      = std::make_unique<Attachment> (audioProcessor.parameters, "delayWet",      delayWetSlider);

    setSize (400, 480);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (15.0f));
    g.drawFittedText ("Input / Pan / Output / Reverb / Delay", getLocalBounds().removeFromTop(28), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);

    int rowHeight = area.getHeight() / 3;
    auto topRow = area.removeFromTop (rowHeight);
    auto midRow = area.removeFromTop (rowHeight);
    auto bottomRow = area;

    int wTop = topRow.getWidth() / 3;
    inputGainSlider.setBounds (topRow.removeFromLeft (wTop).reduced (8));
    panSlider.setBounds (topRow.removeFromLeft (wTop).reduced (8));
    outputGainSlider.setBounds (topRow.reduced (8));

    int wMid = midRow.getWidth() / 4;
    reverbWetSlider.setBounds     (midRow.removeFromLeft (wMid).reduced (8));
    reverbRoomSlider.setBounds    (midRow.removeFromLeft (wMid).reduced (8));
    reverbDampingSlider.setBounds (midRow.removeFromLeft (wMid).reduced (8));
    reverbWidthSlider.setBounds   (midRow.reduced (8));

    int wBot = bottomRow.getWidth() / 3;
    delayTimeSlider.setBounds     (bottomRow.removeFromLeft (wBot).reduced (8));
    delayFeedbackSlider.setBounds (bottomRow.removeFromLeft (wBot).reduced (8));
    delayWetSlider.setBounds      (bottomRow.reduced (8));
}
