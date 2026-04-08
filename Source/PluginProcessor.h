/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ByterAudioProcessor  : public juce::AudioProcessor
{
public:
  //base JUCE stuff
    //==============================================================================
    ByterAudioProcessor();
    ~ByterAudioProcessor() override;


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

  //Added Shit by me
  juce::AudioProcessorValueTreeState audioValueTree;
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();



private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByterAudioProcessor)

    //Bitcrush Variables

    double sampleRate = 44100.0; 

  std::array<float, 2> heldSample {0.0f, 0.0f};
  std::array<float, 2> counter {0.0f, 0.0f};


  //mod env and crush mod tail params
  //env follower states for both chans
  std::array<float, 2> envFollower { 0.0f, 0.0f};
  std::array<float, 2> prevEnv { 0.0f, 0.0f};

  //tail mod states for both chans
  std::array<float, 2> modEnvelope {0.0f, 0.0f};
  std::array<int, 2> delayCounter { 0, 0};
  std::array<bool, 2> tailActive { false, false };



};
