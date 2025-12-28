#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "Mixer.h"
#include "MixerComponent.h"


struct MixerRouting
{
	int leftOutput = 0;
	int rightOutput = 1;
};

class MainComponent  : public juce::AudioAppComponent,
					   public juce::ChangeListener
{
public:
	MainComponent();
	~MainComponent() override;

	void prepareToPlay (int samplesPerBlockExpected,
						double sampleRate) override;

	void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void resized() override;

	void changeListenerCallback (juce::ChangeBroadcaster* source) override;
	
	private:
	void updateMixerRoutings();

private:
	static constexpr int maxInputs = 64;
	static constexpr int numMixers = 8;
	static constexpr int outputStartChannel = 16;

	int activeInputCount = 0;

	juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };

	std::vector<std::unique_ptr<Mixer>> mixers;
	std::vector<MixerComponent*> mixerUIs;
	std::vector<MixerRouting> mixerRoutings;

	std::unique_ptr<juce::AudioDeviceSelectorComponent> deviceSelector;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
