/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
ByterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // fixed bit depth
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "bitDepth", "Bit Depth",
        juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f), 8.0f));

    // sample rate modulation range
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "srMin", "SR Min",
        juce::NormalisableRange<float>(1000.0f, 44100.0f, 1.0f), 2000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "srMax", "SR Max",
        juce::NormalisableRange<float>(1000.0f, 44100.0f, 1.0f), 44100.0f));

    // modulation
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modAmount", "Mod Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modDelay", "Mod Delay",
        juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modDecay", "Mod Decay",
        juce::NormalisableRange<float>(10.0f, 5000.0f, 1.0f), 500.0f));

    // chance: probability that a random speed boost fires on each onset
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "chance", "Chance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // modSpeed: max speed multiplier applied when chance fires (1x = no boost, 10x = very fast)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modSpeed", "Mod Speed",
        juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "onsetThreshold", "Onset Threshold",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "modDirection", "Mod Direction", false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return layout;
}

ByterAudioProcessor::ByterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       audioValueTree(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

ByterAudioProcessor::~ByterAudioProcessor()
{
}

//==============================================================================
const juce::String ByterAudioProcessor::getName() const { return JucePlugin_Name; }

bool ByterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ByterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ByterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ByterAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int    ByterAudioProcessor::getNumPrograms()             { return 1; }
int    ByterAudioProcessor::getCurrentProgram()          { return 0; }
void   ByterAudioProcessor::setCurrentProgram (int)      {}
const juce::String ByterAudioProcessor::getProgramName (int) { return {}; }
void   ByterAudioProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================
void ByterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;

    heldSample.fill(0.0f);
    counter.fill(0.0f);
    envFollower.fill(0.0f);
    prevEnv.fill(0.0f);
    modEnvelope.fill(0.0f);
    delayCounter.fill(0);
    tailActive.fill(false);
    currentSpeedMult.fill(1.0f);
}

void ByterAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ByterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void ByterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // read params once per block
    bool  bypass         = audioValueTree.getRawParameterValue("bypass")->load();
    float bitDepth       = audioValueTree.getRawParameterValue("bitDepth")->load();
    float srMin          = audioValueTree.getRawParameterValue("srMin")->load();
    float srMax          = audioValueTree.getRawParameterValue("srMax")->load();
    float modAmount      = audioValueTree.getRawParameterValue("modAmount")->load();
    float modDelayMs     = audioValueTree.getRawParameterValue("modDelay")->load();
    float modDecayMs     = audioValueTree.getRawParameterValue("modDecay")->load();
    float chance         = audioValueTree.getRawParameterValue("chance")->load();
    float modSpeed       = audioValueTree.getRawParameterValue("modSpeed")->load();
    float onsetThreshold = audioValueTree.getRawParameterValue("onsetThreshold")->load();
    bool  modDirection   = audioValueTree.getRawParameterValue("modDirection")->load();

    if (bypass) return;

    float envReleaseCoeff  = std::exp(-1.0f / (0.1f * (float)sampleRate));
    int   delayTimeSamples = (int)(modDelayMs * 0.001f * (float)sampleRate);

    // bit depth is fixed
    float steps    = std::pow(2.0f, bitDepth);
    float stepSize = 2.0f / steps;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            // envelope follower
            float inputAbs = std::abs(channelData[n]);
            if (inputAbs > envFollower[channel])
                envFollower[channel] = inputAbs;
            else
                envFollower[channel] *= envReleaseCoeff;

            // onset detection
            bool onset = (envFollower[channel] > onsetThreshold) && (prevEnv[channel] <= onsetThreshold);
            prevEnv[channel] = envFollower[channel];

            // onset always fires the tail; chance gates a random speed boost
            if (onset && !tailActive[channel]) {
                delayCounter[channel] = delayTimeSamples;
                modEnvelope[channel]  = 1.0f;
                tailActive[channel]   = true;
                // if chance fires, pick a random multiplier between 1x and modSpeed
                if (random.nextFloat() < chance)
                    currentSpeedMult[channel] = 1.0f + random.nextFloat() * (modSpeed - 1.0f);
                else
                    currentSpeedMult[channel] = 1.0f;
            }

            // tail modulator — decay coeff uses per-channel speed multiplier
            float modDecayCoeff = std::exp(-currentSpeedMult[channel] / (modDecayMs * 0.001f * (float)sampleRate));
            if (tailActive[channel]) {
                if (delayCounter[channel] > 0) {
                    delayCounter[channel]--;
                } else {
                    modEnvelope[channel] *= modDecayCoeff;
                    if (modEnvelope[channel] < 0.001f) {
                        modEnvelope[channel] = 0.0f;
                        tailActive[channel]  = false;
                    }
                }
            }

            // modulate sample rate
            // modDirection false: starts at srMin (crushed), decays toward srMax (clean)
            // modDirection true:  starts at srMax (clean), decays toward srMin (crushed)
            float env = modEnvelope[channel] * modAmount;
            float targetSR = modDirection
                ? srMax - env * (srMax - srMin)   // starts clean, crushes
                : srMin + env * (srMax - srMin);   // starts crushed, cleans up

            float holdTime = (float)sampleRate / targetSR;

            // sample rate reduction
            counter[channel] += 1.0f;
            if (counter[channel] >= holdTime) {
                counter[channel] -= holdTime;
                heldSample[channel] = channelData[n];
            }
            channelData[n] = heldSample[channel];

            // bit depth reduction (fixed)
            channelData[n] = stepSize * std::round(channelData[n] / stepSize);
        }
    }
}

//==============================================================================
bool ByterAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ByterAudioProcessor::createEditor()
{
    return new ByterAudioProcessorEditor (*this);
}

void ByterAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void ByterAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ByterAudioProcessor();
}
