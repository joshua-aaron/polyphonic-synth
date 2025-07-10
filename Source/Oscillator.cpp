/*
  ==============================================================================

    Oscillator.cpp
    Created: 19 May 2025 3:38:17pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "Oscillator.h"
#include "OscillatorUtils.h"
#include <JuceHeader.h>

//==============================================================================
// Public members
//==============================================================================

// constructor & destructor
Oscillator::Oscillator() = default;
Oscillator::~Oscillator() = default;

// Get/Set the wave type (sine, square, triangle, saw)
OscillatorUtils::WaveType Oscillator::getWaveType() const { return _waveType; }
void Oscillator::setWaveType(OscillatorUtils::WaveType newType) { _waveType = newType; }

// Reset the phase to a give starting phase
void Oscillator::resetPhase(float startPhase) { _phase = startPhase; }

// Set the oscillator frequency
void Oscillator::setFrequency(float freqHz, float sampleRateHz)
{
    _frequency = freqHz;
    _sampleRate = sampleRateHz;
    updatePhaseIncrement();
}

// Set the pulse width and bound it in between 0.01 & 0.99
void Oscillator::setPulseWidth(float pw)
{
    _pulseWidth = std::clamp(pw, 0.01f, 0.99f);
}

float Oscillator::processSample()
{
    const float sample = generateSample();
    advancePhase();
    return sample;
}

//==============================================================================
// Private members
//==============================================================================
void Oscillator::updatePhaseIncrement()
{
    _phaseIncrement = _frequency / _sampleRate;
}

void Oscillator::advancePhase()
{
    _phase += _phaseIncrement;
    while (_phase >= 1.0f)
    {
        _phase -= 1.0f;
    }
}

float Oscillator::generateSample()
{
    const float twoPi = juce::MathConstants<float>::twoPi;
    switch (_waveType)
    {
        case OscillatorUtils::WaveType::Sine:
            return std::sin(_phase * twoPi);
        case OscillatorUtils::WaveType::Square:
            return (_phase < _pulseWidth) ? 1.0f : -1.0f;
        case OscillatorUtils::WaveType::Saw:
            return 2.0f * _phase -1.0f;
        case OscillatorUtils::WaveType::Triangle:
            return 1.0f - 4.0f * std::abs(_phase - 0.5f);
        default:
            return 0.0f;
    }
}
