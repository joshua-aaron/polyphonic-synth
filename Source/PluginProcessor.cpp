/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIds.h"

//==============================================================================
PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
         #if ! JucePlugin_IsMidiEffect
          #if ! JucePlugin_IsSynth
           .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          #endif
           .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
         #endif
        ),
_audioProcessorValueTreeState(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    initializeParameterListeners();
}

PluginProcessor::~PluginProcessor()
{
    for (auto* param : ParameterIds::parameterIds)
    {
        _audioProcessorValueTreeState.removeParameterListener(param, this);
    }

}


//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillators
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::OscillatorATypeId,
                                                                  "Oscillator A",
                                                                  juce::StringArray { "Sine", "Square", "Saw", "Triangle" },
                                                                  0));
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::OscillatorBTypeId,
                                                                  "Oscillator B",
                                                                  juce::StringArray { "Sine", "Square", "Saw", "Triangle" },
                                                                  0));
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::OscillatorSubTypeId,
                                                                  "Sub Oscillator",
                                                                  juce::StringArray { "Sine", "Square" },
                                                                  0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::OscillatorAGainId,  "Osc A Gain",  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::OscillatorBGainId,   "Osc B Gain",   juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::OscillatorSubGainId, "Sub Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    
    // Envelopes
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::AmplitudeEnvelopeAttackId,  "Amp Attack",  0.0f, 5.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::AmplitudeEnvelopeDecayId,   "Amp Decay",   0.01f, 5.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::AmplitudeEnvelopeSustainId, "Amp Sustain", 0.0f,  1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::AmplitudeEnvelopeReleaseId, "Amp Release", 0.01f, 5.0f, 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::ModulationEnvelopeAttackId,  "Mod Attack",  0.00f, 5.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::ModulationEnvelopeDecayId,   "Mod Decay",   0.01f, 5.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::ModulationEnvelopeSustainId, "Mod Sustain", 0.0f,  1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::ModulationEnvelopeReleaseId, "Mod Release", 0.01f, 5.0f, 0.1f));
    
    // Master Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::MasterGainId, "Master Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    return { params.begin(), params.end() };
}

void PluginProcessor::addParameterListener(const juce::String& paramId)
{
    _audioProcessorValueTreeState.addParameterListener(paramId, this);
}

void PluginProcessor::initializeParameterListeners()
{
    // Oscillators
    addParameterListener(ParameterIds::OscillatorATypeId);
    addParameterListener(ParameterIds::OscillatorBTypeId);
    addParameterListener(ParameterIds::OscillatorSubTypeId);
    
    addParameterListener(ParameterIds::OscillatorAGainId);
    addParameterListener(ParameterIds::OscillatorBGainId);
    addParameterListener(ParameterIds::OscillatorSubGainId);

    // Envelopes
    addParameterListener(ParameterIds::AmplitudeEnvelopeAttackId);
    addParameterListener(ParameterIds::AmplitudeEnvelopeDecayId);
    addParameterListener(ParameterIds::AmplitudeEnvelopeSustainId);
    addParameterListener(ParameterIds::AmplitudeEnvelopeReleaseId);

    addParameterListener(ParameterIds::ModulationEnvelopeAttackId);
    addParameterListener(ParameterIds::ModulationEnvelopeDecayId);
    addParameterListener(ParameterIds::ModulationEnvelopeSustainId);
    addParameterListener(ParameterIds::ModulationEnvelopeReleaseId);
    
    // Master Gain
    addParameterListener(ParameterIds::MasterGainId);
}

void PluginProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    using WaveType = OscillatorUtils::WaveType;
    
    if (parameterID == ParameterIds::OscillatorATypeId)
    {
        _synthEngine.setOscillatorAType(static_cast<WaveType>(static_cast<int>(newValue)));
    }
    else if (parameterID == ParameterIds::OscillatorBTypeId)
    {
        _synthEngine.setOscillatorBType(static_cast<WaveType>(static_cast<int>(newValue)));
    }
    else if (parameterID == ParameterIds::OscillatorSubTypeId)
    {
        _synthEngine.setOscillatorSubType(static_cast<WaveType>(static_cast<int>(newValue)));
    }
    else if (parameterID == ParameterIds::OscillatorAGainId)
    {
        _synthEngine.setOscillatorAGain(newValue);
    }
    else if (parameterID == ParameterIds::OscillatorBGainId)
    {
        _synthEngine.setOscillatorBGain(newValue);
    }
    else if (parameterID == ParameterIds::OscillatorSubGainId)
    {
        _synthEngine.setOscillatorSubGain(newValue);
    }
    else if (parameterID == ParameterIds::AmplitudeEnvelopeAttackId ||
             parameterID == ParameterIds::AmplitudeEnvelopeDecayId ||
             parameterID == ParameterIds::AmplitudeEnvelopeSustainId ||
             parameterID == ParameterIds::AmplitudeEnvelopeReleaseId)
    {
        auto p = juce::ADSR::Parameters{
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::AmplitudeEnvelopeAttackId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::AmplitudeEnvelopeDecayId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::AmplitudeEnvelopeSustainId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::AmplitudeEnvelopeReleaseId)->load()
        };
        
        _synthEngine.setAmplitudeEnvelopeParams(p);
    }
    else if (parameterID == ParameterIds::ModulationEnvelopeAttackId ||
             parameterID == ParameterIds::ModulationEnvelopeDecayId ||
             parameterID == ParameterIds::ModulationEnvelopeSustainId ||
             parameterID == ParameterIds::ModulationEnvelopeReleaseId)
    {
        auto p = juce::ADSR::Parameters{
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::ModulationEnvelopeAttackId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::ModulationEnvelopeDecayId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::ModulationEnvelopeSustainId)->load(),
            _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::ModulationEnvelopeReleaseId)->load()
        };
        
        _synthEngine.setModulationEnvelopeParams(p);
    }
    else if (parameterID == ParameterIds::MasterGainId)
    {
        _synthEngine.setMasterGain(newValue);
    }
}


const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    _synthEngine.prepareToPlay(sampleRate, samplesPerBlock);
    
    // Master gain
    if (auto * masterGainParam = _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::MasterGainId))
    {
        _synthEngine.setMasterGain(std::clamp(masterGainParam->load(), 0.0f, 1.0f));
    }
    
    // Oscillator Gains
    if (auto* oscAGain = _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::OscillatorAGainId))
    {
        _synthEngine.setOscillatorAGain(std::clamp(oscAGain->load(), 0.0f, 1.0f));
    }

    if (auto* oscBGain = _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::OscillatorBGainId))
    {
        _synthEngine.setOscillatorBGain(std::clamp(oscBGain->load(), 0.0f, 1.0f));
    }

    if (auto* subGain = _audioProcessorValueTreeState.getRawParameterValue(ParameterIds::OscillatorSubGainId))
    {
        _synthEngine.setOscillatorSubGain(std::clamp(subGain->load(), 0.0f, 1.0f));
    }
}

void PluginProcessor::releaseResources()
{
    _synthEngine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    _synthEngine.processBlock(buffer, midiMessages);
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Serialize tree to memory
    juce::MemoryOutputStream stream(destData, true);
    _audioProcessorValueTreeState.state.writeToStream(stream);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto stateTree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (stateTree.isValid() && stateTree.getType() == juce::Identifier("SynthState"))
    {
        _audioProcessorValueTreeState.state = stateTree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

SynthEngine& PluginProcessor::getSynthEngine() { return _synthEngine; }
juce::AudioProcessorValueTreeState& PluginProcessor::getAudioProcessorValueTreeState() {return _audioProcessorValueTreeState; }
