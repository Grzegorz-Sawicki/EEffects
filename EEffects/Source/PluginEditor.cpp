/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ============================================================================== 
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "EffectInfo.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      basicEffectsUI(p.parameters),
	  effectsListUI(audioProcessor.getEffectsInfo()),
	  effectsRackUI(*this, p.parameters, audioProcessor.getEffectsInfo())
{
    addAndMakeVisible(effectsListUI);
    addAndMakeVisible(effectsRackUI);
    addAndMakeVisible(basicEffectsUI);

    effectsListUI.onToggleChanged = [this] (int idx, bool state)
    {
        audioProcessor.setEffectActive (idx, state);
		updateUIFromProcessor();
    };

    effectsListUI.onRowMoved = [this] (int fromIndex, int toIndex)
    {
        audioProcessor.moveEffect (fromIndex, toIndex);
        updateUIFromProcessor();
    };

    updateUIFromProcessor();
	audioProcessor.addChangeListener(this);

    setSize (1000, 700);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
	audioProcessor.removeChangeListener(this);
}

void NewProjectAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    updateUIFromProcessor();
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void NewProjectAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

	auto basicEffectsBounds = bounds.removeFromBottom(bounds.getHeight() / 4);
	auto effectsListBounds = bounds.removeFromLeft(bounds.getWidth() / 5);
	auto effectsRackBounds = bounds;

	basicEffectsUI.setBounds(basicEffectsBounds);
	effectsListUI.setBounds(effectsListBounds);
	effectsRackUI.setBounds(effectsRackBounds);
}

void NewProjectAudioProcessorEditor::updateUIFromProcessor()
{
    auto effectsInfo = audioProcessor.getEffectsInfo();

    juce::MessageManager::callAsync([this, ei = std::vector<EffectInfo>(effectsInfo)]()
        {
            auto eiCopy = ei;
            effectsListUI.setEffectsInfo(eiCopy);
            effectsRackUI.setEffectsInfo(eiCopy);
        });
}