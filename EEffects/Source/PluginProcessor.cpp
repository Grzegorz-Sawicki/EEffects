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
       outputPan("Output Pan"),
	   filter("Filter")
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
		effects.push_back(&filter);
    }

    syncEffectsInfo();

	changeBroadcaster.sendChangeMessage();

    this->inputGain.reset();
    this->inputGain.prepare(spec);
    this->outputGain.reset();
    this->outputGain.prepare(spec);
    this->outputPan.reset();
    this->outputPan.prepare(spec);

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

    updateParameters();
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
        reverbParams.roomSize = parameters.getRawParameterValue(ID::reverbRoom)->load();
        reverbParams.damping = parameters.getRawParameterValue(ID::reverbDamping)->load();
        reverbParams.width = parameters.getRawParameterValue(ID::reverbWidth)->load();
        reverbParams.wetLevel = parameters.getRawParameterValue(ID::reverbMix)->load();
        reverbParams.dryLevel = 1.0f - reverbParams.wetLevel;
        bool bypass = parameters.getRawParameterValue(ID::reverbBypass)->load() > 0.5f;

        ReverbEffect::ReverbParameters reverbEffectParams{ reverbParams, bypass };
        reverb.setParameters(reverbEffectParams);
    }

    { // Pan parameters
		PanEffect::PanParameters panParams;
		panParams.panParam = parameters.getRawParameterValue(ID::pan)->load();
		outputPan.setParameters(panParams);
    }

    { // Gain parameters
		GainEffect::GainParameters inGainParams;
		inGainParams.gainDb = parameters.getRawParameterValue(ID::inputGain)->load();
		inputGain.setParameters(inGainParams);

		GainEffect::GainParameters outGainParams;
		outGainParams.gainDb = parameters.getRawParameterValue(ID::outputGain)->load();
		outputGain.setParameters(outGainParams);
    }

    { // Delay parameters
		DelayEffect::DelayParameters delayParams;
		delayParams.delayTimeMs = parameters.getRawParameterValue(ID::delayTimeMs)->load();
		delayParams.feedback = parameters.getRawParameterValue(ID::delayFeedback)->load();
		delayParams.wetLevel = parameters.getRawParameterValue(ID::delayMix)->load();
		delayParams.bypass = parameters.getRawParameterValue(ID::delayBypass)->load() > 0.5f;
		delay.setParameters(delayParams);
    }

    { // Filter parameters
        FilterEffect::FilterParameters filterParams;
        filterParams.cutoffFrequency = parameters.getRawParameterValue(ID::filterCutoff)->load();
        filterParams.resonance = parameters.getRawParameterValue(ID::filterResonance)->load();
        filterParams.bypass = parameters.getRawParameterValue(ID::filterBypass)->load() > 0.5f;
		filter.setParameters(filterParams);
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

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::inputGain,  1 }, ID::inputGainName,
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::outputGain, 1 }, ID::outputGainName             ,
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::pan, 1 }, ID::panName,
        juce::NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.0f));

    // Reverb parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::reverbMix, 1 }, ID::reverbMix,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.30f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::reverbRoom, 1 }, ID::reverbRoomName,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.50f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::reverbDamping, 1 }, ID::reverbDampingName,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.50f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::reverbWidth, 1 }, ID::reverbWidthName,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID::reverbBypass, 1 }, ID::reverbBypassName, false));

    // Delay parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::delayTimeMs, 1 }, ID::delayTimeMsName,
        juce::NormalisableRange<float> (1.0f, 2000.0f, 0.1f), 500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::delayFeedback, 1 }, ID::delayFeedbackName,
        juce::NormalisableRange<float> (0.0f, 0.98f, 0.001f), 0.35f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::delayMix, 1 }, ID::delayMixName,
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.25f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID::delayBypass, 1 }, ID::delayBypassName, false));

    //Filter parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ID::filterCutoff, 1 }, ID::filterCutoffName,
		juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), 20000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat> (
		juce::ParameterID{ ID::filterResonance, 1 }, ID::filterResonanceName,
		juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
		juce::ParameterID{ ID::filterBypass, 1 }, ID::filterBypassName, false));

    return { params.begin(), params.end() };
}
