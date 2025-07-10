/*
  ==============================================================================

    VoiceWrapper.h
    Created: 30 May 2025 4:20:27pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Voice.h"
#include "../SynthesiserSound.h"

class VoiceWrapper : public juce::SynthesiserVoice
{
public:
    VoiceWrapper();

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound,
                   int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void setPitchBendRange(float semitones);
    
    int getCurrentPlayingNote() const;
    bool isVoiceActive() const override;
    
    OscillatorUtils::WaveType getOscillatorAType() const;
    OscillatorUtils::WaveType getOscillatorBType() const;
    OscillatorUtils::WaveType getOscillatorSubType() const;
    void setOscillatorAType(OscillatorUtils::WaveType type);
    void setOscillatorBType(OscillatorUtils::WaveType type);
    void setOscillatorSubType(OscillatorUtils::WaveType type);
    void setOscillatorAGain(float gain, float rampTimeInSeconds);
    void setOscillatorBGain(float gain, float rampTimeInSeconds);
    void setOscillatorSubGain(float gain, float rampTimeInSeconds);
    void setOscillatorGains(float gainA, float gainB, float gainSub, float rampTimeSeconds);

    juce::ADSR::Parameters getAmplitudeEnvelopeParams() const;
    void setAmplitudeEnvelopeParams(const juce::ADSR::Parameters& params);
    juce::ADSR::Parameters getModulationEnvelopeParams() const;
    void setModulationEnvelopeParams(const juce::ADSR::Parameters& params);

private:
    Voice _voice;
};
