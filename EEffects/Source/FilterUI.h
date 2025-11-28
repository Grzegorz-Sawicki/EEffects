#pragma once

#include "EffectUI.h"

class FilterUI : public EffectUI
{
public:
	FilterUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Cutoff Hz", ID::filterCutoff);
		this->addControl("Resonance", ID::filterResonance);
	}

	~FilterUI() override = default;
};