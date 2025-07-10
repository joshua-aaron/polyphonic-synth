/*
  ==============================================================================

    Gain.h
    Created: 27 May 2025 6:35:21pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once

class Gain
{
public:
    Gain();
    ~Gain();
    void setTargetGain(float newTargetGain, float rampTimeSeconds, double sampleRate);
    float getNextSample();
    bool isRamping() const;
    float processSample(float inputSample);
    float getCurrentGain() const;
    float getTargetGain() const;
    void reset(float newGain);
private:
    double _sampleRate = 44100.0;
    float _currentGain = 1.0f;
    float _targetGain = 1.0f;
    float _gainStep = 0.0f;
    unsigned int _totalSamples = 0;
    unsigned int _samplesLeft = 0;
};
