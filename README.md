# MIDI-pedalboard-controller/switcher

![Controller image](https://raw.githubusercontent.com/jpcarrascal/MIDI-pedalboard-controller/main/MIDI-pedalboard-controller.png)

Arduino Micro-based MIDI pedal controller. I made this device for controlling my [Zoia](https://empresseffects.com/products/zoia) pedal, but it can be used for any MIDI-controlled device or software. Controls and ports include 3 footswitches, 4 knobs, MIDI in/out/thru(*), USB MIDI, and an external pedal loop/switch. Features:

* Dual footswitch mode:
  - Control mode (default). In this mode, footswitches send MIDI CC mesages. Currently they are fixed to CC numbers 0, 1, 2.
  - Program mode, activated by pushing the center footswitch for 1.5 seconds. In this mode, the left and right footswitches send MIDI PC (Program Change) messages. Pushing the center footswitch again reverts back to CC mode.
* Knob "Pick-up" mode: if a CC value corresponding to any of the 4 knobs (CC 3, 4, 5, 6) is received via the onboard MIDI input (channel 15), the knob will pause sending control data _until its position matches incoming MIDI messages_. This avoids sudden parameter value jumps when controlling external devices.
* The external pedal loop can be controlled either via MIDI input (default) or internally with the middle footswitch. Switching between MIDI and internal is done by simultaneously pressing the left and right footswitches.
* MIDI Class-compliant. Works as USB plug-and-play controller for computer software, no driver required.

#### New features:

* MIDI-USB passthrough: it passes PC and CC messages from USB-MIDI through to USB ports. Acts (almost) like a USB MIDI interface, so it is possible to control pedals from a computer with no additional hardware.

(*) _The MIDI ports are 1/8" TRS jack, conforming to the MIDI standard described in [the MIDI.org website](https://www.midi.org/specifications/midi-transports-specifications/specification-for-use-of-trs-connectors-with-midi-devices-2) (sometimes referred as "TRS-A")._

### Dependencies
- r89m Buttons: https://github.com/r89m/Button
- r89m PushButton: https://github.com/r89m/PushButton
- Bounce2: https://github.com/thomasfredericks/Bounce2
- 47 effects Arduino MIDI library: https://github.com/FortySevenEffects/arduino_midi_library

### Possible improvements
- Add port for external expression pedal.
