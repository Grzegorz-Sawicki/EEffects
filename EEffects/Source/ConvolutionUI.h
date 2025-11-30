#pragma once

#include "EffectUI.h"

class ConvolutionUI : public EffectUI
{
public:
	ConvolutionUI(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) :
		EffectUI(editorIn, vts, effectName, mixParameter, bypassParameter)
	{

	}

	~ConvolutionUI() override = default;
};