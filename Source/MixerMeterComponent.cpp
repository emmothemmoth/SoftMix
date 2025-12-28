/*
  ==============================================================================

    MixerMeterComponent.cpp
    Created: 27 Dec 2025 12:28:41pm
    Author:  Jesse Emmot

  ==============================================================================
*/

#include "MixerMeterComponent.h"

MixerMeterComponent::MixerMeterComponent() {}

void MixerMeterComponent::setLevels(float left, float right)
{
	leftLevel = left;
	rightLevel = right;
	repaint(); // triggers visual update
}

void MixerMeterComponent::paint(juce::Graphics& g)
{
	auto area = getLocalBounds().toFloat();
	g.fillAll(juce::Colours::black); // background

	// draw left meter
	auto leftRect = juce::Rectangle<float>(
		area.getX() + 4,
		area.getY() + (1.0f - leftLevel) * area.getHeight(),
		area.getWidth() / 2 - 6,
		leftLevel * area.getHeight()
	);
	g.setColour(juce::Colours::green);
	g.fillRect(leftRect);

	// draw right meter
	auto rightRect = juce::Rectangle<float>(
		area.getX() + area.getWidth() / 2 + 2,
		area.getY() + (1.0f - rightLevel) * area.getHeight(),
		area.getWidth() / 2 - 6,
		rightLevel * area.getHeight()
	);
	g.setColour(juce::Colours::green);
	g.fillRect(rightRect);
}

