/*
  ==============================================================================

    SynthEngine.h
    Created: 19 May 2025 3:28:44pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utils/EngineUtils.h"
#include "../Voice/VoiceWrapper.h"

class SynthEngine {
    public:
    SynthEngine();
    ~SynthEngine() = default;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    void setPitchRange(float semitones);
    void requestPlayModeChange(EngineUtils::PlayMode mode);
    void handleDeferredPlayModeChange();
    void setPlayMode(EngineUtils::PlayMode mode);
    void reset();
    
    // Oscillators
    OscillatorUtils::WaveType getOscillatorAType() const;
    OscillatorUtils::WaveType getOscillatorBType() const;
    OscillatorUtils::WaveType getOscillatorSubType() const;
    
    void setOscillatorAType(OscillatorUtils::WaveType type);
    void setOscillatorBType(OscillatorUtils::WaveType type);
    void setOscillatorSubType(OscillatorUtils::WaveType type);
    
    void setOscillatorAGain(float gain);
    void setOscillatorBGain(float gain);
    void setOscillatorSubGain(float gain);
    void setOscillatorGains(float gainA, float gainB, float gainSub);
    
    // Envelopes
    const juce::ADSR::Parameters getAmplitudeEnvelopeParams() const;
    const juce::ADSR::Parameters getModulationEnvelopeParams() const;
    
    void setAmplitudeEnvelopeParams(const juce::ADSR::Parameters& params);
    void setModulationEnvelopeParams(const juce::ADSR::Parameters& params);
    void setEnvelopeParameters(const juce::ADSR::Parameters& amplitudeEnvelopeParams, const juce::ADSR::Parameters& modulationEnvelopeParams);
    
    // Gain
    void setMasterGain(float gain);
    float getMasterGain() const;
    
private:
    double _sampleRate = 44100.0;
    static constexpr int _maxVoices = 8;
    VoiceWrapper _voicePool[_maxVoices];
    int _numVoices = 8;
    std::unordered_map<int, int> _activeNotes; // key = midi note, value = voice pool index

    float _pitchBendRange = 2.0f;
    float _pitchWheelValue = 0.0f;
    int _numChannels = 2;
    Gain _masterGain;
    static constexpr float _gainCeiling = 0.25f; // Gives roughly -12dB headroom
    static constexpr float _gainRampTimeSeconds = 0.025f;
    
    EngineUtils::PlayMode _playMode = EngineUtils::PlayMode::Polyphonic;
    EngineUtils::PlayMode _requestedPlayMode;
    std::atomic<bool> _playModeChangeRequested = false;
    
    // Helpers
    void renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void handleMidi(const juce::MidiBuffer& midiMessages);
    void setSampleRate(double sampleRate);
};
