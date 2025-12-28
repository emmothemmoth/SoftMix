#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include <vector>
#include <memory>

#include "Mixer.h"
#include "FaderLookAndFeel.h"
#include "MixerMeterComponent.h"

struct ChannelStrip
{
	std::unique_ptr<juce::Slider> fader;
	std::unique_ptr<juce::Label> label;
	bool Muted = false;
};

class MixerComponent : public juce::Component,
					   public juce::Timer
{
public:
	explicit MixerComponent(Mixer& mixer);
	~MixerComponent() override;
	
	void mouseWheelMove (const juce::MouseEvent& event,
						 const juce::MouseWheelDetails& wheel) override;

	void setActiveInputs (const std::vector<int>& inputChannels, const juce::StringArray& channelNames);
	void resized() override;
	void timerCallback() override;
	
	std::unique_ptr<MixerMeterComponent>& GetMeters() {return meters;}

private:
	Mixer& mixer;

	//std::vector<std::unique_ptr<juce::Slider>> faders;
	std::vector<ChannelStrip> channels;
	std::unique_ptr<FaderLookAndFeel> faderLookAndFeel;
	
	juce::Viewport viewport;
	juce::Component faderContainer;
	std::unique_ptr<MixerMeterComponent> meters;
	static constexpr int meterWidth = 40;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerComponent)
};

