/*
  ==============================================================================

	OscObjectComponent.h
	Created: 10 Mar 2021 7:37:34pm
	Author:  pat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OscObject.h"
#include "PatLookAndFeel.h"

//==============================================================================
class RightClickMenu
	: public Component
	, public TextEditor::Listener
{
public:

	RightClickMenu(OscObject* _oscObject, PatToggleButton* button)
		: oscObject(_oscObject)
	{

		switch (button->type)
		{
		case PatToggleButton::Types::Random:
			oscRange = &oscObject->randomRange;
			break;
		case PatToggleButton::Types::Knob:
			oscRange = &oscObject->knobRanges[button->knobId];
			break;
		}
		if (!oscRange) return;

		loLabel.setText("lo", dontSendNotification);
		loLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(loLabel);

		hiLabel.setText("hi", dontSendNotification);
		hiLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(hiLabel);

		intToggle.setButtonText("int?");
		intToggle.setToggleState(oscRange->isInt, dontSendNotification);
		intToggle.onStateChange = [this]()
		{
			oscRange->isInt = this->intToggle.getToggleState();
		};
		addAndMakeVisible(intToggle);

		// INPUTS
		{
			const String& inputRestrictions = "0123456789.-";

			loInput.setInputRestrictions(0, inputRestrictions);
			loInput.setJustification(Justification::verticallyCentred);
			loInput.addListener(this);
			loInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
			loInput.setText(String(oscRange->range.getStart()), dontSendNotification);
			addAndMakeVisible(loInput);

			hiInput.setInputRestrictions(0, inputRestrictions);
			hiInput.setJustification(Justification::verticallyCentred);
			hiInput.addListener(this);
			hiInput.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
			hiInput.setText(String(oscRange->range.getEnd()), dontSendNotification);
			addAndMakeVisible(hiInput);
		}
	}

	void updateInputs(TextEditor& e)
	{
		String& text = e.getText();
		float val;
		const bool isLo = &e == &loInput;

		if (text.isEmpty())
		{
			val = isLo ? 0.0f : 1.0f;
			e.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
			e.setText(String(val), dontSendNotification);
		}
		else
		{
			val = CharacterFunctions::readDoubleValue(e.getText().toUTF8());
		}

		if (isLo)
			oscRange->range.setStart(val);
		else
			oscRange->range.setEnd(val);
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

	void resized() override
	{
		FlexBox loFb;
		loFb.flexDirection = FlexBox::Direction::row;
		loFb.items.add(FlexItem(loLabel).withFlex(1));
		loFb.items.add(FlexItem(loInput).withFlex(3));

		FlexBox hiFb;
		hiFb.flexDirection = FlexBox::Direction::row;
		hiFb.items.add(FlexItem(hiLabel).withFlex(1));
		hiFb.items.add(FlexItem(hiInput).withFlex(3));

		FlexBox mainFb;
		mainFb.flexDirection = FlexBox::Direction::column;
		mainFb.items.add(FlexItem(loFb).withFlex(1));
		mainFb.items.add(FlexItem(hiFb).withFlex(1));
		mainFb.items.add(FlexItem(intToggle).withFlex(1));
		mainFb.performLayout(getLocalBounds().toFloat());
	}

	OscObject* oscObject;
	Label loLabel, hiLabel;
	TextEditor loInput, hiInput;
	ToggleButton intToggle;

	OscObject::OscObjectRange* oscRange;
};

//==============================================================================
class AddOrRemoveButton : public ShapeButton
{
public:

	AddOrRemoveButton(const String& name, bool isAdd) : ShapeButton(name, PatColours::pink, PatColours::lightPink, PatColours::darkPink)
	{
		const float w = 100.0f;
		const float h = 100.0f;

		Path shape;

		shape.addLineSegment(Line<float>(w / 2.0f, 0.0f, w / 2.0f, h), 8.0f); // UP -> DOWN
		shape.addLineSegment(Line<float>(0.0f, h / 2.0f, w, h / 2.0f), 8.0f); // LEFT -> RIGHT

		if (!isAdd) shape.applyTransform(AffineTransform::rotation(MathConstants<double>::halfPi / 2.0f));

		setShape(shape, false, true, false);
	}

};

//==============================================================================
/*
*/
class OscObjectComponent
	: public Component
	, public Button::Listener
	, public MouseListener
	, public TextEditor::Listener
{
public:

	OscObjectComponent() = default;

	OscObjectComponent(MidiOscMap& _oscMap)
		: oscMap(_oscMap)
		, addButton("addOscObject", true)
		, removeButton("removeOscObject", false)
	{
		setInterceptsMouseClicks(true, true);

		pathTextEditor.setMultiLine(false, true);
		pathTextEditor.setJustification(Justification::verticallyCentred);
		pathTextEditor.setInputRestrictions(0, String("/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
		pathTextEditor.addListener(this);
		addAndMakeVisible(pathTextEditor);

		pathLabel.setText("path: ", dontSendNotification);
		pathLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(pathLabel);

		removeButton.addListener(this);
		addAndMakeVisible(removeButton);

		addButton.addListener(this);
		addAndMakeVisible(addButton);

		noteToggle.setButtonText("note?");
		noteToggle.addListener(this);
		addAndMakeVisible(noteToggle);

		velToggle.setButtonText("vel?");
		velToggle.addListener(this);
		addAndMakeVisible(velToggle);

		randomToggle.setButtonText("random?");
		randomToggle.addListener(this);
		randomToggle.onRightClick = buttonRightClickedLambda;
		addAndMakeVisible(randomToggle);

		for (int i = 0; i < 16; ++i)
		{
			toggles.add(new PatToggleButton(String(i + 1), PatToggleButton::Types::Knob, i));
			toggles.getLast()->addListener(this);
			toggles.getLast()->onRightClick = buttonRightClickedLambda;
		}
		for (const auto& toggles : toggles)
			addAndMakeVisible(toggles);

		updateMode();
	}

	OscObjectComponent(MidiOscMap& _oscMap, OscObject* _oscObject) : OscObjectComponent(_oscMap)
	{
		oscObject = _oscObject;
		updateMode();
	}

	OscObjectComponent(MidiOscMap& _oscMap, OscObject* _oscObject
		, std::function<OscObject* ()> _addOscObjectCallback
		, std::function<void()> _removeOscObjectCallback
	)
		: OscObjectComponent(_oscMap, _oscObject)
	{
		addOscObjectCallback = _addOscObjectCallback;
		removeOscObjectCallback = _removeOscObjectCallback;
	}

	void buttonRightClicked(PatToggleButton* b)
	{
		RightClickMenu cm(oscObject, b);
		PopupMenu m;
		m.setLookAndFeel(&getLookAndFeel());
		m.addItem(1, "close");
		m.addCustomItem(2, cm, 100, 100, false);

		const int result = m.show();
	}

	void updateMode()
	{
		if (oscObject)
		{
			pathTextEditor.setText(oscObject->path, dontSendNotification);
			pathTextEditor.setJustification(Justification::centred);
			pathTextEditor.setVisible(true);

			pathLabel.setVisible(true);
			removeButton.setVisible(true);

			noteToggle.setToggleState(oscObject->useNote, dontSendNotification);
			noteToggle.setVisible(true);

			velToggle.setToggleState(oscObject->useVel, dontSendNotification);
			velToggle.setVisible(true);

			randomToggle.setToggleState(oscObject->randomRange.inUse, dontSendNotification);
			randomToggle.setVisible(true);

			{
				uint8 num = 0;
				for (auto& toggle : toggles)
				{
					toggle->setToggleState(oscObject->isUsingKnob(num), dontSendNotification);
					toggle->setVisible(true);
					++num;
				}
			}

			addButton.setVisible(false);
		}
		else
		{
			pathTextEditor.setVisible(false);
			pathLabel.setVisible(false);
			removeButton.setVisible(false);
			noteToggle.setVisible(false);
			velToggle.setVisible(false);
			randomToggle.setVisible(false);
			for (auto& toggle : toggles)
				toggle->setVisible(false);

			addButton.setVisible(true);
		}
	}

	void buttonClicked(Button* b) override
	{
		if (b == &removeButton)
		{
			if (!removeOscObjectCallback) return;
			removeOscObjectCallback();
			oscObject = nullptr;
			updateMode();
			return;
		}
		else if (b == &addButton)
		{
			if (!addOscObjectCallback) return;
			oscObject = addOscObjectCallback();
			updateMode();
			return;
		}
		else if (b == &noteToggle)
		{
			if (oscObject)
			{
				oscObject->useNote = noteToggle.getToggleState();
			}
			return;
		}
		else if (b == &velToggle)
		{
			if (oscObject)
			{
				oscObject->useVel = velToggle.getToggleState();
			}
			return;
		}
		else if (b == &randomToggle)
		{
			if (oscObject)
			{
				oscObject->randomRange.inUse = randomToggle.getToggleState();
			}
			return;
		}
		else
		{
			uint8 num = 0;
			for (const auto& toggle : toggles)
			{
				if (b == toggle)
				{
					if (oscObject)
					{
						if (b->getToggleState())
						{
							oscObject->addKnobToUse(num);
						}
						else
						{
							oscObject->removeKnobToUse(num);
						}
					}
					break;
				}
				++num;
			}
		}
	}

	void resized() override
	{
		const int halfX = (int)floor((float)getWidth() / 2.0f);
		const int halfY = (int)floor((float)getHeight() / 2.0f);

		// path
		{
			const int margin = 30;
			pathLabel.setBounds(margin, margin, halfX - margin * 2, halfY - margin * 2);
			pathTextEditor.setBounds(halfX + margin, margin, halfX - margin * 2, halfY - margin * 2);
		}

		// toggles
		{
			const int thirdX = (int)floor((float)getWidth() / 3.0f);
			const int quarterY = (int)floor((float)getHeight() / 4.0f);
			const int sixthY = (int)floor((float)getHeight() / 6.0f);

			noteToggle.setBounds(0, halfY, thirdX, sixthY);
			velToggle.setBounds(0, halfY + sixthY, thirdX, sixthY);
			randomToggle.setBounds(0, halfY + 2 * sixthY, thirdX, sixthY);

			const int unitX = (int)floor((float)thirdX * 2.0f / 8.0f);
			for (int i = 0; i < 16; ++i)
			{
				toggles[i]->setBounds(
					thirdX + unitX * (i % 8)
					, halfY + (int)floor(i / 8.0f) * quarterY
					, unitX
					, quarterY
				);
			}
		}

		// add/remove
		removeButton.setBounds(8, 8, 16, 16);

		addButton.setSize(32, 32);
		addButton.setCentrePosition(halfX, halfY);
	}

	void setMidi(const int note)
	{
		if (oscMap.count(note) == 1)
		{
			oscObject = &oscMap[note];
		}
		else
		{
			oscObject = nullptr;
		}

		updateMode();
	}

	// TEXT EDITOR LISTENER
	void textEditorReturnKeyPressed(TextEditor& e) override
	{
		updateText(e);
	}

	void textEditorEscapeKeyPressed(TextEditor& e) override
	{
		updateText(e);
	}

	void textEditorFocusLost(TextEditor& e) override
	{
		updateText(e);
	}

	void updateText(TextEditor& e)
	{
		if (oscObject)
		{
			String& text = e.getText();
			if (text.isEmpty())
			{
				text = "/oscvst";
				e.setColour(TextEditor::textColourId, PatColours::pink); // weird workaround because setText uses wrong colour?
				e.setText(text, dontSendNotification);
			}
			oscObject->setPath(text);
		}
	}

	// Variables
public:

	std::function<OscObject* ()> addOscObjectCallback;
	std::function<void()> removeOscObjectCallback;

private:

	MidiOscMap& oscMap;
	OscObject* oscObject{ nullptr };
	Label pathLabel;
	TextEditor pathTextEditor;
	AddOrRemoveButton removeButton;
	AddOrRemoveButton addButton;

	PatToggleButton noteToggle;
	PatToggleButton velToggle;
	PatToggleButton randomToggle;
	OwnedArray<PatToggleButton> toggles;

	std::function<void(PatToggleButton*)> buttonRightClickedLambda = [this](PatToggleButton* b) { this->buttonRightClicked(b); };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscObjectComponent);
};