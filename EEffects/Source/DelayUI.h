#pragma once

#include "EffectUI.h"

class DelayUI : public EffectUI
{
public:
	DelayUI(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(editorIn, vts, effectName, mixParameter, bypassParameter)
	{
		this->addControl("Delay ms", ID::delayTimeMs);
		this->addControl("Feedback", ID::delayFeedback);
	}

	~DelayUI() override = default;
};