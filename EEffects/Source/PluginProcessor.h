/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Effect.h"
#include "ReverbEffect.h"
#include "DelayEffect.h"
#include "GainEffect.h"
#include "PanEffect.h"
#include "EffectInfo.h"

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

    // UI / editor helpers for chain management (thread-safe)
    int getNumEffects() const noexcept;
    juce::String getEffectName (int index) const noexcept;
    bool isEffectActive (int index) const noexcept;
    void setEffectActive (int index, bool active) noexcept;
    void moveEffect (int fromIndex, int toIndex) noexcept;

    std::vector<EffectInfo> getEffectsInfo() noexcept;
	void syncEffectsInfo() noexcept;

    void addChangeListener(juce::ChangeListener* listener) { changeBroadcaster.addChangeListener(listener); }
    void removeChangeListener(juce::ChangeListener* listener) { changeBroadcaster.removeChangeListener(listener); }


private:
    const float smoothingTimeSeconds = 0.02f;

	std::vector<IEffect*> effects;
    std::vector<EffectInfo> effectsInfo;

    GainEffect inputGain;
	GainEffect outputGain;
    PanEffect outputPan;
    ReverbEffect reverb;
    DelayEffect delay;

    void processEffect(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& tempBuffer, IEffect& effect);

    // lock protecting effects vector for brief GUI modifications
    mutable juce::SpinLock effectsLock;

    juce::ChangeBroadcaster changeBroadcaster;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
