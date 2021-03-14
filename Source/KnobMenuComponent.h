/*
  ==============================================================================

	KnobMenuComponent.h
	Created: 10 Mar 2021 8:10:26pm
	Author:  pat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class KnobMenuComponent : public juce::Component
{
public:

	KnobMenuComponent()
	{
		for (int i = 0; i < 16; ++i)
		{
			knobs.add(new Slider(String(i + 1)));

			auto* knob = knobs.getLast();
			addAndMakeVisible(knob);
			knob->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
			knob->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
			knob->setRange(0.0, 1.0f);
		}
	}

	void resized() override
	{
		const int quarterY = (int)floor((float)getHeight() / 4.0f);
		const int unitX = (int)floor((float)getWidth() / 4.0f);

		for (int i = 0; i < 16; ++i)
		{
			knobs[i]->setBounds(
				unitX * (i % 4)
				, (int)floor(i / 4.0f) * quarterY
				, unitX
				, quarterY
			);
		}
	}

	OwnedArray<Slider> knobs;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobMenuComponent)
};
