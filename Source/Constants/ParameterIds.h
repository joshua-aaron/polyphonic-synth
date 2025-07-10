/*
  ==============================================================================

    ParameterIds.h
    Created: 6 Jul 2025 5:08:57pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
namespace ParameterIds
{
    inline constexpr const char* OscillatorATypeId       = "oscillatorAType";
    inline constexpr const char* OscillatorBTypeId       = "oscillatorBType";
    inline constexpr const char* OscillatorSubTypeId     = "oscillatorSubType";
    inline constexpr const char* OscillatorAGainId       = "oscillatorAGain";
    inline constexpr const char* OscillatorBGainId       = "oscillatorBGain";
    inline constexpr const char* OscillatorSubGainId     = "oscillatorSubGain";

    inline constexpr const char* AmplitudeEnvelopeAttackId  = "amplitudeEnvelopeAttack";
    inline constexpr const char* AmplitudeEnvelopeDecayId   = "amplitudeEnvelopeDecay";
    inline constexpr const char* AmplitudeEnvelopeSustainId = "amplitudeEnvelopeSustain";
    inline constexpr const char* AmplitudeEnvelopeReleaseId = "amplitudeEnvelopeRelease";

    inline constexpr const char* ModulationEnvelopeAttackId  = "modulationEnvelopeAttack";
    inline constexpr const char* ModulationEnvelopeDecayId   = "modulationEnvelopeDecay";
    inline constexpr const char* ModulationEnvelopeSustainId = "modulationEnvelopeSustain";
    inline constexpr const char* ModulationEnvelopeReleaseId = "modulationEnvelopeRelease";

    inline constexpr const char* MasterGainId = "masterGain";
    
    inline constexpr const char* parameterIds[] = {
        OscillatorATypeId,
        OscillatorBTypeId,
        OscillatorSubTypeId,
        OscillatorAGainId,
        OscillatorBGainId,
        OscillatorSubGainId,
        AmplitudeEnvelopeAttackId,
        AmplitudeEnvelopeDecayId,
        AmplitudeEnvelopeSustainId,
        AmplitudeEnvelopeReleaseId,
        ModulationEnvelopeAttackId,
        ModulationEnvelopeDecayId,
        ModulationEnvelopeSustainId,
        ModulationEnvelopeReleaseId,
        MasterGainId
    };
}
