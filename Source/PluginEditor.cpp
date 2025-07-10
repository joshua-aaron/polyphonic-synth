/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIds.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , _processor(p)
    , _oscillatorAGainSlider("Osc A Gain")
    , _oscillatorBGainSlider("Osc B Gain")
    , _oscillatorSubGainSlider("Osc Sub Gain")
    , _ampEnvAttackSlider("Attack")
    , _ampEnvDecaySlider("Decay")
    , _ampEnvSustainSlider("Sustain")
    , _ampEnvReleaseSlider("Release")
    , _modEnvAttackSlider("Attack")
    , _modEnvDecaySlider("Decay")
    , _modEnvSustainSlider("Sustain")
    , _modEnvReleaseSlider("Release")
    , _masterGainSlider("Master")
{
    // Oscillators
    setupOscillatorControls();
    
    // Envelopes
    setupEnvelopes();
    
    // Gain
    setupGains();
    
    // Parameter value tree
    setupControlAttachments();
    
    setSize(600, 600);
    setResizable(true, true);
    setResizeLimits(400, 400, 1000, 1000);
}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    constexpr int sliderHeight = 150;
    constexpr int labelHeight = 20;
    constexpr int padding = 4;
    constexpr int componentHeight = sliderHeight + labelHeight + padding;
    constexpr int sliderWidth = 60;
    auto area = getLocalBounds().reduced(10);

    // Oscillator Types
    _oscillatorTypeFlexBox.flexDirection = juce::FlexBox::Direction::row;
    _oscillatorTypeFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    _oscillatorTypeFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    _oscillatorTypeFlexBox.items.clear();
    
    _oscillatorTypeFlexBox.items.add(juce::FlexItem(_oscillatorAType).withMinWidth(120).withMaxWidth(120).withHeight(40).withMargin(4));
    _oscillatorTypeFlexBox.items.add(juce::FlexItem(_oscillatorBType).withMinWidth(120).withMaxWidth(120).withHeight(40).withMargin(4));
    _oscillatorTypeFlexBox.items.add(juce::FlexItem(_oscillatorSubType).withMinWidth(120).withMaxWidth(120).withHeight(40).withMargin(4));

    auto oscArea = area.removeFromTop(60);
    _oscillatorTypeFlexBox.performLayout(oscArea);
    
    // Oscillator Gains
    _oscillatorGainsFlexBox.flexDirection = juce::FlexBox::Direction::row;
    _oscillatorGainsFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    _oscillatorGainsFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    _oscillatorGainsFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    _oscillatorGainsFlexBox.items.clear();

    for (auto* comp : {
        &_oscillatorAGainSlider,
        &_oscillatorBGainSlider,
        &_oscillatorSubGainSlider
    }) {
        _oscillatorGainsFlexBox.items.add(juce::FlexItem(*comp)
                                          .withMinWidth(sliderWidth)
                                          .withMinHeight(componentHeight)
                                          .withMargin(juce::FlexItem::Margin(5)));
    }

    auto oscGainArea = area.removeFromTop(sliderHeight + 20);
    _oscillatorGainsFlexBox.performLayout(oscGainArea);

    // Amplitude Envelope
    _ampEnvFlexBox.flexDirection = juce::FlexBox::Direction::row;
    _ampEnvFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    _ampEnvFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    _ampEnvFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    _ampEnvFlexBox.items.clear();
    
    for (auto* comp : {
        &_ampEnvAttackSlider,
        &_ampEnvDecaySlider,
        &_ampEnvSustainSlider,
        &_ampEnvReleaseSlider
    })
    {
        _ampEnvFlexBox.items.add(juce::FlexItem(*comp)
                                 .withMinWidth(sliderWidth)
                                 .withMinHeight(componentHeight)
                                 .withMargin(juce::FlexItem::Margin(5)));
    }

    auto ampEnvArea = area.removeFromTop(sliderHeight + 20);
    _ampEnvFlexBox.performLayout(ampEnvArea);
    
    _modEnvFlexBox.flexDirection = juce::FlexBox::Direction::row;
    _modEnvFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    _modEnvFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    _modEnvFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    _modEnvFlexBox.items.clear();

    for (auto* comp : {
        &_modEnvAttackSlider,
        &_modEnvDecaySlider,
        &_modEnvSustainSlider,
        &_modEnvReleaseSlider
    }) {
        _modEnvFlexBox.items.add(juce::FlexItem(*comp)
                                 .withMinWidth(sliderWidth)
                                 .withMinHeight(componentHeight)
                                 .withMargin(juce::FlexItem::Margin(5)));
    }

    auto modEnvArea = area.removeFromTop(sliderHeight + 20);
    _modEnvFlexBox.performLayout(modEnvArea);
    
    // Gain slider
    _masterGainFlexBox.flexDirection = juce::FlexBox::Direction::row;
    _masterGainFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    _masterGainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    _masterGainFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    _masterGainFlexBox.items.clear();

    _masterGainFlexBox.items.add(juce::FlexItem(_masterGainSlider)
                           .withMinWidth(sliderWidth)
                           .withMinHeight(componentHeight)
                           .withMargin(juce::FlexItem::Margin(5)));

    auto masterGainArea = area.removeFromTop(sliderHeight + 20);
    _masterGainFlexBox.performLayout(masterGainArea);
}

