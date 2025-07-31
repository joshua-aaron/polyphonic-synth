/*
  ==============================================================================

    VoiceWrapper.cpp
    Created: 30 May 2025 4:20:27pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "VoiceWrapper.h"
#include <JuceHeader.h>

VoiceWrapper::VoiceWrapper()
{
    setPitchBendRange(2.0f); // TODO: don't hardode this. Add crrent pitch bend position set
}

bool VoiceWrapper::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthesiserSound*>(sound) != nullptr;
}

void VoiceWrapper::startNote(int midiNoteNumber, float velocity,
                             juce::SynthesiserSound* /*sound*/,
                             int currentPitchWheelPosition)
{
    auto normalizedOffset = std::clamp((currentPitchWheelPosition - 8192) / 8192.0f, -1.0f, 1.0f);
    _voice.setPitchBend(normalizedOffset);
    _voice.startNote(midiNoteNumber, velocity);
}

void VoiceWrapper::stopNote(float velocity, bool allowTailOff)
{
    _voice.stopNote(velocity, allowTailOff);
    if (!allowTailOff || !_voice.isActive())
        clearCurrentNote();
}

void VoiceWrapper::pitchWheelMoved(int newValue)
{
    // Convert [0, 16383] to [-1.0, +1.0]
    auto normalizedOffset = std::clamp((newValue - 8192) / 8192.0f, -1.0f, 1.0f);
    _voice.setPitchBend(normalizedOffset);
}

void VoiceWrapper::controllerMoved(int controllerNumber, int newValue)
{
    _voice.controllerMoved(controllerNumber, newValue);
}

void VoiceWrapper::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                   int startSample, int numSamples)
{
    if (!_voice.isActive())
    {
        clearCurrentNote();
        return;
    }
    
    auto numChannels = outputBuffer.getNumChannels();
    float* channelBuffer[numChannels];
    
    for (auto ch = 0; ch < numChannels; ++ch)
    {
        channelBuffer[ch] = outputBuffer.getWritePointer(ch);
    }
    
    for (auto i = 0; i < numSamples; ++i)
    {
        auto sample = 0.0f;
        _voice.renderNextSample(sample);
        
        for (auto ch = 0; ch < numChannels; ++ch)
        {
            channelBuffer[ch][startSample + i] += sample;
        }
    }
}

void VoiceWrapper::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    _voice.prepare(sampleRate, samplesPerBlock, outputChannels);
}

void VoiceWrapper::setPitchBendRange(float semitones)
{
    _voice.setPitchBendRange(semitones);
}

int VoiceWrapper::getCurrentlyPlayingNote() const
{
    return _voice.getMidiNote();
}

bool VoiceWrapper::isVoiceActive() const
{
    return _voice.isActive();
}

OscillatorUtils::WaveType VoiceWrapper::getOscillatorAType() const { return _voice.getOscillatorAType(); }
OscillatorUtils::WaveType VoiceWrapper::getOscillatorBType() const { return _voice.getOscillatorBType(); }
OscillatorUtils::WaveType VoiceWrapper::getOscillatorSubType() const { return _voice.getOscillatorSubType(); }
void VoiceWrapper::setOscillatorAType(OscillatorUtils::WaveType type) { _voice.setOscillatorAType(type); }
void VoiceWrapper::setOscillatorBType(OscillatorUtils::WaveType type) { _voice.setOscillatorBType(type); }
void VoiceWrapper::setOscillatorSubType(OscillatorUtils::WaveType type) { _voice.setOscillatorSubType(type); }

juce::ADSR::Parameters VoiceWrapper::getAmplitudeEnvelopeParams() const { return _voice.getAmplitudeEnvelopeParams(); }
void VoiceWrapper::setAmplitudeEnvelopeParams(const juce::ADSR::Parameters& params) { _voice.setAmplitudeEnvelopeParams(params); }
juce::ADSR::Parameters VoiceWrapper::getModulationEnvelopeParams() const { return _voice.getModulationEnvelopeParams(); }
void VoiceWrapper::setModulationEnvelopeParams(const juce::ADSR::Parameters& params) { _voice.setModulationEnvelopeParams(params); }

void VoiceWrapper::setOscillatorAGain(float gain, float rampTimeSeconds)
{
    _voice.setOscillatorAGain(gain, rampTimeSeconds);
}

void VoiceWrapper::setOscillatorBGain(float gain, float rampTimeSeconds)
{
    _voice.setOscillatorBGain(gain, rampTimeSeconds);
}

void VoiceWrapper::setOscillatorSubGain(float gain, float rampTimeSeconds)
{
    _voice.setOscillatorSubGain(gain, rampTimeSeconds);
}

void VoiceWrapper::setOscillatorGains(float gainA, float gainB, float gainSub, float rampTimeSeconds)
{
    _voice.setOscillatorGains(gainA, gainB, gainSub, rampTimeSeconds);
}
