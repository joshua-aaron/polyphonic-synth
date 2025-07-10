/*
  ==============================================================================

    MidiUtils.h
    Created: 29 May 2025 10:05:01pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <string>

namespace MidiUtils
{
    float getMidiNoteInHertz(int midiNote);
    std::string getNoteNameWithEnharmonics(int midiNote);
}
