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
      reverbUI(p.parameters, "Reverb", "reverbBypass"),
      delayUI(p.parameters, "Delay", "delayBypass"),
      basicEffectsUI(p.parameters),
	  effectsListUI(audioProcessor.getEffectsInfo()),
	  effectsRackUI(audioProcessor.getEffectsInfo())
{
    addAndMakeVisible(effectsListUI);
    addAndMakeVisible(effectsRackUI);
    addAndMakeVisible(basicEffectsUI);

    auto refreshEffectsList = [this]()
    {
		auto effectsInfo = audioProcessor.getEffectsInfo();

        juce::MessageManager::callAsync ([this, ei = std::vector<EffectInfo>(effectsInfo)]()
        {
            auto eiCopy = ei;
            effectsListUI.setEffectsInfo(eiCopy);
			effectsRackUI.setEffectsInfo(eiCopy);
        });
    };

    effectsListUI.onToggleChanged = [this] (int idx, bool state)
    {
        audioProcessor.setEffectActive (idx, state);
    };

    effectsListUI.onRowMoved = [this, refreshEffectsList] (int fromIndex, int toIndex)
    {
        audioProcessor.moveEffect (fromIndex, toIndex);
		//effectsRackUI.moveEffectUI(fromIndex, toIndex);
        refreshEffectsList();
    };

    refreshEffectsList();
	effectUIs.push_back(&reverbUI);
	effectUIs.push_back(&delayUI);
	effectsRackUI.setEffectUIs(effectUIs);

    setSize (1000, 700);
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
    auto bounds = getLocalBounds();

	auto basicEffectsBounds = bounds.removeFromBottom(bounds.getHeight() / 4);
	auto effectsListBounds = bounds.removeFromLeft(bounds.getWidth() / 5);
	auto effectsRackBounds = bounds;

	basicEffectsUI.setBounds(basicEffectsBounds);
	effectsListUI.setBounds(effectsListBounds);
	effectsRackUI.setBounds(effectsRackBounds);
}
