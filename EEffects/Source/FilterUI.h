#pragma once

#include "EffectUI.h"

class FilterUI : public EffectUI
{
public:
	FilterUI(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(editorIn, vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Cutoff Hz", ID::filterCutoff);
		this->addControl("Resonance", ID::filterResonance);
	}

	~FilterUI() override = default;
};