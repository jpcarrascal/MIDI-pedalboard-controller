#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <Bounce2.h>
#include <MIDI.h>
#include "MIDIUSB.h"

#define POT_COUNT 4 // We have 4 potentiometers/knobs

// Configurationflags:
const bool debug = false;
const bool usbMIDI = true; // Send MIDI via USB?
const bool srlMIDI = true; // Send/receive MIDI via MIDI ports?
bool pickUpMode = true;
bool loopInternal = false;
bool center_toggle = true;
int center_val = 0;

PushButton sw_left = PushButton(2, ENABLE_INTERNAL_PULLUP);
PushButton sw_center = PushButton(3, ENABLE_INTERNAL_PULLUP);
PushButton sw_right = PushButton(4, ENABLE_INTERNAL_PULLUP);
const int relay1 = 5;
const int relay2 = 6;

const int mainLED =  8;
const int relayLED = 7;
const long blinkInterval = 300; // blink interval
const int PCchannel = 13;
const int CCchannel = 1;
const int INchannel = 16;
int currentProgram = 0;
int mainLedState = HIGH;
bool loopState = false;
unsigned long previousMillis = 0;
String mode = "CC"; // "CC" = Control Change, PC" = Program Change
const int cc_left   = 0;
const int cc_right  = 1;
const int cc_center = 2;
const int cc_pot[POT_COUNT] = {3, 4, 5, 6};
const int cc_relay = 7;
const int pot[POT_COUNT] = {A0, A1, A2, A3};
int potval[POT_COUNT];
int potvalIN[POT_COUNT];
bool potPosCorrect[POT_COUNT] = {true, true, true, true};

MIDI_CREATE_DEFAULT_INSTANCE();
void setup() {
  MIDI.begin(INchannel);
  MIDI.turnThruOff();
  // MIDI baud rate
  if(debug)
    Serial.begin(9600);
  else
    Serial.begin(31250);
  // For brighter LEDs, uncomment these two lines:
  //pinMode(mainLED, OUTPUT);
  //pinMode(mainLED, OUTPUT);
  digitalWrite(mainLED, HIGH);
  for(int i=0; i<4; i++) {
    potval[i] = analogRead(pot[i]);
  }

  // Footswitch press and release callbacks
  sw_left.onPress(onButtonPressed);
  sw_center.onPress(onButtonPressed);
  sw_right.onPress(onButtonPressed);

  sw_left.onRelease(onButtonReleased);
  sw_center.onRelease(onButtonReleased);
  sw_right.onRelease(onButtonReleased);
  
  // If center footswitch is held for 1.5 seconds, switch to PC mode
  sw_center.onHold(1500, setPCmode);
  
  // relay inputs:
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  switchLoop(false);
}

void loop() {
  if (MIDI.read()) {
    if (MIDI.getType() == midi::ControlChange) {
      // If a CC that corresponds to one of the knobs is received,
      // switch to "pick-up" mode by blocking sending MIDI CC 
      // until the knob position and the controller value match.
      int ccNumber = MIDI.getData1();
      int index = isPotCC(ccNumber);
      if( index >= 0 && pickUpMode) { // it is a knob CC
        potvalIN[index] = MIDI.getData2();
        if(abs( potvalIN[index] - potval[index] ) < 2)
          potPosCorrect[index] = true;
        else
          potPosCorrect[index] = false;
      } else {
        if(ccNumber == cc_relay) {
          loopState = MIDI.getData2() > 63;
          switchLoop(loopState);
        }
      }
    }
  }
  
  midiEventPacket_t rx;
  rx = MidiUSB.read();
  if (rx.header != 0) {
    if(rx.header == 0xB) {
      MIDI.sendControlChange(rx.byte2, rx.byte3, CCchannel);
    } else if(rx.header == 0xC) {
      MIDI.sendProgramChange(rx.byte2, PCchannel);
    }
    // Would be great to make this work instead of the two calls above
    // so not only CC and PC messages are passed through:
    // MIDI.send( rx.header, rx.byte2, rx.byte3, (rx.byte1 >> 4) & 0x0F);
    // Ref:
    // - Arduino MIUSB input example
    // - https://arduino.stackexchange.com/questions/41684/midiusb-why-is-the-command-put-twice
    // - https://fortyseveneffects.github.io/arduino_midi_library/a00032.html#ga58454de7d3ee8ee824f955c805151ad2
  }
  
  sw_left.update();
  sw_right.update();
  sw_center.update();
  unsigned long currentMillis = millis();
  if(mode == "PC") {
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      mainLedState = (!mainLedState);
      digitalWrite(mainLED, mainLedState);
    } 
  }

  int potvalNew[4];
  for(int i=0; i<4; i++) {
    potvalNew[i] = analogRead(pot[i]);
  }
  for(int i=0; i<4; i++) {
    if( abs(potval[i] - potvalNew[i]) > 7 && mode == "CC") {
      int outval = mapAndClamp(potval[i], i);
      potval[i] = potvalNew[i];
      if(potPosCorrect[i]) {
        ccSend(cc_pot[i], outval, CCchannel);
      } else {
        // Check if knob position matches value received via MIDI.
        // If so, allow sending MIDI again (i.e. pick-up).
        if( abs( outval - potvalIN[i] ) < 2 ) {
          potPosCorrect[i] = true;
        }
      }
    }
  }
}

