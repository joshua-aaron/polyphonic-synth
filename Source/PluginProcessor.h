/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthEngine.h"

//==============================================================================
/**
*/
class PluginProcessor  : public juce::AudioProcessor,
                         public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    PluginProcessor();
    ~PluginProcessor() override;

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
    SynthEngine& getSynthEngine();
    juce::AudioProcessorValueTreeState& getAudioProcessorValueTreeState();

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
private:
    SynthEngine _synthEngine;
    juce::AudioProcessorValueTreeState _audioProcessorValueTreeState;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initializeParameterListeners();
    void addParameterListener(const juce::String& paramID);
    std::vector<juce::String> getParameterIds() const;
    static constexpr float _gainRampTimeInSeconds= 0.03f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
