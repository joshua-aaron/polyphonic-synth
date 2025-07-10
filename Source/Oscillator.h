/*
  ==============================================================================

 Oscillator.h
    Created: 19 May 2025 3:38:17pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "OscillatorUtils.h"

class Oscillator {
public:
    // Constructor & destructor
    Oscillator();
    ~Oscillator();
    
    // enum for supported wave types
    OscillatorUtils::WaveType getWaveType() const;
    void setWaveType(OscillatorUtils::WaveType type);
    void setFrequency(float freqHz, float sampleRateHz);
    void setPulseWidth(float pw);
    void resetPhase(float startPhase = 0.0f);
    float processSample();
private:
    float _frequency = 440.0f;
    float _sampleRate = 44100.0f;
    float _phase = 0.0f;
    float _phaseIncrement = 0.0f;
    float _pulseWidth = 0.5f; // For square wave pwm
    OscillatorUtils::WaveType _waveType = OscillatorUtils::WaveType::Sine; // Initial wave type
    
    void updatePhaseIncrement();
    void advancePhase();
    float generateSample();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};
