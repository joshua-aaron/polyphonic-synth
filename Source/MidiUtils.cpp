/*
  ==============================================================================

    MidiUtils.cpp
    Created: 29 May 2025 10:05:59pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "MidiUtils.h"
#include <string_view>

namespace
{
    static constexpr std::string_view sharpNames[] =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };
    
    static constexpr std::string_view flatNames[]  =
    {
        "C", "Db", "D", "Eb", "E", "F",
        "Gb", "G", "Ab", "A", "Bb", "B"
    };
}

float MidiUtils::getMidiNoteInHertz(int midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote-69) / 12.0f);
}

std::string MidiUtils::getNoteNameWithEnharmonics(int midiNote)
{   
    int noteInOctave = midiNote % 12;
    int octave = (midiNote / 12) -1;
    
    // If the note name is the same (like C, D, E, F, G, A, B) show one note
    if (sharpNames[noteInOctave] == flatNames[noteInOctave])
        return std::string(sharpNames[noteInOctave]) + std::to_string(octave);
    
    return std::string(sharpNames[noteInOctave]) + " / " + std::string(flatNames[noteInOctave]) + std::to_string(octave);
}
