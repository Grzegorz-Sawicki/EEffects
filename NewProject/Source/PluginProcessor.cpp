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
	   reverb(parameters)
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
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    inputGainProcessor.reset();
    inputGainProcessor.prepare (spec);
    inputGainProcessor.setRampDurationSeconds (smoothingTimeSeconds);
    inputGainProcessor.setGainDecibels (parameters.getRawParameterValue ("inputGain")->load());

	outputGainProcessor.reset();
	outputGainProcessor.prepare(spec);
	outputGainProcessor.setRampDurationSeconds(smoothingTimeSeconds);
	outputGainProcessor.setGainDecibels(parameters.getRawParameterValue("outputGain")->load());

    outputPanProcessor.reset();
	outputPanProcessor.prepare(spec);
	outputPanProcessor.setRule(juce::dsp::Panner<float>::Rule::balanced);
	outputPanProcessor.setPan(parameters.getRawParameterValue("pan")->load());



    delayTimeSmoothed.reset (sampleRate, smoothingTimeSeconds);
    delayFeedbackSmoothed.reset (sampleRate, smoothingTimeSeconds);
    delayWetSmoothed.reset (sampleRate, smoothingTimeSeconds);

    float delayMs     = parameters.getRawParameterValue ("delayTimeMs")->load();
    float delaySamples = delayMs * static_cast<float> (sampleRate) / 1000.0f;
    delayTimeSmoothed.setCurrentAndTargetValue (juce::jlimit (0.0f, maxDelaySeconds * static_cast<float> (sampleRate), delaySamples));

    delayFeedbackSmoothed.setCurrentAndTargetValue (parameters.getRawParameterValue ("delayFeedback")->load());
    delayWetSmoothed.setCurrentAndTargetValue (parameters.getRawParameterValue ("delayWet")->load());

    reverb.reset();
    reverb.prepare (spec);

    // Przygotuj delayLine (maksymalny rozmiar w próbkach) i skonfiguruj go dla kana³ów
    const int maxDelaySamples = static_cast<int> (std::ceil (maxDelaySeconds * sampleRate)) + 2;
    delayLine = decltype(delayLine) (maxDelaySamples); // utwórz DelayLine z maks. rozmiarem
    delayLine.prepare (spec);
    delayLine.reset();
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

// linear interpolation helper (nadal dostêpny, ale DelayLine robi interpolacjê wewnêtrzn¹)
static inline float linearInterpolate (float a, float b, float frac) noexcept
{
    return a + (b - a) * frac;
}

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Pobierz nowe docelowe wartoœci parametrów (atomic safe)
    float inDb         = parameters.getRawParameterValue ("inputGain")->load();
    float outDb        = parameters.getRawParameterValue ("outputGain")->load();
    float pan          = parameters.getRawParameterValue ("pan")->load();

    // Reverb params
    float reverbWet     = parameters.getRawParameterValue ("reverbWet")->load();     // 0..1
    float reverbRoom    = parameters.getRawParameterValue ("reverbRoom")->load();    // 0..1
    float reverbDamping = parameters.getRawParameterValue ("reverbDamping")->load(); // 0..1
    float reverbWidth   = parameters.getRawParameterValue ("reverbWidth")->load();   // 0..1

    // Delay params
    float delayMs       = parameters.getRawParameterValue ("delayTimeMs")->load();    // ms
    float delayFeedback = parameters.getRawParameterValue ("delayFeedback")->load();  // 0..0.98
    float delayWet      = parameters.getRawParameterValue ("delayWet")->load();       // 0..1

    // Delay smoothing targets (delayTime in samples)
    const float sr = static_cast<float> (getSampleRate());
    const float delaySamplesTarget = juce::jlimit (0.0f, maxDelaySeconds * sr, delayMs * sr / 1000.0f);
    delayTimeSmoothed.setTargetValue (delaySamplesTarget);
    delayFeedbackSmoothed.setTargetValue (juce::jlimit (0.0f, 0.99f, delayFeedback));
    delayWetSmoothed.setTargetValue (juce::jlimit (0.0f, 1.0f, delayWet));

    const int numSamples = buffer.getNumSamples();

	inputGainProcessor.setGainDecibels(inDb);

	{
    // utwórz AudioBlock z aktualnych wskaŸników bufora (pewne i bezpoœrednie)
    juce::dsp::AudioBlock<float> gainBlock (const_cast<float**> (buffer.getArrayOfWritePointers()),
                                           static_cast<size_t> (totalNumOutputChannels),
                                           static_cast<size_t> (numSamples));

    juce::dsp::ProcessContextReplacing<float> gainContext (gainBlock);
    inputGainProcessor.process (gainContext);
	}

    // 2) Delay: use juce::dsp::DelayLine (multi-channel). Sequence:
    //    popSample(channel, delayInSamples, true) -> returns delayed sample
    //    compute writeSample = in + delayedSample * feedback
    //    pushSample(channel, writeSample)
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float delaySamples = delayTimeSmoothed.getNextValue();
        float fb = delayFeedbackSmoothed.getNextValue();
        float wet = delayWetSmoothed.getNextValue();
        float dry = 1.0f - wet;

        for (int ch = 0; ch < totalNumOutputChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer (ch);
            float inSample = channelData[sample];

            // Pobierz próbkê opóŸnion¹ (DelayLine interpoluje wewnêtrznie)
            float delayedSample = delayLine.popSample (ch, delaySamples, true);

            // Mix dry + wet
            float outSample = inSample * dry + delayedSample * wet;

            // Feedback write value
            float writeSample = inSample + delayedSample * fb;

            // Zapisz do delay line
            delayLine.pushSample (ch, writeSample);

            // Zapisz wynik
            channelData[sample] = outSample;
        }
    }

    reverb.process(buffer);

    outputGainProcessor.setGainDecibels(outDb);
    {
        juce::dsp::AudioBlock<float> gainBlock(const_cast<float**>(buffer.getArrayOfWritePointers()),
            static_cast<size_t>(totalNumOutputChannels),
            static_cast<size_t>(numSamples));

        juce::dsp::ProcessContextReplacing<float> gainContext(gainBlock);
        outputGainProcessor.process(gainContext);
    }

    outputPanProcessor.setPan(pan);
    {
        juce::dsp::AudioBlock<float> panBlock(const_cast<float**>(buffer.getArrayOfWritePointers()),
            static_cast<size_t>(totalNumOutputChannels),
            static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> panContext(panBlock);
        outputPanProcessor.process(panContext);
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
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    if (state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
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

    return { params.begin(), params.end() };
}
