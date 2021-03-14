/*
  ==============================================================================

	PatLookAndFeel.h
	Created: 8 Mar 2021 6:46:18pm
	Author:  pat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace PatColours
{
	const Colour pink = Colour(232, 97, 122);
	const Colour grey = Colour(230, 230, 230);

	const Colour lightPink = pink.brighter(0.4f);
	const Colour darkPink = pink.darker(0.5f);
};

class PatLookAndFeel : public LookAndFeel_V4
{
public:

	PatLookAndFeel()
	{
		LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Berlin Sans FB");

		//// COLOURS
		setColour(ResizableWindow::backgroundColourId, Colours::white);

		// LABEL
		setColour(Label::textColourId, PatColours::pink);
		setColour(Label::backgroundColourId, Colours::transparentWhite);
		setColour(Label::outlineColourId, Colours::transparentWhite);

		// TEXT EDITOR
		setColour(TextEditor::textColourId, PatColours::pink);
		setColour(TextEditor::outlineColourId, PatColours::pink);
		setColour(TextEditor::backgroundColourId, Colours::white);
		setColour(TextEditor::highlightColourId, PatColours::pink);
		setColour(TextEditor::highlightedTextColourId, Colours::white);
		setColour(TextEditor::focusedOutlineColourId, PatColours::pink);

		// SLIDER
		setColour(Slider::rotarySliderFillColourId, PatColours::pink);
		setColour(Slider::rotarySliderOutlineColourId, Colours::pink);
		setColour(Slider::thumbColourId, PatColours::pink);
		setColour(Slider::textBoxTextColourId, PatColours::pink);
		setColour(Slider::textBoxOutlineColourId, PatColours::pink);

		// TOGGLE
		setColour(ToggleButton::tickDisabledColourId, Colours::black);
		setColour(ToggleButton::tickColourId, PatColours::pink);
		setColour(ToggleButton::textColourId, Colours::black);

		// MIDI KEYBOARD
		setColour(MidiKeyboardComponent::keyDownOverlayColourId, PatColours::pink);
		setColour(MidiKeyboardComponent::mouseOverKeyOverlayColourId, PatColours::pink);
		setColour(MidiKeyboardComponent::upDownButtonArrowColourId, PatColours::pink);
		setColour(MidiKeyboardComponent::upDownButtonBackgroundColourId, Colours::white);
		setColour(MidiKeyboardComponent::keySeparatorLineColourId, PatColours::lightPink);
		setColour(MidiKeyboardComponent::blackNoteColourId, PatColours::lightPink);
		setColour(MidiKeyboardComponent::shadowColourId, Colours::transparentBlack);
		setColour(MidiKeyboardComponent::textLabelColourId, PatColours::pink);

		// POPUP MENU
		setColour(PopupMenu::backgroundColourId, Colours::white);
		setColour(PopupMenu::textColourId, PatColours::pink);
		setColour(PopupMenu::highlightedBackgroundColourId, PatColours::pink);
		setColour(PopupMenu::highlightedTextColourId, Colours::white);
	}

	// GENERAL
	Rectangle<float> getCentreRect(const Rectangle<float>& bounds, const float margin)
	{
		const float halfX = bounds.getWidth() / 2.0f;
		const float halfY = bounds.getHeight() / 2.0f;

		return Rectangle<float>(
			bounds.getX() + halfX * (1.0f - margin),
			bounds.getY() + halfY * (1.0f - margin),
			bounds.getWidth() * margin,
			bounds.getHeight() * margin
			);
	}

	Rectangle<float> getCentreRectFromZero(const float width, const float height, const float marginX, const float marginY)
	{
		return Rectangle<float>(
			width * (1.0f - marginX) / 2.0f,
			height * (1.0f - marginY) / 2.0f,
			width * marginX,
			height * marginY
			);
	}

	// BUTTON, GENERAL
	void drawButton(Graphics& g, Button* b, bool isHighlighted, bool isDown)
	{
		const int margin = 5;
		const auto mainRect = b->getLocalBounds().reduced(margin);

		Colour fontColour = Colours::white;

		g.setColour(PatColours::pink);
		g.fillRect(mainRect.expanded(1));

		if (isHighlighted)
		{
			g.setColour(PatColours::lightPink);
			g.setOpacity(0.5);
			g.fillRect(mainRect.expanded(4));
			g.setOpacity(1.0);
		}

		if (isDown || b->getToggleState())
		{
			g.setColour(PatColours::darkPink);
			g.fillRect(mainRect);

			if (b->getToggleState())
			{
				g.setColour(PatColours::pink);
				g.fillRect(mainRect.reduced(2));
			}
		}
		else
		{
			g.setColour(Colours::white);
			g.fillRect(mainRect);
			fontColour = PatColours::pink;
		}

		g.setColour(fontColour);
		g.drawText(b->getButtonText(), mainRect, Justification::centred, false);
	}

	// TOGGLE BUTTON
	void drawToggleButton(Graphics& g, ToggleButton& b, bool isHighlighted, bool isDown) override
	{
		drawButton(g, &b, isHighlighted, isDown);
	}

	// TEXT BUTTON
	void drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColour, bool isHighlighted, bool isDown) override
	{
		drawButton(g, &b, isHighlighted, isDown);
	}

	void drawButtonText(Graphics& g, TextButton& b, bool isHighlighted, bool isDown) override
	{
		return;
	}

};

//==============================================================================

class PatTextButton : public TextButton
{
public:

	void clicked() override
	{
	}

	void clicked(const ModifierKeys& m) override
	{
		if (!m.isRightButtonDown())
		{
			clicked();
		}
	}
};


class PatToggleButton : public ToggleButton
{
public:

	enum Types
	{
		Random,
		Knob
	};

	PatToggleButton() = default;

	PatToggleButton(const String& text, const Types _type = Types::Random, const uint8 _knobId = 0)
		: ToggleButton(text)
		, type(_type)
		, knobId(_knobId)
	{}

	void internalClickCallback(const ModifierKeys& m) override
	{
		if (m.isRightButtonDown() && onRightClick != nullptr)
		{
			onRightClick(this);
		}
		else
		{
			Button::internalClickCallback(m);
		}
	}

	Types type{ Types::Random };
	uint8 knobId{ 0 };
	std::function<void(PatToggleButton*)> onRightClick;
};