/*
  ==============================================================================

    FaderLookAndFeel.h
    Created: 25 Dec 2025 7:07:30pm
    Author:  Jesse Emmot

  ==============================================================================
*/
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FaderLookAndFeel : public juce::LookAndFeel_V4
{
public:
	FaderLookAndFeel();

	void drawLinearSlider(juce::Graphics& g,
							  int x, int y, int width, int height,
							  float sliderPos,
							  float minSliderPos,
							  float maxSliderPos,
							  const juce::Slider::SliderStyle style,
							  juce::Slider& slider) override;

private:
	juce::Colour trackColour;
	juce::Image channelImage;
	juce::Image knobImage;
	const int textAreaHeight = 20;
	const int knobXOffset = 3;
};



