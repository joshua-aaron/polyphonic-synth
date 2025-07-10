/*
  ==============================================================================

    SliderWithLabel.h
    Created: 3 Jul 2025 2:45:34pm
    Author:  Joshua Navon

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SliderWithLabel : public juce::Component
{
public:
    SliderWithLabel(const juce::String& labelText,
                    juce::Slider::SliderStyle style = juce::Slider::LinearVertical,
                    juce::Slider::TextEntryBoxPosition textboxPosition = juce::Slider::TextBoxBelow);

    void resized() override;

    juce::Slider& getSlider();
    juce::Label& getLabel();

private:
    juce::Slider _slider;
    juce::Label _label;
};
