/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ============================================================================== 
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      reverbUI(p.parameters, "Reverb", "reverbBypass"),
      delayUI(p.parameters, "Delay", "delayBypass"),
      basicEffectsUI(p.parameters)
{
    addAndMakeVisible(effectsListUI);
    addAndMakeVisible(effectsRackUI);
    effectsRackUI.addAndMakeVisible(reverbUI);
    effectsRackUI.addAndMakeVisible(delayUI);
    addAndMakeVisible(basicEffectsUI);

    auto refreshEffectsList = [this]()
    {
        std::vector<juce::String> names;
        std::vector<bool> actives;

        const int n = audioProcessor.getNumEffects();
        names.reserve(n);
        actives.reserve(n);

        for (int i = 0; i < n; ++i)
        {
            names.push_back (audioProcessor.getEffectName (i));
            actives.push_back (audioProcessor.isEffectActive (i));
        }

        juce::MessageManager::callAsync ([this, names = std::move(names), actives = std::move(actives)]()
        {
            effectsListUI.setEffects (names, &actives);
        });
    };

    effectsListUI.onToggleChanged = [this] (int idx, bool state)
    {
        audioProcessor.setEffectActive (idx, state);
    };

    effectsListUI.onRowMoved = [this, refreshEffectsList] (int fromIndex, int toIndex)
    {
        audioProcessor.moveEffect (fromIndex, toIndex);
        refreshEffectsList();
    };

    refreshEffectsList();

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

	auto rackLocal = effectsRackUI.getLocalBounds();

	reverbUI.setBounds(rackLocal.removeFromTop(140));

	delayUI.setBounds(rackLocal.removeFromTop(140));
}
