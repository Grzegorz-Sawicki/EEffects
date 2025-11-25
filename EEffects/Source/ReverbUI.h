#pragma once

#include "EffectUI.h"

class ReverbUI : public EffectUI
{
public:
	ReverbUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Room", "reverbRoom");
		this->addControl("Damp", "reverbDamping");
		this->addControl("Width", "reverbWidth");
	}

	~ReverbUI() override = default;
};