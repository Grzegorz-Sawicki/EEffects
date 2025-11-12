/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ============================================================================== 
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    juce::Slider inputGainSlider;
    juce::Slider panSlider;
    juce::Slider outputGainSlider;

    juce::Label inputLabel;
    juce::Label panLabel;
    juce::Label outputLabel;

    juce::Slider reverbWetSlider;
    juce::Slider reverbRoomSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWidthSlider;

    juce::Label reverbWetLabel;
    juce::Label reverbRoomLabel;
    juce::Label reverbDampingLabel;
    juce::Label reverbWidthLabel;

    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Slider delayWetSlider;

    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    juce::Label delayWetLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> inputAttach;
    std::unique_ptr<Attachment> panAttach;
    std::unique_ptr<Attachment> outputAttach;

    std::unique_ptr<Attachment> reverbWetAttach;
    std::unique_ptr<Attachment> reverbRoomAttach;
    std::unique_ptr<Attachment> reverbDampingAttach;
    std::unique_ptr<Attachment> reverbWidthAttach;

    std::unique_ptr<Attachment> delayTimeAttach;
    std::unique_ptr<Attachment> delayFeedbackAttach;
    std::unique_ptr<Attachment> delayWetAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
