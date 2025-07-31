/*
  ==============================================================================

    Voice.cpp
    Created: 17 May 2025 11:00:53am
    Author:  Joshua Navon

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../Utils/MidiUtils.h"
#include "../Oscillator/Oscillator.h"
#include "Voice.h"

// Constructors
Voice::Voice()
{
    initialiseDefaults();
}

// Private methods
void Voice::initialiseDefaults()
{
    // Oscillators
    initialiseOscillators(OscillatorUtils::WaveType::Sine,
                          OscillatorUtils::WaveType::Sine,
                          OscillatorUtils::WaveType::Sine,
                          OscillatorUtils::OctaveOffset::One);
    
    // Envelopes
    resetEnvelopes();
    
    // Gains
    setOscillatorGains(0.0f, 0.0f, 0.0f);
}

void Voice::updateOscillatorFrequencies(std::optional<int> midiNote)
{
    auto note = midiNote.value_or((_midiNote >= 0 ? _midiNote : _previousMidiNote));
    if (note < 0)
    {
        _currentFrequency = 0.0f;
    }
    else
    {
        auto pitchBendFactor = std::pow(2.0f, _pitchBendSemitoneOffset / 12.0f);
        _currentFrequency = std::clamp(MidiUtils::getMidiNoteInHertz(note), 0.0f, 20000.0f) * pitchBendFactor;
        if (_midiNote >= 0)
        {
            _previousMidiNote = _midiNote;
        }
    }
    
    _oscillatorA.setFrequency(_currentFrequency, _sampleRate);
    _oscillatorB.setFrequency(_currentFrequency, _sampleRate);
    
    auto subFreq = OscillatorUtils::applyOctaveOffset(_currentFrequency, _oscillatorSubOffset);
    _oscillatorSub.setFrequency(subFreq, _sampleRate);
}

void Voice:: setEnvelopeSampleRate(double sampleRate)
{
    _amplitudeEnvelope.setSampleRate(sampleRate);
    _modulationEnvelope.setSampleRate(sampleRate);
}

void Voice::resetEnvelopes()
{
    // Assign them to the envelopes
    _amplitudeEnvelope.setParameters(_amplitudeEnvelopeParams);
    _modulationEnvelope.setParameters(_modulationEnvelopeParams);
    
    // Reset the envelopes
    _amplitudeEnvelope.reset();
    _modulationEnvelope.reset();
}

void Voice::initialiseOscillators(OscillatorUtils::WaveType typeA,
                                  OscillatorUtils::WaveType typeB,
                                  OscillatorUtils::WaveType typeSub,
                                  OscillatorUtils::OctaveOffset oscillatorSubOffset)
{
    // Set oscillator types and offset amount
    setOscillatorTypes(typeA, typeB, typeSub);
    setOscillatorSubOffset(oscillatorSubOffset);
    
    // Set default pulse width for square waves
    _oscillatorA.setPulseWidth(0.5f);
    _oscillatorB.setPulseWidth(0.5f);
    _oscillatorSub.setPulseWidth(0.5f);
    
    // Reset phase
    _oscillatorA.resetPhase();
    _oscillatorB.resetPhase();
    _oscillatorSub.resetPhase();
}

void Voice::activateEnvelopes()
{
    _amplitudeEnvelope.noteOn();
    _modulationEnvelope.noteOn();
}

void Voice::deactivateEnvelopes()
{
    _amplitudeEnvelope.noteOff();
    _modulationEnvelope.noteOff();
}

float Voice::getNextSampleSummed()
{
    // Generate the samples
    auto sampleA = getNextSample(_oscillatorA, _gainA);
    auto sampleB = getNextSample(_oscillatorB, _gainB);
    auto sampleSub = getNextSample(_oscillatorSub, _gainSub);
    
    // Return the sum of each sample
    return sampleA + sampleB + sampleSub;
}

float Voice::getNextSample(Oscillator& oscillator, Gain& gain)
{
    return gain.processSample(oscillator.processSample());
}

// Initialisation
void Voice::prepare(double sampleRate, int samplePerBlock, int numChannels)
{
    if (_sampleRate != sampleRate)
    {
        _sampleRate = sampleRate;
        setEnvelopeSampleRate(sampleRate);
    }
    
    int note = (_midiNote >= 0) ? _midiNote : _previousMidiNote;
    float gainA = (note >= 0) ? std::clamp(_velocity * _gainA.getCurrentGain(), 0.0f, 1.0f) : 0.0f;
    float gainB = (note >= 0) ? std::clamp(_velocity * _gainB.getCurrentGain(), 0.0f, 1.0f) : 0.0f;
    float gainSub = (note >= 0) ? std::clamp(_velocity * _gainSub.getCurrentGain(), 0.0f, 1.0f) : 0.0f;

    
    updateOscillatorFrequencies(note);
    setOscillatorGains(gainA, gainB, gainSub);
}

// Playback behaviour
void Voice::startNote(int midiNote, float velocity)
{
    _midiNote = midiNote;
    _velocity = velocity;
    _currentFrequency = MidiUtils::getMidiNoteInHertz(_midiNote);
    
    updateOscillatorFrequencies(_midiNote);
    
    activateEnvelopes();
    
    float gainA = std::clamp(_velocity * _gainA.getCurrentGain(), 0.0f, 1.0f);
    float gainB = std::clamp(_velocity * _gainB.getCurrentGain(), 0.0f, 1.0f);
    float gainSub = std::clamp(_velocity * _gainSub.getCurrentGain(), 0.0f, 1.0f);
    
    setOscillatorGains(gainA, gainB, gainSub);
    _active = true;
    DBG("_midiNote set to: " << _midiNote);
}

void Voice::stopNote(float /* velocity */, bool allowTailOff)
{
    if (allowTailOff)
    {
        deactivateEnvelopes(); // Start release phase
    }
    else
    {
        prepareForReuse(); // Kill the voice
        resetEnvelopes();
    }
}


