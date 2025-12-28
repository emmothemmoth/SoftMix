/*
  ==============================================================================

    MixerMeterComponent.h
    Created: 27 Dec 2025 12:28:41pm
    Author:  Jesse Emmot

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class MixerMeterComponent : public juce::Component
{
public:
	MixerMeterComponent();

	void setLevels(float left, float right);
	void paint(juce::Graphics& g) override;

private:
	float leftLevel = 0.0f;
	float rightLevel = 0.0f;
};
