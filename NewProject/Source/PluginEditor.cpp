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
	reverbUI(p.parameters, "Test", "reverbBypass"),
	delayUI(p.parameters, "Delay", "delayBypass")
   //   reverbUI (p.parameters),
   //   delayUI (p.parameters),
	  //basicEffectsUI(p.parameters)
{
	//addAndMakeVisible(basicEffectsUI);
 //   addAndMakeVisible(reverbUI);
 //   addAndMakeVisible(delayUI);

	addAndMakeVisible(reverbUI);
	addAndMakeVisible(delayUI);

    setSize (500, 500);
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

	reverbUI.setBounds(topRow.reduced(6));
	delayUI.setBounds(midRow.reduced(6));
}
