#include "MixerComponent.h"

MixerComponent::MixerComponent(Mixer& m)
	: mixer(m)
{
	startTimerHz(30);
	faderLookAndFeel = std::make_unique<FaderLookAndFeel>();
	meters = std::make_unique<MixerMeterComponent>();
	addAndMakeVisible(*meters);
	
	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&faderContainer, false);
	viewport.setScrollBarsShown(true, false);
	viewport.setInterceptsMouseClicks(false, true);
	faderContainer.setInterceptsMouseClicks (false, true);
}

MixerComponent::~MixerComponent()
{
	// IMPORTANT: Detach LookAndFeel before destruction
	for (auto& fader : faders)
		fader->setLookAndFeel(nullptr);
}

void MixerComponent::mouseWheelMove (const juce::MouseEvent& event,
									 const juce::MouseWheelDetails& wheel)
{
	viewport.mouseWheelMove(event, wheel);
}

void MixerComponent::setActiveInputs (const std::vector<int>& inputChannels)
{
	faders.clear();

	for (int i = 0; i < inputChannels.size(); ++i)
		{
			int ch = inputChannels[i];
			auto slider = std::make_unique<juce::Slider>();
			auto* sliderPtr = slider.get();

			slider->setSliderStyle(juce::Slider::LinearVertical);
			slider->setRange(0.0, 1.0, 0.001);
			slider->setValue(0.0);
			slider->setSkewFactorFromMidPoint(0.25); // optional audio taper
			slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			slider->setLookAndFeel(faderLookAndFeel.get());
			slider->setInterceptsMouseClicks(true, false);
			slider->setMouseDragSensitivity(1);
			slider->setVelocityBasedMode(false);
			slider->setScrollWheelEnabled(false);
			slider->setName("In " + juce::String(ch + 1));

			slider->onValueChange = [this, ch, sliderPtr]()
			{
				mixer.setGain(ch, (float) sliderPtr->getValue());
			};

			faderContainer.addAndMakeVisible(*slider);
			faders.push_back(std::move(slider));
		}

		resized();
}

void MixerComponent::resized()
{
	const int channelWidth = 48;
	const int channelSpacing = 4;
	const int totalFadersWidth = static_cast<int>((channelWidth + channelSpacing) * faders.size());

	// faderContainer inside viewport
	faderContainer.setBounds(0, 0, totalFadersWidth, getHeight());
	viewport.setBounds(0, 0, getWidth() - meterWidth, getHeight());

	for (int i = 0; i < (int) faders.size(); ++i)
	{
		faders[i]->setBounds(i * (channelWidth + channelSpacing),
							 20,
							 channelWidth,
							 getHeight() - 20);
	}

	// Meter area to the right of the faders
	meters->setBounds(getWidth() - meterWidth, 0, meterWidth, getHeight());
}

void MixerComponent::timerCallback()
{
	meters->setLevels(
		   mixer.getMeterL(),
		   mixer.getMeterR()
	   );
}

