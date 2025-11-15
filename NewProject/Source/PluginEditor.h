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

    struct EffectsListUI : juce::Component
    {
        EffectsListUI() {}
        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colours::darkgrey);
        }
    };

    struct EffectsRackUI : juce::Component
    {
        EffectsRackUI() {}
        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colours::grey);
        }
	};

    EffectsListUI effectsListUI;
    EffectsRackUI effectsRackUI;
    BasicEffectsUI basicEffectsUI;

    ReverbUI reverbUI;
	DelayUI  delayUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
