/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "OscObject.h"

//==============================================================================
struct OscHandler
{
	OSCSender sender;
	String address = "127.0.0.1";
	int port = 1234;
	int interval = 100;
};

//==============================================================================
/**
*/
class OscvstAudioProcessor
	: public juce::AudioProcessor
	, public MidiKeyboardStateListener
	, public Timer
{
public:
	//==============================================================================
	OscvstAudioProcessor();
	~OscvstAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================
	MidiKeyboardState keyboardState;
	std::vector<std::atomic<float>*> knobs;
	AudioProcessorValueTreeState paramsState;
	MidiOscMap oscMap;
	OscHandler osc;
	std::function<void()> onStateLoadedCallback;

	OscObject* addOscObject(const int note, const String& path);
	bool removeOscObject(const int note);

	void timerCallback() override;

private:

	void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override {}; 

	//==============================================================================
	MidiBuffer midiBuffer;
	Random random;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscvstAudioProcessor)
};