void Voice::renderNextSample(float& outputSample)
{
    _lastAmplitudeEnvSample = _amplitudeEnvelope.getNextSample();
    if (_lastAmplitudeEnvSample > 0.0001f)
    {
        outputSample = getNextSampleSummed() * _lastAmplitudeEnvSample;
    }
    else
    {
        prepareForReuse();
        outputSample = 0.0f;
        return;
    }
    
    if (_modulationEnvelope.isActive())
    {
        _lastModulationEnvSample = _modulationEnvelope.getNextSample(); // TODO: Implement modulation...
    }
    else
    {
        _lastModulationEnvSample = 0.0f;
        _modulationEnvelope.reset();
    }
}

bool Voice::isActive() const
{
    return _active;
}

void Voice::prepareForReuse()
{
    _midiNote = -1;
    _previousMidiNote = -1;
    _active = false;
    _velocity = 0.0f;
    
    _lastAmplitudeEnvSample = 0.0f;
    _lastModulationEnvSample = 0.0f;
    
    setOscillatorGains(0.0f, 0.0f, 0.0f);
}

void Voice::setSampleRate(double sampleRate)
{
    _sampleRate = sampleRate;
}

void Voice::setEnvelopeParams(juce::ADSR::Parameters& amplitudeEnvParams, juce::ADSR::Parameters& modulationEnvParams)
{
    _amplitudeEnvelopeParams = amplitudeEnvParams;
    _modulationEnvelopeParams = modulationEnvParams;
    _amplitudeEnvelope.setParameters(amplitudeEnvParams);
    _modulationEnvelope.setParameters(modulationEnvParams);
}

OscillatorUtils::WaveType Voice::getOscillatorAType() const { return _oscillatorA.getWaveType(); }
OscillatorUtils::WaveType Voice::getOscillatorBType() const { return _oscillatorB.getWaveType(); }
OscillatorUtils::WaveType Voice::getOscillatorSubType() const { return _oscillatorSub.getWaveType(); }

