/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ByterAudioProcessorEditor::ByterAudioProcessorEditor (ByterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto makeRotary = [](juce::Slider& s) {
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    };

    makeRotary(bitDepthKnob);
    makeRotary(sampleRateKnob);
    makeRotary(crushMinKnob);
    makeRotary(crushMaxKnob);
    makeRotary(modAmountKnob);
    makeRotary(modDelayKnob);
    makeRotary(modDecayKnob);
    makeRotary(chanceKnob);
    makeRotary(onsetThresholdKnob);

    bypassButton.setButtonText("Bypass");
    modDirectionButton.setButtonText("Direction");

    auto& vt = audioProcessor.audioValueTree;

    bitDepthAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "bitDepth",       bitDepthKnob);
    sampleRateAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "sampleRate",     sampleRateKnob);
    crushMinAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "crushMin",       crushMinKnob);
    crushMaxAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "crushMax",       crushMaxKnob);
    modAmountAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "modAmount",      modAmountKnob);
    modDelayAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "modDelay",       modDelayKnob);
    modDecayAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "modDecay",       modDecayKnob);
    chanceAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "chance",         chanceKnob);
    onsetThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vt, "onsetThreshold", onsetThresholdKnob);
    bypassAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(vt, "bypass",         bypassButton);
    modDirectionAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(vt, "modDirection",   modDirectionButton);

    for (auto* w : { &bitDepthKnob, &sampleRateKnob, &crushMinKnob, &crushMaxKnob,
                     &modAmountKnob, &modDelayKnob, &modDecayKnob, &chanceKnob, &onsetThresholdKnob })
        addAndMakeVisible(w);

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(modDirectionButton);

    setSize(600, 400);
}

ByterAudioProcessorEditor::~ByterAudioProcessorEditor()
{
}

//==============================================================================
void ByterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void ByterAudioProcessorEditor::resized()
{
    // row 1: core bitcrush
    bitDepthKnob.setBounds       (20,  20, 90, 110);
    sampleRateKnob.setBounds     (120, 20, 90, 110);
    crushMinKnob.setBounds       (220, 20, 90, 110);
    crushMaxKnob.setBounds       (320, 20, 90, 110);
    bypassButton.setBounds       (430, 50, 80,  30);

    // row 2: tail/mod controls
    modAmountKnob.setBounds      (20,  200, 90, 110);
    modDelayKnob.setBounds       (120, 200, 90, 110);
    modDecayKnob.setBounds       (220, 200, 90, 110);
    chanceKnob.setBounds         (320, 200, 90, 110);
    onsetThresholdKnob.setBounds (420, 200, 90, 110);
    modDirectionButton.setBounds (430, 160, 100, 30);
}
