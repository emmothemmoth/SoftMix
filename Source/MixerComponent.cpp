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
	for (auto& channel : channels)
		{
			channel.fader->setLookAndFeel(nullptr);
			channel.label->setLookAndFeel(nullptr);
		}
}

void MixerComponent::mouseWheelMove (const juce::MouseEvent& event,
									 const juce::MouseWheelDetails& wheel)
{
	viewport.mouseWheelMove(event, wheel);
}

void MixerComponent::setActiveInputs (const std::vector<int>& inputChannels, const juce::StringArray& channelNames)
{
	channels.clear();

	for (int i = 0; i < inputChannels.size(); ++i)
		{
			int ch = inputChannels[i];
			ChannelStrip channel;
			channel.fader = std::make_unique<juce::Slider>();
			auto* sliderPtr = channel.fader.get();

			channel.fader->setSliderStyle(juce::Slider::LinearVertical);
			channel.fader->setRange(0.0, 1.0, 0.001);
			channel.fader->setValue(0.0);
			channel.fader->setSkewFactorFromMidPoint(0.25); // optional audio taper
			channel.fader->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			channel.fader->setLookAndFeel(faderLookAndFeel.get());
			channel.fader->setInterceptsMouseClicks(true, false);
			channel.fader->setMouseDragSensitivity(1);
			channel.fader->setVelocityBasedMode(false);
			channel.fader->setScrollWheelEnabled(false);
			channel.fader->setName(channelNames[ch]);
			channel.fader->onValueChange = [this, ch, sliderPtr]()
			{
				mixer.setGain(ch, (float) sliderPtr->getValue());
			};
			faderContainer.addAndMakeVisible(*channel.fader);
		
			channel.label = std::make_unique<juce::Label>();
			channel.label->setText(channelNames[ch], juce::NotificationType::dontSendNotification);
			channel.label->setEditable(false, true, false);
			channel.label->setJustificationType(juce::Justification::centred);
			channel.label->setInterceptsMouseClicks(true, false);
			channel.label->setLookAndFeel(faderLookAndFeel.get());
			auto* labelPtr = channel.label.get();
			channel.label->onTextChange = [labelPtr, sliderPtr]()
			{
				sliderPtr->setName(labelPtr	->getText());
			};
			faderContainer.addAndMakeVisible(*channel.label);
			channels.push_back(std::move(channel));
		}

		resized();
}

void MixerComponent::resized()
{
	const int channelWidth = 48;
	const int channelSpacing = 4;
	const int labelHeight = 22;
	const int totalFadersWidth = static_cast<int>((channelWidth + channelSpacing) * channels.size());

	// faderContainer inside viewport
	faderContainer.setBounds(0, 0, totalFadersWidth, getHeight());
	viewport.setBounds(0, 0, getWidth() - meterWidth, getHeight());

	for (int i = 0; i < (int) channels.size(); ++i)
	{
		auto xPos = i * (channelWidth + channelSpacing);
		channels[i].fader->setBounds(i * (channelWidth + channelSpacing),
							 20,
							 channelWidth,
							 getHeight() - 20);
		channels[i].label->setBounds(xPos, getHeight() - labelHeight, channelWidth, labelHeight);
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

