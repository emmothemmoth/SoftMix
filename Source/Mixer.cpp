#include "Mixer.h"

Mixer::Mixer (int maxInputs_)
	: maxInputs (maxInputs_),
	  activeInputs (maxInputs_)
{
	gains.reset (new std::atomic<float>[maxInputs]);

	for (int i = 0; i < maxInputs; ++i)
		gains[i].store (0.0f, std::memory_order_relaxed);
}

void Mixer::setGain (int input, float value)
{
	if (input < maxInputs)
		gains[input].store (value, std::memory_order_relaxed);
}

float Mixer::getGain (int input) const
{
	if (input < maxInputs)
		return gains[input].load (std::memory_order_relaxed);

	return 0.0f;
}

void Mixer::setActiveInputs (int count)
{
	activeInputs = juce::jlimit (0, maxInputs, count);
}

int Mixer::getActiveInputs() const
{
	return activeInputs;
}

void Mixer::pushMeterValue (float left, float right)
{
	meterL.store (left,  std::memory_order_relaxed);
	meterR.store (right, std::memory_order_relaxed);
}

float Mixer::getMeterL() const
{
	return meterL.load (std::memory_order_relaxed);
}

float Mixer::getMeterR() const
{
	return meterR.load (std::memory_order_relaxed);
}
