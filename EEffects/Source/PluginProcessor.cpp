/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ============================================================================== 
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Konstruktor inicjalizuje APVTS
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr, "PARAMETERS", createParameterLayout()),
       reverb("Reverb"),
       delay("Delay", 2.0f),
       inputGain("Input Gain"),
       outputGain("Output Gain"),
       outputPan("Output Pan")
#endif
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================

const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
// Chain management helpers
int NewProjectAudioProcessor::getNumEffects() const noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);
    return static_cast<int> (effects.size());
}

juce::String NewProjectAudioProcessor::getEffectName (int index) const noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);
    if (index >= 0 && index < (int) effects.size() && effects[index] != nullptr)
        return effects[index]->getName();
    return "";
}

bool NewProjectAudioProcessor::isEffectActive (int index) const noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);
    if (index >= 0 && index < (int) effects.size() && effects[index] != nullptr)
        return effects[index]->isActive();
    return false;
}

void NewProjectAudioProcessor::setEffectActive (int index, bool active) noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);

    if (index >= 0 && index < (int)effectsInfo.size())
    {
        effectsInfo[index].active = active;

        if (index >= 0 && index < (int)effects.size() && effects[index] != nullptr)
            effects[index]->setActive(active);
    }
}

void NewProjectAudioProcessor::moveEffect (int fromIndex, int toIndex) noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);

    if (fromIndex == toIndex || fromIndex < 0 || toIndex < 0)
        return;

    const int n = static_cast<int> (effects.size());
    if (fromIndex >= n || toIndex >= n)
        return;

	auto info = std::move(effectsInfo[fromIndex]);
    effectsInfo.erase(effectsInfo.begin() + fromIndex);
    effectsInfo.insert(effectsInfo.begin() + toIndex, std::move(info));

    auto ptr = effects[fromIndex];
    effects.erase (effects.begin() + fromIndex);
    effects.insert (effects.begin() + toIndex, ptr);
}

std::vector<EffectInfo> NewProjectAudioProcessor::getEffectsInfo() noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);
    return effectsInfo;
}

void NewProjectAudioProcessor::syncEffectsInfo() noexcept
{
    const juce::SpinLock::ScopedLockType sl (effectsLock);
    effectsInfo.clear();
	effectsInfo.reserve(effects.size());

    for (auto* eff : effects)
    {
        if (eff)
            effectsInfo.push_back(EffectInfo(eff->getName(), eff->isActive()));
    }
}

//==============================================================================

void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    {
        const juce::SpinLock::ScopedLockType sl (effectsLock);
        effects.clear();
        effects.push_back(&reverb);
        effects.push_back(&delay);
    }

    syncEffectsInfo();

    this->inputGain.reset();
    this->inputGain.prepare(spec);
    this->outputGain.reset();
    this->outputGain.prepare(spec);
    this->outputPan.reset();
    this->outputPan.prepare(spec);

    updateParameters();

    std::vector<IEffect*> localCopy;
    {
        const juce::SpinLock::ScopedLockType sl (effectsLock);
        localCopy = effects;
    }

    for (auto* eff : localCopy)
    {
        if (eff)
        {
            eff->reset();
            eff->prepare(spec);
        }
    }
}

void NewProjectAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    std::vector<IEffect*> localCopy;
    {
        const juce::SpinLock::ScopedLockType sl (effectsLock);
        localCopy = effects;
    }

	updateParameters();

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(numChannels, numSamples, false, true, true);

	processEffect(buffer, tempBuffer, inputGain);

    for (auto* eff : localCopy)
    {
        if (eff == nullptr || ! eff->isActive())
            continue;

		processEffect(buffer, tempBuffer, *eff);
    }

	processEffect(buffer, tempBuffer, outputGain);
	processEffect(buffer, tempBuffer, outputPan);
}

void NewProjectAudioProcessor::processEffect(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& tempBuffer, IEffect& effect) {
	const int numChannels = buffer.getNumChannels();
	const int numSamples = buffer.getNumSamples();

    tempBuffer.clear();

    juce::dsp::AudioBlock<float> inputBlock(const_cast<float**>(buffer.getArrayOfWritePointers()),
        static_cast<size_t>(numChannels),
        static_cast<size_t>(numSamples));
    juce::dsp::AudioBlock<float> outputBlock(tempBuffer.getArrayOfWritePointers(),
        static_cast<size_t>(numChannels),
        static_cast<size_t>(numSamples));

    juce::dsp::ProcessContextNonReplacing<float> ctx(inputBlock, outputBlock);

    effect.process(ctx);
    for (int ch = 0; ch < numChannels; ++ch)
        buffer.copyFrom(ch, 0, tempBuffer, ch, 0, numSamples);
}

