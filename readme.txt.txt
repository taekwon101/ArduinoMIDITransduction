Transducing to MIDI starter experimentation  

Currently takes guitar hero controller (Fender Stratocaster w/ whammy) and interprets whammy as MIDI CC from a Pro Micro board (w/ MidiUSB library, is USB MIDI-compliant out of the box)

To-do
- bug: whammy bouncing back to default from deep push will trigger a max value and stay there sometimes
- "debouncing"?
- map LH notes (1st position to strum trigger and up-neck to instant trigger)
- strum detection (possibly implement velocity from strum detection)
- program hammers/pulls as alternative note activation
- change scale with a body button
- light LED when note press detected

Notes

Considerations for arduino model:
	MIDI USB class compliant viable microcontrollers
		ATMEGA16U2 (+hack) = some Uno and some Mega
		ATMEGA32U4 (+MIDI USB library) = micro, pro micro, leonardo
	Number of devices for pins/multiplexing/bitshifting

Environment/housing
	Breadboard vs plate vs jumpers and i/o
	Container = cardboard vs Al vs 3DP or combination

Components
	Basic electronics
		Passive components: resistors, capacitors, inductors
		Tools: wires, breadboard, solder, tape, glue
	Transduction devices
		Tactile buttons (SPST switch, etc.)
		Flick switches
		Piezosensors
		Potentiometers (slide/knob, lin/log)
		Slide capacitive touch
	Other
		LEDs
		Relays/switches


