# Synthesisers amirite?
## Introduction
This project was born out of an interest to learn DSP and audio programming in a sound design context. As such, this definitely isn't perfect. However, I aim to incrementally improve on the plugin to eventually have a production-ready polyphonic synthesiser. 

The synthesiser is programmed in C++ using the [JUCE application framework](https://juce.com/)

## Settings / Plugin Parameters
Supported Play Modes:
- Monophonic
- Duophonic
- Polyphonic (Default)

Oscillator Wave Types:
- Sine
- Square
- Saw
- Triangle

Sub-Oscillator Wave Types:
- Sine
- Square

Envelopes
- Amplitude
- Modulation (not currently hooked up)

Gains:
- Master
- Oscillator A
- Oscillator B
- Sub-Oscillator

## Description
The goal with this is to have a fully functional 8-voice synthesiser that can work in the afformentioned play modes. Once all of the base parts are hooked up, I plan to add a few more blocks to the signal flow. Those include:
- LFOs for modulation
- LPF, HPF, and BP filters with resonance
- Distortion / Saturation
- Reverb
- Delay
- Other misc effects

