# MIDI-pedalboard-controller/switcher

![Controller image](https://raw.githubusercontent.com/jpcarrascal/MIDI-pedalboard-controller/main/MIDI-pedalboard-controller.png)

Pedalboard controller, for Zoia and other MIDI-controlled devices. Features 3 footswitches and 4 knobs. Features:

* Dual mode:
  - Control mode (default). In this mode, footswitches send MIDI CC mesages. Currently they are fixed to CC numbers 0, 1, 2.
  - Program mode, activated by pushing the center footswitch for 1.5 seconds. In this mode, the left and right footswitches send MIDI PC (Program Change) messages. Pushing the center footswitch again reverts back to CC mode.
* "Pick-up" mode: if CC values corresponding to the 4 knobs (CC 3, 4, 5, 6) are received via the onboard MIDI input (channel 15), knobs will only send control data once their position match incoming MIDI messages. This avoids sudden parameter value jumps when controlling external devices.
* External pedal loop, controlled either via MIDI input or internally assigned to the middle footswitch.
* MIDI Class-compliant. Works as USB plug-and-play controller for computer software, no driver required.

The MIDI output port is a 1/8" TRS jack, conforming to the MIDI standard described in [the MIDI.org website](https://www.midi.org/specifications/midi-transports-specifications/specification-for-use-of-trs-connectors-with-midi-devices-2) (sometimes referred as "TRS-A").


### Dependencies
- r89m Buttons: https://github.com/r89m/Button
- r89m PushButton: https://github.com/r89m/PushButton
- Bounce2: https://github.com/thomasfredericks/Bounce2
- 47 effects Arduino MIDI library: https://github.com/FortySevenEffects/arduino_midi_library

### Potential improvements
- Add port for external expression pedal.
