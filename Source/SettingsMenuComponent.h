/*
  ==============================================================================

	SettingsMenuComponent.h
	Created: 13 Mar 2021 4:40:10pm
	Author:  pat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SettingsMenuComponent
	: public Component
	, public TextEditor::Listener
{
public:

	SettingsMenuComponent(OscvstAudioProcessor& _audioProcessor)
		: audioProcessor(_audioProcessor)
		, osc(_audioProcessor.osc)
	{
		// LABELS
		addressLabel.setText("address", dontSendNotification);
		addressLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(addressLabel);

		portLabel.setText("port", dontSendNotification);
		portLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(portLabel);

		intervalLabel.setText("interval", dontSendNotification);
		intervalLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(intervalLabel);

		// INPUTS
		const String& inputRestrictions = "0123456789";

		addressInput.setJustification(Justification::verticallyCentred);
		addressInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		addressInput.setText(osc.address, dontSendNotification);
		addressInput.setInputRestrictions(0, inputRestrictions + ".");
		addressInput.addListener(this);
		addAndMakeVisible(addressInput);

		portInput.setJustification(Justification::verticallyCentred);
		portInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		portInput.setText(String(osc.port), dontSendNotification);
		portInput.setInputRestrictions(0, inputRestrictions);
		portInput.addListener(this);
		addAndMakeVisible(portInput);

		intervalInput.setJustification(Justification::verticallyCentred);
		intervalInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		intervalInput.setText(String(osc.interval), dontSendNotification);
		intervalInput.setInputRestrictions(0, inputRestrictions);
		intervalInput.addListener(this);
		addAndMakeVisible(intervalInput);
	}

	~SettingsMenuComponent() override
	{
	}

	void updateInputs(TextEditor& e)
	{
		const auto* ep = &e;

		if (ep == &addressInput)
		{
			String& text = e.getText();
			if (text.isEmpty())
			{
				text = "127.0.0.1";
				e.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
				e.setText(text, dontSendNotification);
			}
			osc.address = text;
			osc.sender.disconnect();
			osc.sender.connect(osc.address, osc.port);
		}
		else if (ep == &portInput)
		{
			const String& text = e.getText();
			if (text.isEmpty())
			{
				osc.port = 1234;
				e.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
				e.setText(String(osc.port), dontSendNotification);
			}
			else
			{
				osc.port = CharacterFunctions::getIntValue<int, CharPointer_UTF8>(text.toUTF8());
			}
			osc.sender.disconnect();
			osc.sender.connect(osc.address, std::max(0, osc.port));
		}
		else if (ep == &intervalInput)
		{
			const String& text = e.getText();
			if (text.isEmpty())
			{
				osc.interval = 100;
				e.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
				e.setText(String(osc.interval), dontSendNotification);
			}
			else
			{
				osc.interval = CharacterFunctions::getIntValue<int, CharPointer_UTF8>(e.getText().toUTF8());
			}
			audioProcessor.startTimer(std::max(osc.interval, 1));
		}
	}

	void textEditorReturnKeyPressed(TextEditor& e) override
	{
		updateInputs(e);
	}

	void textEditorEscapeKeyPressed(TextEditor& e) override
	{
		updateInputs(e);
	}

	void textEditorFocusLost(TextEditor& e) override
	{
		updateInputs(e);
	}

	void reset()
	{
		addressInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		addressInput.setText(String(osc.address), dontSendNotification);

		portInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		portInput.setText(String(osc.port), dontSendNotification);

		intervalInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
		intervalInput.setText(String(osc.interval), dontSendNotification);
	}

	void resized() override
	{
		FlexBox addressFb;
		addressFb.flexDirection = FlexBox::Direction::row;
		addressFb.items.add(FlexItem(addressLabel).withFlex(1));
		addressFb.items.add(FlexItem(addressInput).withFlex(1).withMargin(8));

		FlexBox portFb;
		portFb.flexDirection = FlexBox::Direction::row;
		portFb.items.add(FlexItem(portLabel).withFlex(1));
		portFb.items.add(FlexItem(portInput).withFlex(1).withMargin(8));

		FlexBox intervalFb;
		intervalFb.flexDirection = FlexBox::Direction::row;
		intervalFb.items.add(FlexItem(intervalLabel).withFlex(1));
		intervalFb.items.add(FlexItem(intervalInput).withFlex(1).withMargin(8));

		FlexBox mainFb;
		mainFb.flexDirection = FlexBox::Direction::column;
		mainFb.justifyContent = FlexBox::JustifyContent::spaceAround;
		mainFb.items.add(FlexItem(addressFb).withFlex(1).withMaxHeight(50.0f));
		mainFb.items.add(FlexItem(portFb).withFlex(1).withMaxHeight(50.0f));
		mainFb.items.add(FlexItem(intervalFb).withFlex(1).withMaxHeight(50.0f));

		mainFb.performLayout(getLocalBounds().toFloat());
	}

private:
	OscHandler& osc;
	OscvstAudioProcessor& audioProcessor;
	Label addressLabel, portLabel, intervalLabel;
	TextEditor addressInput, portInput, intervalInput;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsMenuComponent)
};
