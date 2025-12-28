/*
  ==============================================================================

    FaderLookAndFeel.cpp
    Created: 26 Dec 2025 12:15:04pm
    Author:  Jesse Emmot

  ==============================================================================
*/

#include "FaderLookAndFeel.h"
#include "BinaryData.h"

FaderLookAndFeel::FaderLookAndFeel()
{
	channelImage = juce::ImageCache::getFromMemory(
		BinaryData::channelBigv3_png,
		BinaryData::channelBigv3_pngSize);

	knobImage = juce::ImageCache::getFromMemory(
		BinaryData::fader_icon_gold_png,
		BinaryData::fader_icon_gold_pngSize);
}

void FaderLookAndFeel::drawLinearSlider(juce::Graphics& g,
									   int x, int y, int width, int height,
									   float sliderPos,
									   float minSliderPos,
									   float maxSliderPos,
									   const juce::Slider::SliderStyle style,
									   juce::Slider& slider)
{
	if (style != juce::Slider::LinearVertical)
		return;

	// Draw fader channel background
	g.drawImageWithin(channelImage, x, y, width, height, juce::RectanglePlacement::stretchToFit);
	//g.drawImageWithin(channelImage,
	//				  x,
	//				  y,
	//				  width,
	//				  height,
	//				  juce::RectanglePlacement::stretchToFit);

	// --- Knob positioning ---
	// --- Desired knob width relative to channel ---
	const float knobWidthRatio = 0.65f;   // 🔧 tweak (0.5–0.8 typical)
	const float maxKnobHeight  = height * 0.25f;

	// Scale knob proportionally
	float scale = (width * knobWidthRatio) / knobImage.getWidth();
	float scaledW = knobImage.getWidth()  * scale;
	float scaledH = knobImage.getHeight() * scale;

	// Clamp height so it never dominates the channel
	if (scaledH > maxKnobHeight)
	{
		float heightScale = maxKnobHeight / scaledH;
		scaledW *= heightScale;
		scaledH *= heightScale;
	}

	// Center horizontally
	float knobX = x + (width - scaledW) * 0.5f - knobXOffset;

	// Use sliderPos for vertical position
	float knobY = sliderPos - scaledH * 0.5f;

	// Clamp travel
	knobY = juce::jlimit((float) y,
						 (float) (y + height - scaledH),
						 knobY);

	// Draw scaled knob
	g.drawImage(knobImage,
				juce::Rectangle<float>(knobX, knobY, scaledW, scaledH));
	
	
	auto labelArea = juce::Rectangle<int>(
		x,
		y + height - 20,
		width,
		20
	);

	juce::FontOptions fontOptions(10.0f, juce::Font::bold);

	juce::Font labelFont (fontOptions);

	g.setFont (labelFont);
	g.setColour (juce::Colours::white.withAlpha (0.7f));

	g.drawFittedText (
		slider.getName(),
		juce::Rectangle<int> (x, knobY + scaledH - 18, width, 18),
		juce::Justification::centred,
		1
	);
}

