/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscvstAudioProcessorEditor::OscvstAudioProcessorEditor(OscvstAudioProcessor& p)
	: AudioProcessorEditor(&p)
	, audioProcessor(p)
	, oscMap(p.oscMap)
	, keyboardComponent(audioProcessor.keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard, oscMap)
	, oscObjectComponent(oscMap)
	, settingsMenuComponent(p)
{
	// GUI
	setSize(400, 300);

	setLookAndFeel(&patLookAndFeel);

	// KEYBOARD
	addAndMakeVisible(keyboardComponent);
	keyboardComponent.mouseDownOnKeyCallback = [this](const int note)
	{
		this->oscObjectComponent.setMidi(note);
	};

	// OSC OBJECT COMPONENT
	oscObjectComponent.addOscObjectCallback = [this]()
	{
		return this->addOscObject(keyboardComponent.lastKey, "/oscvst");
	};
	oscObjectComponent.removeOscObjectCallback = [this]()
	{
		this->removeOscObject(keyboardComponent.lastKey);
	};
	addAndMakeVisible(oscObjectComponent);

	// KNOB MENU COMPONENT
	for (int i = 0; i < audioProcessor.knobs.size(); ++i)
	{
		knobs.add(new SliderParameterAttachment(*audioProcessor.paramsState.getParameter(String("knob") + String(i + 1)), *knobMenuComponent.knobs[i]));
	}
	addAndMakeVisible(knobMenuComponent);

	// OSC
	oscObjectComponent.setMidi(60);
	keyboardComponent.lastKey = 60;

	// MENU COMPONENT
	addAndMakeVisible(settingsMenuComponent);

	// MENU
	addAndMakeVisible(menuBar);
	menuBar.setSceneCallback = [this](const Scenes scene)
	{
		this->setScene(scene);
	};
	setScene(Scenes::OscEdit);
	menuBar.setScene(Scenes::OscEdit);

	// STATE
	audioProcessor.onStateLoadedCallback = [this]()
	{
		this->keyboardComponent.repaint();
		this->oscObjectComponent.setMidi(keyboardComponent.lastKey);
	};
}

OscvstAudioProcessorEditor::~OscvstAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

//==============================================================================
void OscvstAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void OscvstAudioProcessorEditor::resized()
{
	int y = 0;
	menuBar.setBounds(0, 0, getWidth(), 25);
	y += 25;
	knobMenuComponent.setBounds(0, y, getWidth(), getHeight() - y);
	settingsMenuComponent.setBounds(0, y, getWidth(), getHeight() - y);

	keyboardComponent.setBounds(0, y, getWidth(), 75);
	y += 75;
	oscObjectComponent.setBounds(0, y, getWidth(), getHeight() - y);
}

//==============================================================================
OscObject* OscvstAudioProcessorEditor::addOscObject(const int note, const String& path)
{
	if (auto oscObject = audioProcessor.addOscObject( note, path))
	{
		keyboardComponent.repaint();
		return oscObject;
	}

	return nullptr;
}

//==============================================================================
void OscvstAudioProcessorEditor::removeOscObject(const int note)
{
	if (audioProcessor.removeOscObject(note))
	{
		keyboardComponent.repaint();
	}
}

//==============================================================================
void OscvstAudioProcessorEditor::setScene(const Scenes _scene)
{
	scene = _scene;

	switch (scene)
	{

	case Scenes::OscEdit:
	{
		oscObjectComponent.setVisible(true);
		keyboardComponent.setVisible(true);
		knobMenuComponent.setVisible(false);
		settingsMenuComponent.setVisible(false);
	}
	break;

	case Scenes::KnobMenu:
	{
		oscObjectComponent.setVisible(false);
		keyboardComponent.setVisible(false);
		knobMenuComponent.setVisible(true);
		settingsMenuComponent.setVisible(false);
	}
	break;

	case Scenes::SettingsMenu:
	{
		oscObjectComponent.setVisible(false);
		keyboardComponent.setVisible(false);
		knobMenuComponent.setVisible(false);
		settingsMenuComponent.setVisible(true);
	}
	break;

	}
}
