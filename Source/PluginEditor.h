/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_gui_extra/juce_gui_extra.h> // ensure this is included if needed
#include "PluginProcessor.h"
#include "SliderWithLabel.h"

using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using SliderAttachment   = juce::AudioProcessorValueTreeState::SliderAttachment;

//==============================================================================
/**
*/
class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override = default;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& _processor;
    
    // Flexboxes
    juce::FlexBox _oscillatorTypeFlexBox;
    juce::FlexBox _oscillatorGainsFlexBox;
    juce::FlexBox _ampEnvFlexBox;
    juce::FlexBox _modEnvFlexBox;
    juce::FlexBox _masterGainFlexBox;
    
    // Oscillator controls
    juce::ComboBox _oscillatorAType;
    juce::ComboBox _oscillatorBType;
    juce::ComboBox _oscillatorSubType;
    
    SliderWithLabel _oscillatorAGainSlider;
    SliderWithLabel _oscillatorBGainSlider;
    SliderWithLabel _oscillatorSubGainSlider;
    
    std::unique_ptr<ComboBoxAttachment> _oscillatorATypeAttachment;
    std::unique_ptr<ComboBoxAttachment> _oscillatorBTypeAttachment;
    std::unique_ptr<ComboBoxAttachment> _oscillatorSubTypeAttachment;
    
    std::unique_ptr<SliderAttachment> _oscillatorAGainAttachment;
    std::unique_ptr<SliderAttachment> _oscillatorBGainAttachment;
    std::unique_ptr<SliderAttachment> _oscillatorSubGainAttachment;

    // Amplitude Envelope Sliders
    SliderWithLabel _ampEnvAttackSlider;
    SliderWithLabel _ampEnvDecaySlider;
    SliderWithLabel _ampEnvSustainSlider;
    SliderWithLabel _ampEnvReleaseSlider;
    std::unique_ptr<SliderAttachment> _ampEnvAttackAttachment;
    std::unique_ptr<SliderAttachment> _ampEnvDecayAttachment;
    std::unique_ptr<SliderAttachment> _ampEnvSustainAttachment;
    std::unique_ptr<SliderAttachment> _ampEnvReleaseAttachment;

    // Modulation Envelope Sliders
    SliderWithLabel _modEnvAttackSlider;
    SliderWithLabel _modEnvDecaySlider;
    SliderWithLabel _modEnvSustainSlider;
    SliderWithLabel _modEnvReleaseSlider;
    std::unique_ptr<SliderAttachment> _modEnvAttackAttachment;
    std::unique_ptr<SliderAttachment> _modEnvDecayAttachment;
    std::unique_ptr<SliderAttachment> _modEnvSustainAttachment;
    std::unique_ptr<SliderAttachment> _modEnvReleaseAttachment;
    
    // Master gain Slider
    SliderWithLabel _masterGainSlider;
    std::unique_ptr<SliderAttachment> _masterGainAttachment;
    
    void setOscillatorType(juce::ComboBox& cb, bool isOscillatorSub);
    void setupOscillatorControls();
    void setupEnvelopeSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    void setupGainSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name,  float rangeMin, float rangeMax, float increment, float initialValue);
    void setupEnvelopes();
    void setupGains();
    void setupControlAttachments();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
