# Oscvst
A VST3 plugin for sending OSC messages
Made with [JUCE](https://github.com/juce-framework/JUCE) v6.0.7

## Usage

### Osc Edit
![Osc Edit Menu](/ReadmeImages/main.png)

Select a MIDI note to assign using the on screen keyboard. Then, press the + button.

![Osc Edit Screen](/ReadmeImages/oscEdit.png)

- The path box lets you edit the path to send the OSC message to, this will be the first argument of the OSC message.
- The "note?" and "vel?" buttons decide whether or not to send the MIDI note number (0-127) and MIDI velocity (0.0-1.0) as arguments for the OSC message.
- The "random?" buttons decide whether or not to send a random number along with the OSC message.
- The number buttons decide whether or not to send the value of the corresponding VST parameter knob along with the OSC message.
This OSC message will be sent every time Oscvst recieves a corresponding MIDI note message, or if the user right clicks on this key using the on screen keyboard.

![Right Click Menu](/ReadmeImages/rightClickMenu.png)

The "random?" button and the number buttons allow you to open the right click menu which can scale the output of each of these values. The "int?" button allows you to send the value as an integer rather than a float.

### Knobs
![Knob Menu](/ReadmeImages/knobs.png)

This screen allows you to modify and monitor the value of the 16 VST parameter knobs. These knobs can be modified from within your DAW, their names are of the form "Knob i".

### Settings

- Address: sets the IP address to send the message to, by default it is localhost = 127.0.0.1 (i.e, same computer)
- Port: the port to send the message to, can be changed to any positive number provided that port is not in use.
- Interval: Oscvst stores the incoming MIDI messages into a buffer. The interval (in ms) decides how often it should send those messages and clear the buffer. This VST was designed for sending control messages (i.e, to control visuals), not audio messages (i.e, triggering a synth).
