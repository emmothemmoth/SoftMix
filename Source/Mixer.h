#pragma once

#include <juce_core/juce_core.h>
#include <atomic>

class Mixer
{
public:
	explicit Mixer (int maxInputs);

	void setGain (int input, float value);
	float getGain (int input) const;

	void setActiveInputs (int count);
	int getActiveInputs() const;

	void pushMeterValue (float left, float right);
	float getMeterL() const;
	float getMeterR() const;

private:
	int maxInputs = 0;
	int activeInputs = 0;

	std::unique_ptr<std::atomic<float>[]> gains;

	std::atomic<float> meterL { 0.0f };
	std::atomic<float> meterR { 0.0f };
};