void PluginEditor::setupOscillatorControls()
{
    setOscillatorType(_oscillatorAType, false);
    setOscillatorType(_oscillatorBType, false);
    setOscillatorType(_oscillatorSubType, true);
    _oscillatorAType.setText("Osc A");
    _oscillatorBType.setText("Osc B");
    _oscillatorSubType.setText("Sub");
    
    std::array<juce::ComboBox*, 3> comboBoxes = { &_oscillatorAType, &_oscillatorBType, &_oscillatorSubType };
    for (auto& cb : comboBoxes)
    {
        addAndMakeVisible(cb);
        cb->onChange = [this, cb]() {
            auto selected = static_cast<OscillatorUtils::WaveType>(cb->getSelectedId() - 1);
            if (cb == &_oscillatorAType) _processor.getSynthEngine().setOscillatorAType(selected);
            if (cb == &_oscillatorBType) _processor.getSynthEngine().setOscillatorBType(selected);
            if (cb == &_oscillatorSubType) _processor.getSynthEngine().setOscillatorSubType(selected);
        };
    }
}

void PluginEditor::setOscillatorType(juce::ComboBox& cb, bool isOscillatorSub)
{
    cb.addItem("Sine", 1);
    cb.addItem("Square", 2);
    if (!isOscillatorSub)
    {
        cb.addItem("Saw", 3);
        cb.addItem("Triangle", 4);
    }
    
    cb.setSelectedId(1);
}

void PluginEditor::setupEnvelopes()
{
    for (auto* comp : {
        &_ampEnvAttackSlider,
        &_ampEnvDecaySlider,
        &_ampEnvSustainSlider,
        &_ampEnvReleaseSlider
    })
    {
        addAndMakeVisible(comp);
    }

    for (auto* comp : {
        &_modEnvAttackSlider,
        &_modEnvDecaySlider,
        &_modEnvSustainSlider,
        &_modEnvReleaseSlider
    })
    {
        addAndMakeVisible(comp);
    }
}

void PluginEditor::setupGains()
{
    _masterGainSlider.getSlider().setRange(0.0, 1.0, 0.01);
    _masterGainSlider.getSlider().setValue(0.8);
    addAndMakeVisible(&_masterGainSlider);
    
    _oscillatorAGainSlider.getSlider().setRange(0.0, 1.0, 0.01);
    _oscillatorAGainSlider.getSlider().setValue(1.0);
    addAndMakeVisible(&_oscillatorAGainSlider);
    
    _oscillatorBGainSlider.getSlider().setRange(0.0, 1.0, 0.01);
    _oscillatorBGainSlider.getSlider().setValue(1.0);
    addAndMakeVisible(&_oscillatorBGainSlider);
    
    _oscillatorSubGainSlider.getSlider().setRange(0.0, 1.0, 0.01);
    _oscillatorSubGainSlider.getSlider().setValue(1.0);
    addAndMakeVisible(&_oscillatorSubGainSlider);
}

void PluginEditor::setupControlAttachments()
{
    auto& valueTreeState = _processor.getAudioProcessorValueTreeState();

    _oscillatorATypeAttachment   = std::make_unique<ComboBoxAttachment>(valueTreeState, ParameterIds::OscillatorATypeId, _oscillatorAType);
    _oscillatorBTypeAttachment   = std::make_unique<ComboBoxAttachment>(valueTreeState, ParameterIds::OscillatorBTypeId, _oscillatorBType);
    _oscillatorSubTypeAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, ParameterIds::OscillatorSubTypeId, _oscillatorSubType);
    
    _oscillatorAGainAttachment   = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::OscillatorAGainId, _oscillatorAGainSlider.getSlider());
    _oscillatorBGainAttachment   = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::OscillatorBGainId, _oscillatorBGainSlider.getSlider());
    _oscillatorSubGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::OscillatorSubGainId, _oscillatorSubGainSlider.getSlider());

    _ampEnvAttackAttachment  = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::AmplitudeEnvelopeAttackId, _ampEnvAttackSlider.getSlider());
    _ampEnvDecayAttachment   = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::AmplitudeEnvelopeDecayId, _ampEnvDecaySlider.getSlider());
    _ampEnvSustainAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::AmplitudeEnvelopeSustainId, _ampEnvSustainSlider.getSlider());
    _ampEnvReleaseAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::AmplitudeEnvelopeReleaseId, _ampEnvReleaseSlider.getSlider());

    _modEnvAttackAttachment  = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::ModulationEnvelopeAttackId, _modEnvAttackSlider.getSlider());
    _modEnvDecayAttachment   = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::ModulationEnvelopeDecayId, _modEnvDecaySlider.getSlider());
    _modEnvSustainAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::ModulationEnvelopeSustainId, _modEnvSustainSlider.getSlider());
    _modEnvReleaseAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::ModulationEnvelopeReleaseId, _modEnvReleaseSlider.getSlider());
    
    _masterGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, ParameterIds::MasterGainId, _masterGainSlider.getSlider());
}
