/*
  ==============================================================================

    OscillatorUtils.h
    Created: 29 May 2025 9:39:47pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <cstdint>

namespace OscillatorUtils
{
    enum class WaveType : uint8_t
    {
        Sine,
        Triangle,
        Square,
        Saw
    };
    
    enum class OctaveOffset : uint8_t
    {
        One = 1,
        Two = 2
    };
    
    constexpr int getOctaveOffsetAmount(OctaveOffset offset)
    {
        switch (offset)
        {
            case OctaveOffset::One:
                return -12;
            case OctaveOffset::Two:
                return -24;
            default:
                return 0;
        }
    }
    
    constexpr float applyOctaveOffset(float baseFrequency, OctaveOffset offset)
    {
        int octaveShift = static_cast<int>(offset);
        return baseFrequency / (1 << octaveShift);
    }
}
