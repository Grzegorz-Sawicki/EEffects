/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Effect.h"
#include "ReverbEffect.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Public APVTS so editor can attach controls
    juce::AudioProcessorValueTreeState parameters;

    // Helper to construct parameter layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    // Gain
    juce::dsp::Gain<float> inputGainProcessor;
	juce::dsp::Gain<float> outputGainProcessor;
    juce::dsp::Panner<float> outputPanProcessor;

    // Default ramp time (seconds)
    const float smoothingTimeSeconds = 0.02f;

    // Reverb
    ReverbEffect reverb;
    //juce::AudioBuffer<float> reverbTempBuffer;

    // Delay using juce::dsp::DelayLine (multi-channel supported internally)
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 0 };
    const float maxDelaySeconds = 2.0f; // maksymalny czas opóŸnienia (sekundy)

    // Smoothed controls for delay (delay time in samples)
    juce::SmoothedValue<float> delayTimeSmoothed;     // value = delay in samples
    juce::SmoothedValue<float> delayFeedbackSmoothed;
    juce::SmoothedValue<float> delayWetSmoothed;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
