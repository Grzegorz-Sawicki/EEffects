#pragma once

#include "EffectUI.h"

class DelayUI : public EffectUI
{
public:
	DelayUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Delay ms", ID::delayTimeMs);
		this->addControl("Feedback", ID::delayFeedback);
	}

	~DelayUI() override = default;
};