void Voice::setOscillatorTypes(OscillatorUtils::WaveType typeA, OscillatorUtils::WaveType typeB, OscillatorUtils::WaveType typeSub)
{
    _oscillatorA.setWaveType(typeA);
    _oscillatorB.setWaveType(typeB);
    _oscillatorSub.setWaveType(typeSub);
}

void Voice::setOscillatorAType(OscillatorUtils::WaveType type)
{
    _oscillatorA.setWaveType(type);
}

void Voice::setOscillatorBType(OscillatorUtils::WaveType type)
{
    _oscillatorB.setWaveType(type);
}

void Voice::setOscillatorSubType(OscillatorUtils::WaveType type)
{
    _oscillatorSub.setWaveType(type);
}

void Voice::setOscillatorSubOffset(OscillatorUtils::OctaveOffset offset)
{
    _oscillatorSubOffset = offset;
    auto frequencySub = OscillatorUtils::applyOctaveOffset(_currentFrequency, _oscillatorSubOffset);
    _oscillatorSub.setFrequency(frequencySub, _sampleRate);
}

void Voice::setPitchBend(float normalizedOffset)
{
    _pitchBendSemitoneOffset = std::clamp(normalizedOffset, -1.0f, 1.0f) * _pitchBendRange;
    updateOscillatorFrequencies(std::nullopt);
}

void Voice::setPitchBendRange(float semitones)
{
    _pitchBendRange = std::clamp(semitones, 0.0f, 48.0f); // e.g. up to 4 octaves in semitones;
    updateOscillatorFrequencies(std::nullopt);
}

void Voice::controllerMoved(int controllerNumber, int newValue)
{
    if (controllerNumber == 1) // Mod wheel
    {
        auto normalized = std::clamp(static_cast<float>(newValue) / 127.0f, 0.0f, 1.0f);
        _modWheelDepth = normalized;
    }
}

int Voice::getMidiNote() const { return _midiNote; }

float Voice::getAmplitudeEnvelopeValue() const { return _lastAmplitudeEnvSample; }
float Voice::getModulationEnvelopeValue() const { return _lastModulationEnvSample; }

juce::ADSR::Parameters Voice::getAmplitudeEnvelopeParams() const { return _amplitudeEnvelopeParams; }
void Voice::setAmplitudeEnvelopeParams(const juce::ADSR::Parameters &params)
{
    _amplitudeEnvelopeParams = params;
    _amplitudeEnvelope.setParameters(params);
}

juce::ADSR::Parameters Voice::getModulationEnvelopeParams() const { return _modulationEnvelopeParams; }
void Voice::setModulationEnvelopeParams(const juce::ADSR::Parameters &params)
{
    _modulationEnvelopeParams = params;
    _modulationEnvelope.setParameters(params);
}

void Voice::setOscillatorAGain(float gain, float rampTimeSeconds)
{
    _gainA.setTargetGain(gain, rampTimeSeconds, _sampleRate);
}

void Voice::setOscillatorBGain(float gain, float rampTimeSeconds)
{
    _gainB.setTargetGain(gain, rampTimeSeconds, _sampleRate);
}

void Voice::setOscillatorSubGain(float gain, float rampTimeSeconds)
{
    _gainSub.setTargetGain(gain, rampTimeSeconds, _sampleRate);
}

void Voice::setOscillatorGains(float gainA, float gainB, float gainSub, float rampTimeSeconds)
{
    _gainA.setTargetGain(gainA, rampTimeSeconds, _sampleRate);
    _gainB.setTargetGain(gainB, rampTimeSeconds, _sampleRate);
    _gainSub.setTargetGain(gainSub, rampTimeSeconds, _sampleRate);
}

void Voice::setOscillatorGains(float gainA, float gainB, float gainSub)
{
    _gainA.setTargetGain(gainA, _gainRampTimeSeconds, _sampleRate);
    _gainB.setTargetGain(gainB, _gainRampTimeSeconds, _sampleRate);
    _gainSub.setTargetGain(gainSub, _gainRampTimeSeconds, _sampleRate);
}
