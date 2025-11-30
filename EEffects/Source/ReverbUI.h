#pragma once

#include "EffectUI.h"

class ReverbUI : public EffectUI
{
public:
	ReverbUI(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(editorIn, vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Room", ID::reverbRoom);
		this->addControl("Damp", ID::reverbDamping);
		this->addControl("Width", ID::reverbWidth);
	}

	~ReverbUI() override = default;
};