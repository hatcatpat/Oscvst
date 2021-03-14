/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "OscObject.h"
#include "OscObjectComponent.h"
#include "KnobMenuComponent.h"
#include "PatLookAndFeel.h"
#include "SettingsMenuComponent.h"

//==============================================================================
static enum class Scenes
{
	OscEdit,
	KnobMenu,
	SettingsMenu
};

//==============================================================================
class OscvstMenuBar
	: public Component
	, public Button::Listener
{
public:

	OscvstMenuBar()
	{
		addAndMakeVisible(oscEditButton);
		oscEditButton.setButtonText("osc edit");
		oscEditButton.addListener(this);

		addAndMakeVisible(knobMenuButton);
		knobMenuButton.setButtonText("knobs");
		knobMenuButton.addListener(this);

		addAndMakeVisible(settingsMenuButton);
		settingsMenuButton.setButtonText("settings");
		settingsMenuButton.addListener(this);
	}

	void buttonClicked(Button* b) override
	{
		if (b == &oscEditButton)
		{
			setScene(Scenes::OscEdit);
			if (setSceneCallback) setSceneCallback(Scenes::OscEdit);
		}
		else if (b == &knobMenuButton)
		{
			setScene(Scenes::KnobMenu);
			if (setSceneCallback) setSceneCallback(Scenes::KnobMenu);
		}
		else if (b == &settingsMenuButton)
		{
			setScene(Scenes::SettingsMenu);
			if (setSceneCallback) setSceneCallback(Scenes::SettingsMenu);
		}
	}

	void setScene(const Scenes scene)
	{
		switch (scene)
		{

		case Scenes::OscEdit:
			oscEditButton.setToggleState(true, dontSendNotification);
			knobMenuButton.setToggleState(false, dontSendNotification);
			settingsMenuButton.setToggleState(false, dontSendNotification);
			break;

		case Scenes::KnobMenu:
			oscEditButton.setToggleState(false, dontSendNotification);
			knobMenuButton.setToggleState(true, dontSendNotification);
			settingsMenuButton.setToggleState(false, dontSendNotification);
			break;

		case Scenes::SettingsMenu:
			oscEditButton.setToggleState(false, dontSendNotification);
			knobMenuButton.setToggleState(false, dontSendNotification);
			settingsMenuButton.setToggleState(true, dontSendNotification);
			break;

		}
	}

	void resized() override
	{
		FlexBox fb;
		fb.items.addArray({
			FlexItem(oscEditButton).withFlex(1)
			, FlexItem(knobMenuButton).withFlex(1)
			, FlexItem(settingsMenuButton).withFlex(1)
		});

		fb.performLayout(getLocalBounds().toFloat());
	}

	std::function<void(const Scenes)> setSceneCallback;

private:
	TextButton oscEditButton, knobMenuButton, settingsMenuButton;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscvstMenuBar);
};

//==============================================================================
class OscKeyboardComponent : public MidiKeyboardComponent
{
public:

	OscKeyboardComponent(MidiKeyboardState& state, Orientation orientation, MidiOscMap& _oscMap)
		: MidiKeyboardComponent(state, orientation)
		, oscMap(_oscMap)
	{}

	void drawBlackNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown, bool isOver, Colour noteFillColour) override
	{
		MidiKeyboardComponent::drawBlackNote(midiNoteNumber, g, area, isDown, isOver, noteFillColour);
		drawIndicator(midiNoteNumber, g, area);
	}

	void drawWhiteNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown, bool isOver, Colour lineColour, Colour textColour) override
	{
		MidiKeyboardComponent::drawWhiteNote(midiNoteNumber, g, area, isDown, isOver, lineColour, textColour);
		drawIndicator(midiNoteNumber, g, area);
	}

	void drawIndicator(int midiNoteNumber, Graphics& g, Rectangle<float> area) const
	{
		if (lastKey == midiNoteNumber)
		{
			g.setColour(PatColours::darkPink);
			g.fillEllipse(area.getCentreX() - indicatorRadius / 2.0f, area.getBottomLeft().y - indicatorRadius * 2.0f, indicatorRadius, indicatorRadius);
		}

		if(oscMap.count(midiNoteNumber) != 0)
		{
			g.setColour(PatColours::pink);
			g.fillEllipse(area.getCentreX() - indicatorRadius / 2.0f, area.getBottomLeft().y - indicatorRadius * 3.0f, indicatorRadius, indicatorRadius);
		}
	}

	bool mouseDownOnKey(int midiNoteNumber, const MouseEvent& e) override
	{
		if (e.mods.isRightButtonDown()) return true;

		if (midiNoteNumber != lastKey)
		{
			lastKey = midiNoteNumber;
			repaint();
			if (mouseDownOnKeyCallback) mouseDownOnKeyCallback(lastKey);
		}

		return false;
	}

	bool mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e) override
	{
		return e.mods.isRightButtonDown();
	}

	void mouseUpOnKey(int midiNoteNumber, const MouseEvent& e) override {}

	int lastKey = 0;
	MidiOscMap& oscMap;
	std::function<void(const int)> mouseDownOnKeyCallback;

private:
	const float indicatorRadius = 8;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscKeyboardComponent);
};

//==============================================================================
/**
*/
class OscvstAudioProcessorEditor
	: public juce::AudioProcessorEditor
{

public:
	OscvstAudioProcessorEditor(OscvstAudioProcessor&);
	~OscvstAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

	OscObject* addOscObject(const int note, const String& path);
	void removeOscObject(const int note);

	void setScene(const Scenes _scene);

private:
	OscvstAudioProcessor& audioProcessor;
	PatLookAndFeel patLookAndFeel;

	MidiOscMap& oscMap;
	OscObjectComponent oscObjectComponent;
	OscKeyboardComponent keyboardComponent;
	KnobMenuComponent knobMenuComponent;
	SettingsMenuComponent settingsMenuComponent;
	OwnedArray<SliderParameterAttachment> knobs;
	OscvstMenuBar menuBar;
	Scenes scene;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscvstAudioProcessorEditor);
};
