#include "MainComponent.h"

MainComponent::MainComponent()
{
	setAudioChannels (maxInputs, maxInputs);
	
	juce::AudioDeviceManager::AudioDeviceSetup setup;
	deviceManager.getAudioDeviceSetup (setup);
	setup.bufferSize = 64;
	setup.sampleRate = 0.0;
	deviceManager.setAudioDeviceSetup(setup, true);


	deviceManager.addChangeListener (this);

	mixers.reserve (numMixers);

	for (int i = 0; i < numMixers; ++i)
	{
		mixers.push_back (std::make_unique<Mixer> (maxInputs));

		auto* ui = new MixerComponent (*mixers.back());
		mixerUIs.push_back (ui);

		tabs.addTab ("Mix " + juce::String (i + 1),
					 juce::Colours::grey,
					 ui,
					 true);
	}
	
	if (auto* device = deviceManager.getCurrentAudioDevice())
	{
		juce::BigInteger activeChannels = device->getActiveInputChannels();
		std::vector<int> activeInputIndices;
		for (int ch = 0; ch <= activeChannels.getHighestBit(); ++ch)
			{
			if (activeChannels[ch])
				{
				activeInputIndices.push_back(ch);
				++activeInputCount;
				}
			}

		// Tell each mixer the actual indices
		for (auto* ui : mixerUIs)
			ui->setActiveInputs(activeInputIndices);
	
		
	}
	updateMixerRoutings();

	deviceSelector.reset (new juce::AudioDeviceSelectorComponent (
		deviceManager,
		0, maxInputs,
		0, maxInputs,
		false, false, false, true));
	deviceSelector->setItemHeight(20);

	addAndMakeVisible (deviceSelector.get());
	addAndMakeVisible (tabs);

	setSize (1000, 600);
}

MainComponent::~MainComponent()
{
	deviceManager.removeChangeListener (this);
	shutdownAudio();
}

void MainComponent::prepareToPlay (int, double)
{
}

void MainComponent::releaseResources()
{
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& info)
{
	juce::ScopedNoDenormals noDenormals;

	auto& buffer = *info.buffer;
	const int numSamples = info.numSamples;

	// Safety
	if (numSamples <= 0 || activeInputCount <= 0)
	{
		buffer.clear();
		return;
	}

	// --------------------------------------------------
	// Copy input buffer BEFORE clearing outputs
	// --------------------------------------------------
	juce::AudioBuffer<float> inputCopy;
	inputCopy.makeCopyOf(buffer);

	// --------------------------------------------------
	// Clear output buffer
	// --------------------------------------------------
	buffer.clear();

	auto* device = deviceManager.getCurrentAudioDevice();
	if (device == nullptr)
		return;

	// --------------------------------------------------
	// Collect active input channels
	// --------------------------------------------------
	juce::BigInteger activeInputs = device->getActiveInputChannels();
	std::vector<int> inputChannels;

	for (int ch = 0; ch <= activeInputs.getHighestBit(); ++ch)
		if (activeInputs[ch])
			inputChannels.push_back(ch);

	if (inputChannels.empty())
		return;

	const int numOutputChannels = buffer.getNumChannels();
	const int mixersToUse = juce::jmin((int) mixers.size(),
									   (int) mixerRoutings.size());

	// --------------------------------------------------
	// Process each mixer
	// --------------------------------------------------
	for (int m = 0; m < mixersToUse; ++m)
	{
		const int outL = juce::jlimit(0,
									  numOutputChannels - 1,
									  mixerRoutings[m].leftOutput);

		const int outR = juce::jlimit(0,
									  numOutputChannels - 1,
									  mixerRoutings[m].rightOutput);

		float* left  = buffer.getWritePointer(outL);
		float* right = buffer.getWritePointer(outR);

		// --------------------------------------------------
		// Mix inputs
		// --------------------------------------------------
		for (int s = 0; s < numSamples; ++s)
		{
			float sum = 0.0f;

			for (int ch : inputChannels)
			{
				if (ch < inputCopy.getNumChannels())
				{
					const float* in = inputCopy.getReadPointer(ch);
					sum += in[s] * mixers[m]->getGain(ch);
				}
			}

			// Normalize to prevent attenuation with many channels
			sum /= (float) inputChannels.size();

			left[s]  += sum;
			right[s] += sum;
		}

		// --------------------------------------------------
		// RMS → dBFS metering
		// --------------------------------------------------
		float sumL = 0.0f;
		float sumR = 0.0f;

		for (int s = 0; s < numSamples; ++s)
		{
			sumL += left[s]  * left[s];
			sumR += right[s] * right[s];
		}

		const float rmsL = std::sqrt(sumL / (float) numSamples);
		const float rmsR = std::sqrt(sumR / (float) numSamples);

		// Convert to dBFS
		const float dbL = juce::Decibels::gainToDecibels(rmsL, -100.0f);
		const float dbR = juce::Decibels::gainToDecibels(rmsR, -100.0f);

		// Meter range
		constexpr float minDb = -60.0f;
		constexpr float maxDb = 0.0f;

		const float meterL = juce::jlimit(0.0f, 1.0f,
			juce::jmap(dbL, minDb, maxDb, 0.0f, 1.0f));

		const float meterR = juce::jlimit(0.0f, 1.0f,
			juce::jmap(dbR, minDb, maxDb, 0.0f, 1.0f));

		mixers[m]->pushMeterValue(meterL, meterR);
	}
}



void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
	if (source == &deviceManager)
	{
	activeInputCount = 0;
	if (auto* device = deviceManager.getCurrentAudioDevice())
	{
		juce::BigInteger activeChannels = device->getActiveInputChannels();
		std::vector<int> activeInputIndices;

	for (int ch = 0; ch <= activeChannels.getHighestBit(); ++ch){
		if (activeChannels[ch]){
			activeInputIndices.push_back(ch);
			++activeInputCount;
		}
	}

		// Tell each mixer the actual indices
		for (auto* ui : mixerUIs)
			ui->setActiveInputs(activeInputIndices);
	}

	updateMixerRoutings();
	}
}

void MainComponent::resized()
{
	auto area = getLocalBounds();

	deviceSelector->setBounds (area.removeFromTop (250));
	tabs.setBounds (area);
}

void MainComponent::updateMixerRoutings() {
	mixerRoutings.clear();
	if (auto* device = deviceManager.getCurrentAudioDevice())
	{
		const int numOutputs = device->getActiveOutputChannels().countNumberOfSetBits();
		const int maxMixers = numOutputs / 2;

		for (int i = 0; i < juce::jmin (numMixers, maxMixers); ++i)
		{
			MixerRouting r;
			r.leftOutput  = i * 2;
			r.rightOutput = i * 2 + 1;
			mixerRoutings.push_back (r);
		}
	}
}

