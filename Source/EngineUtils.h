/*
  ==============================================================================

    EngineUtils.h
    Created: 9 Jun 2025 7:53:44pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <cstdint>

namespace EngineUtils
{
    enum class PlayMode : uint8_t
    {
        Monophonic,
        Duophonic,
        Polyphonic
    };
}
