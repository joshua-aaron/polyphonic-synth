/*
  ==============================================================================

    SynthesiserSound.h
    Created: 9 Jun 2025 8:19:31pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthesiserSound : public juce::SynthesiserSound
{
public:
    SynthesiserSound() = default;
    ~SynthesiserSound() = default;
    
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};
