# MIDI-pedalboard-controller

![Controller image](https://raw.githubusercontent.com/jpcarrascal/MIDI-pedalboard-controller/main/MIDI-pedalboard-controller.png)

MIDI pedalboard controller, for Zoia and others. Features three footswitches and 2 potentiometers. It has two modes:
- MIDI CC mode (default).
- MIDI PC mode, activated by pushing the center footswitch for 1.5 seconds. Pushing the center footswitch again reverts back to CC mode.

### Dependencies
- r89m Buttons: https://github.com/r89m/Button
- rm89 PushButton: https://github.com/r89m/PushButton
- Bounce2: https://github.com/thomasfredericks/Bounce2
- 47 effects Arduino MIDI library: https://github.com/FortySevenEffects/arduino_midi_library

### TODO:
- Add other controls, including an external expression pedal and more potentiometers.
- Expose USB port for easy reprogramming and USB MIDI
