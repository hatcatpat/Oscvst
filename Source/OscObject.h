/*
  ==============================================================================

	OscObject.h
	Created: 10 Mar 2021 8:04:53pm
	Author:  pat

  ==============================================================================
*/

#pragma once

#include <tuple>

#include "JuceHeader.h"

//==============================================================================
struct OscObject
{
	struct OscObjectRange
	{
		bool inUse{ false };
		bool isInt{ false };
		Range<float> range{ 0.0f,1.0f };

		float getValue(const float value) const
		{
			return jmap(value, range.getStart(), range.getEnd());
		}

		ValueTree toTree(const String& name) const
		{
			ValueTree tree(name);

			tree.setProperty("inUse", inUse, nullptr);
			tree.setProperty("isInt", isInt, nullptr);
			tree.setProperty("lo", range.getStart(), nullptr);
			tree.setProperty("hi", range.getEnd(), nullptr);

			return tree;
		}
	};
	//==============================================================================

	OscObject() = default;

	OscObject(Random* _random, const int _note, const String _path)
		: random(_random)
		, note(_note)
	{
		setPath(_path);

		for (int i = 0; i < 16; ++i)
		{
			knobRanges.emplace_back();
		}
	}

	void send(OSCSender& sender, const float vel, const std::vector<float>& knobValues)
	{
		OSCMessage msg(path);

		if (useNote) msg.addInt32(note);
		if (useVel) msg.addFloat32(vel);

		if (random && randomRange.inUse)
		{
			if (randomRange.isInt)
				msg.addInt32((int)floor(randomRange.getValue(random->nextFloat())) + 1);
			else
				msg.addFloat32(randomRange.getValue(random->nextFloat()));
		}

		for (int i = 0; i < knobRanges.size(); ++i)
		{
			const auto& knob = knobRanges[i];
			if (knob.inUse)
			{
				if (knob.isInt)
					msg.addInt32((int)floor(knob.getValue(knobValues[i])) + 1);
				else
					msg.addFloat32(knob.getValue(knobValues[i]));
			}
		}

		sender.send(msg);
	}

	void addKnobToUse(const uint8 num)
	{
		if (0 <= num && num < 16)
		{
			knobRanges[num].inUse = true;
		}
	}

	void removeKnobToUse(const uint8 num)
	{
		if (0 <= num && num < 16)
		{
			knobRanges[num].inUse = false;
		}
	}

	bool isUsingKnob(const uint8 num)
	{
		if (0 <= num && num < 16)
		{
			return knobRanges[num].inUse;
		}
		else
		{
			return false;
		}
	}

	void setPath(const String& _path)
	{
		path = _path;

		if (!path.startsWith("/"))
		{
			path = String("/") + path;
		}
	}

	ValueTree toTree() const
	{
		ValueTree tree("oscObject");

		tree.setProperty("path", path, nullptr);
		tree.setProperty("note", note, nullptr);
		tree.setProperty("useNote", useNote, nullptr);
		tree.setProperty("useVel", useVel, nullptr);
		tree.addChild(randomRange.toTree("randomRange"), 0, nullptr);
		for (int i = 0; i < knobRanges.size(); ++i)
			tree.addChild(knobRanges[i].toTree(String("knob") + String(i+1)), 0, nullptr);
	
		return tree;
	}

	int note{ 60 };
	String path;
	std::vector<OscObjectRange> knobRanges;
	bool useNote{ true };
	bool useVel{ true };
	OscObjectRange randomRange;

	Random* random{ nullptr };
};
//==============================================================================
using MidiOscMap = std::map<int, OscObject>; // note, OscObject