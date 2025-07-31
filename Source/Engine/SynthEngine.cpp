/*
  ==============================================================================

    SynthEngine.cpp
    Created: 19 May 2025 3:28:44pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "SynthEngine.h"
#include "../SynthesiserSound.h"
#include "../Voice/VoiceWrapper.h"
#include <JuceHeader.h>

SynthEngine::SynthEngine()
{
    _requestedPlayMode = EngineUtils::PlayMode::Polyphonic;
    setPlayMode(EngineUtils::PlayMode::Polyphonic);
    setPitchRange(2.0f);
}

void SynthEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setSampleRate(sampleRate);
    
    for (auto i = 0; i < _numVoices; ++i)
    {
        _voicePool[i].prepareToPlay(sampleRate, samplesPerBlock, _numChannels);
        _voicePool[i].setPitchBendRange(_pitchBendRange);
    }
}

void SynthEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer)
{
    buffer.clear();
    handleDeferredPlayModeChange();
    handleMidi(midiBuffer);
    renderVoices(buffer, 0, buffer.getNumSamples());
}

void SynthEngine::setPitchRange(float semitones)
{
    _pitchBendRange = semitones;
    for (auto i = 0; i < _numVoices; ++i)
    {
        _voicePool[i].setPitchBendRange(semitones);
    }
}

void SynthEngine::requestPlayModeChange(EngineUtils::PlayMode mode)
{
    _requestedPlayMode = mode;
    _playModeChangeRequested.store(true, std::memory_order_release);
}

void SynthEngine::handleDeferredPlayModeChange()
{
    if (_playModeChangeRequested.exchange(false, std::memory_order_acquire))
    {
        setPlayMode(_requestedPlayMode);
    }
}

void SynthEngine::setPlayMode(EngineUtils::PlayMode mode)
{
    switch (mode)
    {
        case EngineUtils::PlayMode::Monophonic:
            _numVoices = 1;
            break;
        case EngineUtils::PlayMode::Duophonic:
            // TODO: DEFINE THIS PLAY MODE
            _numVoices = 2;
            break;
        case EngineUtils::PlayMode::Polyphonic:
            _numVoices = _maxVoices;
            break;
        default:
            _numVoices = 0;
            break;
    }
    
    _playMode = mode;
    for (auto i = _numVoices; i < _maxVoices; ++i)
    {
        _voicePool[i].stopNote(0.0f, false);
    }
    
    _activeNotes.clear();
}

void SynthEngine::handleMidi(const juce::MidiBuffer &midiBuffer)
{
    for (const auto metadata : midiBuffer)
    {
        const auto msg = metadata.getMessage();
        //const auto time = metadata.samplePosition;
        
        if (msg.isNoteOn() && msg.getVelocity() > 0.0f)
        {
            const int midiNote = msg.getNoteNumber();
            const float velocity = msg.getVelocity();
            
            // Note is already being played
            if (_activeNotes.find(midiNote) != _activeNotes.end()) {
                continue;
            }
            
            // Get next free voice or steal one
            for (auto i = 0; i < _numVoices; ++i)
            {
                if (!_voicePool[i].isVoiceActive())
                {
                    // TODO: Take current pitch wheel position into account
                    _voicePool[i].startNote(midiNote, velocity, nullptr, _pitchWheelValue);
                    _activeNotes.insert({midiNote, i});
                    break;
                }
            }
        }
        else if (msg.isNoteOff() || (msg.isNoteOn() && msg.getVelocity() == 0.0f))
        {
            const int midiNote = msg.getNoteNumber();
            for (auto i = 0; i < _numVoices; ++i)
            {
                if (_voicePool[i].isVoiceActive() && _voicePool[i].getCurrentlyPlayingNote() == midiNote)
                {
                    _voicePool[i].stopNote(0.0f, true);
                    auto it = _activeNotes.find(midiNote);
                    if (it != _activeNotes.end())
                    {
                        _activeNotes.erase(it);
                    }
                    
                    break;
                }
            }
        }
        else if (msg.isPitchWheel())
        {
            const int pitchWheelValue = msg.getPitchWheelValue();
            for (auto i = 0; i < _numVoices; ++i)
            {
                _voicePool[i].pitchWheelMoved(pitchWheelValue);
            }
        }
    }
}

void SynthEngine::renderVoices(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    // Get number of channels and initialise the temp buffer
    const auto numChannels = buffer.getNumChannels();
    juce::AudioBuffer<float> tempBuffer(numChannels, numSamples);
    tempBuffer.clear();
    
    // Get number of acrive voices
    int activeVoices = 0;
    for (auto i = 0; i < _numVoices; ++i)
    {
        if (_voicePool[i].isVoiceActive())
        {
            _voicePool[i].renderNextBlock(tempBuffer, startSample, numSamples);
            ++activeVoices;
        }
    }
    
    // Apply master gain
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float gain = _masterGain.getNextSample();
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = tempBuffer.getWritePointer(channel, startSample);
            channelData[sample] *= gain;
        }
    }
    
    // Calculate & apply voice-count-based gain normalization to create headroom
    auto gainNormalization = 1.0f;
    if (activeVoices > 0)
    {
        gainNormalization = _gainCeiling / std::sqrtf(static_cast<float>(activeVoices));
        gainNormalization = std::clamp(gainNormalization, 0.0f, 1.0f);
    }
    
    tempBuffer.applyGain(gainNormalization);
    for (auto channel = 0; channel < numChannels; ++channel)
    {
        buffer.addFrom(channel, startSample, tempBuffer, channel, startSample, numSamples);
    }
}

void SynthEngine::reset()
{
    for (int i = 0; i < _maxVoices; ++i)
    {
        _voicePool[i].stopNote(0.0f, false);  // immediately stop all voices
    }

    _pitchWheelValue = 0.0f;
    _playModeChangeRequested.store(false, std::memory_order_release);
    _requestedPlayMode = _playMode;
    _activeNotes.clear();
}

OscillatorUtils::WaveType SynthEngine::getOscillatorAType() const { return _voicePool[0].getOscillatorAType(); }
OscillatorUtils::WaveType SynthEngine::getOscillatorBType() const { return _voicePool[0].getOscillatorBType(); }
OscillatorUtils::WaveType SynthEngine::getOscillatorSubType() const { return _voicePool[0].getOscillatorSubType(); }

void SynthEngine::setOscillatorAType(OscillatorUtils::WaveType type)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorAType(type);
    }
}

void SynthEngine::setOscillatorBType(OscillatorUtils::WaveType type)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorBType(type);
    }
}

void SynthEngine::setOscillatorSubType(OscillatorUtils::WaveType type)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorSubType(type);
    }
}

const juce::ADSR::Parameters SynthEngine::getAmplitudeEnvelopeParams() const { return _voicePool[0].getAmplitudeEnvelopeParams(); }
const juce::ADSR::Parameters SynthEngine::getModulationEnvelopeParams() const { return _voicePool[0].getModulationEnvelopeParams(); }

void SynthEngine::setEnvelopeParameters(const juce::ADSR::Parameters& amplitudeEnvelopeParams, const juce::ADSR::Parameters& modulationEnvelopeParams)
{
    for (auto& voice : _voicePool)
    {
        voice.setAmplitudeEnvelopeParams(amplitudeEnvelopeParams);
        voice.setModulationEnvelopeParams(modulationEnvelopeParams);
    }
}

void SynthEngine::setAmplitudeEnvelopeParams(const juce::ADSR::Parameters& params)
{
    for (auto& voice : _voicePool)
    {
        voice.setAmplitudeEnvelopeParams(params);
    }
}

void SynthEngine::setModulationEnvelopeParams(const juce::ADSR::Parameters& params)
{
    for (auto& voice : _voicePool)
    {
        voice.setModulationEnvelopeParams(params);
    }
}

void SynthEngine::setOscillatorAGain(float gain)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorAGain(gain, _gainRampTimeSeconds);
    }
}

void SynthEngine::setOscillatorBGain(float gain)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorBGain(gain, _gainRampTimeSeconds);
    }
}

void SynthEngine::setOscillatorSubGain(float gain)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorSubGain(gain, _gainRampTimeSeconds);
    }
}

void SynthEngine::setOscillatorGains(float gainA, float gainB, float gainSub)
{
    for (auto& voice : _voicePool)
    {
        voice.setOscillatorGains(gainA, gainB, gainSub, _gainRampTimeSeconds);
    }
}

void SynthEngine::setMasterGain(float gain)
{
    _masterGain.setTargetGain(gain, _gainRampTimeSeconds, _sampleRate);
}

float SynthEngine::getMasterGain() const
{
    return _masterGain.getTargetGain();
}

void SynthEngine::setSampleRate(double sampleRate)
{
    _sampleRate = sampleRate;
}
