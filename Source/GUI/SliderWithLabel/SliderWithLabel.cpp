/*
  ==============================================================================

    SliderWithLabel.cpp
    Created: 3 Jul 2025 2:45:34pm
    Author:  Joshua Navon

  ==============================================================================
*/

#include "SliderWithLabel.h"
#include <JuceHeader.h>

SliderWithLabel::SliderWithLabel(const juce::String& labelText,
                                 juce::Slider::SliderStyle style,
                                 juce::Slider::TextEntryBoxPosition textboxPosition)
{
    _slider.setSliderStyle(style);
    _slider.setTextBoxStyle(textboxPosition, false, 50, 20);

    _label.setText(labelText, juce::dontSendNotification);
    _label.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(_slider);
    addAndMakeVisible(_label);
}

void SliderWithLabel::resized()
{
    auto bounds = getLocalBounds();
    const auto labelHeight = 20;
    const auto padding = 4;
    
    _label.setBounds(bounds.removeFromTop(labelHeight));
    bounds.removeFromTop(padding);
    _slider.setBounds(bounds);
}

juce::Slider& SliderWithLabel::getSlider() { return _slider; }
juce::Label& SliderWithLabel::getLabel() { return _label; }