void NewProjectAudioProcessor::updateParameters() {
    { //Reverb parameters
        juce::Reverb::Parameters reverbParams;
        reverbParams.roomSize = parameters.getRawParameterValue("reverbRoom")->load();
        reverbParams.damping = parameters.getRawParameterValue("reverbDamping")->load();
        reverbParams.width = parameters.getRawParameterValue("reverbWidth")->load();
        reverbParams.wetLevel = parameters.getRawParameterValue("reverbWet")->load();
        reverbParams.dryLevel = 1.0f - reverbParams.wetLevel;
        bool bypass = parameters.getRawParameterValue("reverbBypass")->load() > 0.5f;

        ReverbEffect::ReverbParameters reverbEffectParams{ reverbParams, bypass };
        reverb.setParameters(reverbEffectParams);
    }

    { // Pan parameters
		PanEffect::PanParameters panParams;
		panParams.panParam = parameters.getRawParameterValue("pan")->load();
		outputPan.setParameters(panParams);
    }

    { // Gain parameters
		GainEffect::GainParameters inGainParams;
		inGainParams.gainDb = parameters.getRawParameterValue("inputGain")->load();
		inputGain.setParameters(inGainParams);

		GainEffect::GainParameters outGainParams;
		outGainParams.gainDb = parameters.getRawParameterValue("outputGain")->load();
		outputGain.setParameters(outGainParams);
    }

    { // Delay parameters
		DelayEffect::DelayParameters delayParams;
		delayParams.delayTimeMs = parameters.getRawParameterValue("delayTimeMs")->load();
		delayParams.feedback = parameters.getRawParameterValue("delayFeedback")->load();
		delayParams.wetLevel = parameters.getRawParameterValue("delayWet")->load();
		delayParams.bypass = parameters.getRawParameterValue("delayBypass")->load() > 0.5f;
		delay.setParameters(delayParams);
    }
}

//==============================================================================

bool NewProjectAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
// State management
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    syncEffectsInfo();

    auto state = parameters.copyState();
    if (state.isValid())
    {
        std::unique_ptr<juce::XmlElement> paramsXml (state.createXml());

        std::unique_ptr<juce::XmlElement> wrapper (new juce::XmlElement ("PLUGIN_STATE"));
        wrapper->addChildElement (paramsXml.release());

        juce::XmlElement* effectsXml = new juce::XmlElement ("EffectsOrder");
        for (const auto& info : effectsInfo)
        {
            juce::XmlElement* e = new juce::XmlElement ("Effect");
            e->setAttribute ("name", info.name);
            e->setAttribute ("active", info.active ? 1 : 0);
            effectsXml->addChildElement (e);
        }
        wrapper->addChildElement (effectsXml);

        copyXmlToBinary (*wrapper, destData);
    }
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState == nullptr)
        return;

    juce::XmlElement* paramsXml = xmlState->getChildByName ("PARAMETERS");
    if (paramsXml == nullptr)
    {
        if (xmlState->hasTagName ("PARAMETERS"))
            paramsXml = xmlState.get();
    }

    if (paramsXml != nullptr)
    {
        parameters.replaceState (juce::ValueTree::fromXml (*paramsXml));
    }

    juce::XmlElement* effectsXml = xmlState->getChildByName ("EffectsOrder");
    if (effectsXml != nullptr)
    {
        std::vector<EffectInfo> requested;
        for (auto* child = effectsXml->getFirstChildElement(); child != nullptr; child = child->getNextElement())
        {
            if (child->hasTagName ("Effect"))
            {
                juce::String name = child->getStringAttribute ("name", {});
                bool active = child->getBoolAttribute ("active", false);
                if (name.isNotEmpty())
                    requested.emplace_back (name, active);
            }
        }

        const juce::SpinLock::ScopedLockType sl (effectsLock);

        std::vector<IEffect*> newEffects;
        newEffects.reserve (requested.size());

        for (const auto& info : requested)
        {
            auto it = std::find_if (effects.begin(), effects.end(), [&] (IEffect* e) { return e != nullptr && e->getName() == info.name; });
            if (it != effects.end())
            {
                newEffects.push_back (*it);
            }
            else
            {
                
            }
        }

        for (auto* e : effects)
            if (std::find (newEffects.begin(), newEffects.end(), e) == newEffects.end())
                newEffects.push_back (e);

        effects = std::move (newEffects);

        effectsInfo.clear();
        effectsInfo.reserve (effects.size());
        for (auto* e : effects)
        {
            bool active = true;
            auto itReq = std::find_if (requested.begin(), requested.end(), [&] (const EffectInfo& ii) { return ii.name == e->getName(); });
            if (itReq != requested.end())
                active = itReq->active;

            effectsInfo.emplace_back (e->getName(), active);
        }

        for (size_t i = 0; i < effects.size(); ++i)
        {
            if (effects[i] != nullptr)
                effects[i]->setActive (effectsInfo[i].active);
        }

        changeBroadcaster.sendChangeMessage();
    }
    else
    {
        syncEffectsInfo();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

// Static helper: parameter layout
juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "inputGain",  1 }, "Input Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "outputGain", 1 }, "Output Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "pan", 1 }, "Pan",
        juce::NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.0f));

    // Reverb parameters
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverbWet", 1 }, "Reverb Wet",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.30f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverbRoom", 1 }, "Reverb Room Size",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.50f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverbDamping", 1 }, "Reverb Damping",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.50f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverbWidth", 1 }, "Reverb Width",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "reverbBypass", 1 }, "Reverb Bypass", false));

    // Delay parameters
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delayTimeMs", 1 }, "Delay Time (ms)",
        juce::NormalisableRange<float> (1.0f, 2000.0f, 0.1f), 500.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delayFeedback", 1 }, "Delay Feedback",
        juce::NormalisableRange<float> (0.0f, 0.98f, 0.001f), 0.35f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delayWet", 1 }, "Delay Wet",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.25f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "delayBypass", 1 }, "Delay Bypass", false));

    return { params.begin(), params.end() };
}
