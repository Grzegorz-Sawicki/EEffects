#pragma once

#include "EffectUI.h"

class DelayUI : public EffectUI
{
public:
	DelayUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String bypassParameter) :
		EffectUI(vts, effectName, bypassParameter)
	{
		this->addControl("Delay ms", "delayTimeMs");
		this->addControl("Feedback", "delayFeedback");
		this->addControl("Delay Wet", "delayWet");
	}

	~DelayUI() override = default;
};