void configurePushButton(Bounce& bouncedButton){
  bouncedButton.interval(10); //10 is default
}

void onButtonPressed(Button& btn){
  if(btn.is(sw_left)) {
    if(mode == "PC") {
      if(currentProgram > 0)
        currentProgram--;
      else
        currentProgram = 63;
      pcSend(currentProgram, PCchannel);
    }
    else
      ccSend(cc_left, 127, CCchannel);
  } else if (btn.is(sw_right)){
    if(mode == "PC") {
      if(currentProgram < 63)
        currentProgram++;
      else
        currentProgram = 0;
      pcSend(currentProgram, PCchannel);
    }
    else
      ccSend(cc_right, 127, CCchannel);
  } else if (btn.is(sw_center)) {
    if(mode == "PC")
      setCCmode();
    else {
      if(center_toggle) {
        if(center_val==0) center_val = 127;
        else center_val = 0;        
      } else {
        center_val = 127;
      }
      ccSend(cc_center, center_val, CCchannel);      
    }
    if(loopInternal) {
      loopState = !loopState;
      switchLoop(loopState);
    }
  }
}

void onButtonReleased(Button& btn, uint16_t duration){
  if(btn.is(sw_left)) {
    if(sw_right.isPressed())
      loopInternal = !loopInternal; // Swap relay control source (sw_center vs MIDI)
    if(mode == "CC")
      ccSend(cc_left, 0, CCchannel);
  } else if (btn.is(sw_right)){
    if(sw_left.isPressed())
      loopInternal = !loopInternal; // Swap relay control source (sw_center vs MIDI)
    if(mode == "CC")
      ccSend(cc_right, 0, CCchannel);
  } else if (btn.is(sw_center)) {
    if(mode == "CC" && !center_toggle)
      ccSend(cc_center, 0, CCchannel);
  }
}

void setCCmode() {
  if(debug) Serial.println("CC mode");
  mode = "CC";
  //else // CC mode
  digitalWrite(mainLED, HIGH);
}

void setPCmode() {
  if(debug) Serial.println("PC mode");
  ccSend(cc_center, 0, CCchannel);
  mode = "PC";
}

void ccSend(int cc, int value, int channel) {
  if(debug) {
    debugThis("cc", cc, value);
  } else {
    if(srlMIDI) {
      MIDI.sendControlChange(cc, value, channel);
    }
    if(usbMIDI) {
      midiEventPacket_t event = {0x0B, 0xB0 | channel, cc, value};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
    }
  }
}

void pcSend(int value, int channel) {
  if(debug) {
    debugThis("pc", -1, value);
  } else {
    if(srlMIDI) {
      MIDI.sendProgramChange(value, channel);
    }
    if(usbMIDI) {
      midiEventPacket_t event = {0x0B, 0x0C | channel, value, 0};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
    }
  }
}

void switchLoop(bool state) {
  if(debug) {
    Serial.print("loop: ");
    Serial.println(loopState);    
  }
  digitalWrite(relay1, !state); // Relays are open on high
  digitalWrite(relay2, !state);
  digitalWrite(relayLED, state);
}

void debugThis(String name, int i, int value) {
  if(debug) {
    Serial.print(name);
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(": ");
    Serial.println(value);
  }
}

int mapAndClamp(int input, int i) {
  int inMin[POT_COUNT] = {8,8,8,8};
  int inMax[POT_COUNT] = {1014,1014,1014,1013};
  int outval = map(input, inMin[i], inMax[i], 0, 127);
  if(outval < 0) outval = 0;
  if(outval>127) outval = 127;
  return (outval);
}

int isPotCC(int val) {
  for(int i=0; i<POT_COUNT; i++) {
    if(cc_pot[i] == val)
      return (i);
  }
  return (-1);
}
