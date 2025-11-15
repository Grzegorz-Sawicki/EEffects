#include "ReverbUI.h"

ReverbUI::ReverbUI (juce::AudioProcessorValueTreeState& vts)
    : EffectUI (vts, "Reverb", "")
{
    setSize (kComponentW, kComponentH);

    addAndMakeVisible(nameComponent);
	addAndMakeVisible(controlsComponent);
	addAndMakeVisible(bypassComponent);
}

void ReverbUI::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::transparentBlack);
}

void ReverbUI::resized()
{
    auto bounds = getLocalBounds();
    nameComponent.setBounds(bounds.removeFromLeft(bounds.getWidth() / 5));
	bypassComponent.setBounds(bounds.removeFromRight(bounds.getWidth() / 10));
	controlsComponent.setBounds(bounds);
}