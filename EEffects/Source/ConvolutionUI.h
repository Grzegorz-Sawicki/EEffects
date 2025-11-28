#pragma once

#include "EffectUI.h"

class ConvolutionUI : public EffectUI
{
public:
	ConvolutionUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(vts, effectName, mixParameter, bypassParameter)
	{

	}

	~ConvolutionUI() override = default;
};