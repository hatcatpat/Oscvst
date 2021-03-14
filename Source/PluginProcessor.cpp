/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscvstAudioProcessor::OscvstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), false)
#endif
	)
#endif
	, random(Time::currentTimeMillis())
	,
	paramsState(*this, nullptr, Identifier("Oscvst"),
		{
			std::make_unique<AudioParameterFloat>("knob1", "Knob 1", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob2", "Knob 2", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob3", "Knob 3", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob4", "Knob 4", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob5", "Knob 5", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob6", "Knob 6", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob7", "Knob 7", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob8", "Knob 8", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob9", "Knob 9", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob10", "Knob 10", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob11", "Knob 11", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob12", "Knob 12", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob13", "Knob 13", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob14", "Knob 14", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob15", "Knob 15", 0.f, 1.f, 0.f),
			std::make_unique<AudioParameterFloat>("knob16", "Knob 16", 0.f, 1.f, 0.f),
		}
		)
{

	// PARAMS
	for (int i = 0; i < 16; ++i)
	{
		knobs.emplace_back(paramsState.getRawParameterValue("knob" + String(i + 1)));
	}

	// KEYBOARD
	keyboardState.addListener(this);

	//// OSC
	osc.sender.connect(osc.address, osc.port);
	// TIMER
	startTimer(osc.interval);
}

OscvstAudioProcessor::~OscvstAudioProcessor()
{
}

//==============================================================================
const juce::String OscvstAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool OscvstAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool OscvstAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool OscvstAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double OscvstAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int OscvstAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int OscvstAudioProcessor::getCurrentProgram()
{
	return 0;
}

void OscvstAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String OscvstAudioProcessor::getProgramName(int index)
{
	return {};
}

void OscvstAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void OscvstAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void OscvstAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OscvstAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void OscvstAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	keyboardState.processNextMidiBuffer(midiMessages, buffer.getNumSamples(), 0, true);
}

//==============================================================================
bool OscvstAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OscvstAudioProcessor::createEditor()
{
	return new OscvstAudioProcessorEditor(*this);
}

//==============================================================================
void OscvstAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// OSC SETTINGS
	{
		if (paramsState.state.getChildWithName("oscSettings").isValid())
		{
			paramsState.state.removeChild(paramsState.state.getChildWithName("oscSettings"), nullptr);
		}
		ValueTree oscSettingsTree("oscSettings");
		oscSettingsTree.setProperty("address", osc.address, nullptr);
		oscSettingsTree.setProperty("port", osc.port, nullptr);
		oscSettingsTree.setProperty("interval", osc.interval, nullptr);
		paramsState.state.addChild(oscSettingsTree, 0, nullptr);
	}

	// OSC MAP
	{
		if (paramsState.state.getChildWithName("oscMap").isValid())
		{
			paramsState.state.removeChild(paramsState.state.getChildWithName("oscMap"), nullptr);
		}
		if (!oscMap.empty())
		{
			ValueTree oscMapTree("oscMap");
			for (const auto& p : oscMap)
			{
				oscMapTree.addChild(p.second.toTree(), 0, nullptr);
			}

			paramsState.state.addChild(oscMapTree, 0, nullptr);
		}
	}

	auto state = paramsState.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void OscvstAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(paramsState.state.getType()))
		{
			paramsState.replaceState(ValueTree::fromXml(*xmlState));

			// OSC SETTINGS
			{
				const auto& oscSettingsTree = paramsState.state.getChildWithName("oscSettings");
				if (oscSettingsTree.isValid())
				{
					osc.address = oscSettingsTree["address"];
					osc.port = oscSettingsTree["port"];
					osc.interval = oscSettingsTree["interval"];
					osc.sender.disconnect();
					osc.sender.connect(osc.address, osc.port);
					startTimer(std::max(osc.interval, 1));
				}
			}

			// OSC MAP
			{
				oscMap.clear();
				const auto& oscMapTree = paramsState.state.getChildWithName("oscMap");
				if (oscMapTree.isValid())
				{
					for (int i = 0; i < oscMapTree.getNumChildren(); ++i)
					{
						// OSC OBJECT
						const auto& oscObjectTree = oscMapTree.getChild(i);
						if (oscObjectTree.isValid())
						{
							const String& path = oscObjectTree["path"];
							const int note = oscObjectTree["note"];
							OscObject* oscObject = addOscObject(note, path);
							if (oscObject)
							{
								oscObject->useNote = oscObjectTree["useNote"];
								oscObject->useVel = oscObjectTree["useVel"];

								// RANDOM RANGE
								{
									const auto& rangeTree = oscObjectTree.getChildWithName("randomRange");
									if (rangeTree.isValid())
									{
										oscObject->randomRange.inUse = rangeTree.getProperty("inUse");
										oscObject->randomRange.isInt = rangeTree.getProperty("isInt");
										oscObject->randomRange.range.setStart(rangeTree.getProperty("lo"));
										oscObject->randomRange.range.setEnd(rangeTree.getProperty("hi"));
									}
								}

								// KNOB RANGES
								for (int i = 0; i < 16; ++i)
								{
									const auto& rangeTree = oscObjectTree.getChildWithName(String("knob") + String(i + 1));
									if (rangeTree.isValid())
									{
										oscObject->knobRanges[i].inUse = rangeTree.getProperty("inUse");
										oscObject->knobRanges[i].isInt = rangeTree.getProperty("isInt");
										oscObject->knobRanges[i].range.setStart(rangeTree.getProperty("lo"));
										oscObject->knobRanges[i].range.setEnd(rangeTree.getProperty("hi"));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (onStateLoadedCallback) onStateLoadedCallback();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new OscvstAudioProcessor();
}

//==============================================================================
void OscvstAudioProcessor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	midiBuffer.addEvent(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity), 0);
}

//==============================================================================
OscObject* OscvstAudioProcessor::addOscObject(const int note, const String& path)
{
	if (oscMap.count(note) == 0)
	{
		oscMap.emplace(note, OscObject(&random, note, path));
	}
	else
	{
		oscMap[note] = OscObject(&random, note, path);
	}

	return &oscMap[note];
}

//==============================================================================
bool OscvstAudioProcessor::removeOscObject(const int note)
{
	if (oscMap.count(note) != 0)
	{
		oscMap.erase(note);
		return true;
	}
	else
	{
		return false;
	}
}

//==============================================================================
void OscvstAudioProcessor::timerCallback()
{
	if (midiBuffer.getNumEvents() == 0) return;

	std::vector<float> knobValues;
	for (const auto v : knobs)
	{
		knobValues.push_back(*v);
	}

	for (const auto& it : midiBuffer)
	{
		const MidiMessage& msg = it.getMessage();
		const int note = msg.getNoteNumber();
		const float vel = msg.getVelocity() / 127.0f;

		if (oscMap.count(note) != 0)
		{
			oscMap[note].send(osc.sender, vel, knobValues);
		}
	}

	midiBuffer.clear();
}