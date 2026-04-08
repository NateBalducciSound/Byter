/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ByterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ByterAudioProcessorEditor (ByterAudioProcessor&);
    ~ByterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ByterAudioProcessor& audioProcessor;

  //knob declaration
  juce::Slider bitDepthKnob;
  juce::Slider sampleRateKnob;
  juce::ToggleButton bypassButton;

  //env follower and bitcrush mod knobs
  juce::Slider crushMinKnob;
  juce::Slider crushMaxKnob;
  juce::Slider modAmountKnob;
  juce::Slider modDelayKnob;
  juce::Slider chanceKnob;
  juce::Slider onsetThresholdKnob;

  juce::ToggleButton modDirectionButton


  //bind knobs using attachments to keep value tree params in sync
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  bitDepthAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  sampleRateAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
  bypassAttachment;


  //Bind Envelope Follower Knobs
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  crushMinAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  crushMaxAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  modAmountAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  modDelayAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  chanceAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
  onsetThresholdAttachment;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
  modDirectionAttachment;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ByterAudioProcessorEditor)

};
 
