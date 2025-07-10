/*
  ==============================================================================

    GainUtils.h
    Created: 29 May 2025 8:52:14pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <algorithm>
#include <cmath>

namespace GainUtils
{
    // Normalize the gain against a given max value and bound it between [0,1]
    inline float normalizeGainValue(int gain, int maxGain)
    {
        return std::clamp(static_cast<float>(gain) / static_cast<float>(maxGain), 0.0f, 1.0f);
    }

    // Defaults to a linear gain curve
    inline float applyPerceptualGainCurve(float normalizedGain, float exponent = 1.0f)
    {
        return std::pow(std::clamp(normalizedGain, 0.0f, 1.0f), exponent);
    }
    
    inline float decibelsToGain(float dB)
    {
        return std::pow(10.0f, dB / 20.0f);
    }

    inline float gainToDecibels(float gain, float minDb)
    {
        constexpr float minGain = 1e-5f;
        if (gain <= minGain)
            return minDb;

        return 20.0f * std::log10(gain);
    }
}
