/*
  ==============================================================================

    Voice.h
    Created: 17 May 2025 11:00:53am
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <string>
#include <JuceHeader.h>
#include "Oscillator.h"
#include "OscillatorUtils.h"
#include "Gain.h"

class Voice
{
public:
    // Constructors / Destructors
    Voice();
    ~Voice() = default;
    
    // Initializing methods
    void prepare(double sampleRate, int samplePerBlock, int numChannels);
    
    // Playback
    void startNote(int midiNote, float velocity);
    void stopNote(float velocity, bool allowTailOff);
    void renderNextSample(float& outputSample);
    
    void prepareForReuse();
    bool isActive() const;
    
    void setSampleRate(double sampleRate);
    void setEnvelopeParams(juce::ADSR::Parameters& amplitudeEnvParams, juce::ADSR::Parameters& modulationEnvParams);
    void setPitchBend(float semitoneOffset);
    void setPitchBendRange(float semitones);
    void controllerMoved(int controllerNumber, int newValue);
    int getMidiNote() const;
    
    // Oscillator methods
    OscillatorUtils::WaveType getOscillatorAType() const;
    OscillatorUtils::WaveType getOscillatorBType() const;
    OscillatorUtils::WaveType getOscillatorSubType() const;
    void setOscillatorTypes(OscillatorUtils::WaveType typeA, OscillatorUtils::WaveType typeB, OscillatorUtils::WaveType typeSub);
    void setOscillatorAType(OscillatorUtils::WaveType type);
    void setOscillatorBType(OscillatorUtils::WaveType type);
    void setOscillatorSubType(OscillatorUtils::WaveType type);
    void setOscillatorSubOffset(OscillatorUtils::OctaveOffset offset);
    
    // Envelope methods
    juce::ADSR::Parameters getAmplitudeEnvelopeParams() const;
    juce::ADSR::Parameters getModulationEnvelopeParams() const;
    void setAmplitudeEnvelopeParams(const juce::ADSR::Parameters& params);
    void setModulationEnvelopeParams(const juce::ADSR::Parameters& params);
    
    float getAmplitudeEnvelopeValue() const;
    float getModulationEnvelopeValue() const;
    
    // Gain methods
    void setOscillatorAGain(float gain, float rampTimeInSeconds);
    void setOscillatorBGain(float gain, float rampTimeInSeconds);
    void setOscillatorSubGain(float gain, float rampTimeInSeconds);
    void setOscillatorGains(float gainA, float gainB, float gainSub, float rampTimeSeconds);
    
private:
    // Oscillators
    Oscillator _oscillatorA;
    Oscillator _oscillatorB;
    Oscillator _oscillatorSub;
    OscillatorUtils::OctaveOffset _oscillatorSubOffset = OscillatorUtils::OctaveOffset::One;
    
    // Envelopes
    juce::ADSR _amplitudeEnvelope;
    juce::ADSR::Parameters _amplitudeEnvelopeParams;
    float _lastAmplitudeEnvSample = 0.0f;
    juce::ADSR _modulationEnvelope;
    juce::ADSR::Parameters _modulationEnvelopeParams;
    float _lastModulationEnvSample = 0.0f;
    
    // Gain
    Gain _gainA;
    Gain _gainB;
    Gain _gainSub;
    
    // Note info
    bool _active = false;
    float _currentFrequency = 440.0f;
    int _midiNote = -1;
    int _previousMidiNote = -1;
    float _pitchBendSemitoneOffset = 0.0f;
    float _pitchBendRange = 2.0f;
    float _modWheelDepth = 0.0f;
    
    std::string _noteNameWithEnharmonics = "";
    double _sampleRate = 44100.0;
    float _velocity = 1.0f;
    
    // Constants
    static constexpr float _gainRampTimeSeconds = 0.025f;
    
    // Helpers
    void initialiseDefaults();
    void updateOscillatorFrequencies(std::optional<int> midiNote);
    void setEnvelopeSampleRate(double sampleRate);
    void setOscillatorGains(float gainA, float gainB, float gainSub);
    void setOscillatorGains(float gain, float rampTimeInSeconds);
    void resetEnvelopes();
    void deactivateEnvelopes();
    void activateEnvelopes();
    float getNextSampleSummed();
    float getNextSample(Oscillator& oscillator, Gain& gain);
    void processGains(float& sample);
    void initialiseOscillators(OscillatorUtils::WaveType typeA,
                               OscillatorUtils::WaveType typeB,
                               OscillatorUtils::WaveType typeSub,
                               OscillatorUtils::OctaveOffset oscillatorSubOffset);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Voice)
};
