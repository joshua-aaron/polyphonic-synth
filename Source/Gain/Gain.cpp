/*
  ==============================================================================

    Gain.cpp
    Created: 27 May 2025 6:35:21pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "Gain.h"
#include "../Utils/GainUtils.h"
#include <JuceHeader.h>

Gain::Gain() = default;
Gain::~Gain() = default;

void Gain::setTargetGain(float targetGain, float rampTimeSeconds, double sampleRate)
{
    auto clampedTarget = std::clamp(targetGain, 0.0f, 1.0f);
    if (rampTimeSeconds <= 0.0f || sampleRate <= 0.0)
    {
        _currentGain = clampedTarget;
        _targetGain = clampedTarget;
        _gainStep = 0.0f;
        _samplesLeft = 0;
        _totalSamples = 0;
        return;
    }
    
    _targetGain = clampedTarget;
    _totalSamples = std::max(1u, static_cast<unsigned int>(rampTimeSeconds * sampleRate));
    _gainStep = (clampedTarget - _currentGain) / static_cast<float>(_totalSamples);
    _samplesLeft = _totalSamples;
}

float Gain::getNextSample()
{
    if (_samplesLeft > 0)
    {
        _currentGain += _gainStep;
        --_samplesLeft;
    }
    else
    {
        _currentGain = _targetGain;
    }
    
    return _currentGain;
}

float Gain::processSample(float inputSample)
{
    return inputSample * getNextSample();
}

bool Gain::isRamping() const
{
    return _currentGain != _targetGain;
}

void Gain::reset(float gain)
{
    _currentGain = std::clamp(gain, 0.0f, 1.0f);
    _targetGain = _currentGain;
    _gainStep = 0.0f;
    _samplesLeft = 0;
    _totalSamples = 0;
}

float Gain::getCurrentGain() const
{
    return _currentGain;
}

float Gain::getTargetGain() const
{
    return _targetGain;
}
