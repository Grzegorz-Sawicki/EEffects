/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ============================================================================== 
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BasicEffectsUI.h"
#include "ReverbUI.h"
#include "DelayUI.h"
#include "EffectsListUI.h"
#include "EffectsRackUI.h"
#include <functional>
#include <vector>
#include <memory>

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

    EffectsListUI effectsListUI;
    EffectsRackUI effectsRackUI;
    BasicEffectsUI basicEffectsUI;

    ReverbUI reverbUI;
    DelayUI  delayUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
