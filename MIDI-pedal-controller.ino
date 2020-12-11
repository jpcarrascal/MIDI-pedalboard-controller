#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <Bounce2.h>
#include <MIDI.h>

const bool debug = false;
PushButton sw_left = PushButton(2, ENABLE_INTERNAL_PULLUP);
PushButton sw_right = PushButton(3, ENABLE_INTERNAL_PULLUP);
PushButton sw_center = PushButton(4, ENABLE_INTERNAL_PULLUP);
const int pot1 = A1;
const int pot2 = A0;
int pot1val, pot2val;

const int ledPin =  9;
const long interval = 300; // blink interval
const int PCchannel = 13;
const int CCchannel = 1;
int currentProgram = 0;
int ledState = HIGH;
unsigned long previousMillis = 0;
String mode = "CC"; // "CC" = Control Change, PC" = Program Change
const int cc_left   = 0;
const int cc_right  = 1;
const int cc_center = 2;
const int cc_pot1   = 3;
const int cc_pot2   = 4;

MIDI_CREATE_DEFAULT_INSTANCE();
void setup() {
        // MIDI baud rate
        if(debug)
          Serial.begin(9600);
        else
          Serial.begin(31250);
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, HIGH);
        pot1val = analogRead(pot1);
        pot2val = analogRead(pot2);

        // Footswitch press and release callbacks
        sw_left.onPress(onButtonPressed);
        sw_center.onPress(onButtonPressed);
        sw_right.onPress(onButtonPressed);

        sw_left.onRelease(onButtonReleased);
        sw_center.onRelease(onButtonReleased);
        sw_right.onRelease(onButtonReleased);
        
        // If center footswitch is been held for 1.5 seconds, switch to PC mode
        sw_center.onHold(1500, setPCmode);
}

void loop() {
  sw_left.update();
  sw_right.update();
  sw_center.update();
  unsigned long currentMillis = millis();
  if(mode == "PC") {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    } 
  }
  else // CC mode
    digitalWrite(ledPin, HIGH);

  int pot1valnew = analogRead(pot1);
  int pot2valnew = analogRead(pot2);
  debugControl("pot 1", pot1valnew);
  if( abs(pot1val - pot1valnew) > 7 && mode == "CC") {
    int outval = mapAndClamp(pot1valnew);
    MIDI.sendControlChange(cc_pot1, outval, CCchannel);
    pot1val = pot1valnew;
  }
  if( abs(pot2val - pot2valnew) > 7 && mode == "CC") {
    int outval = mapAndClamp(pot2valnew);
    MIDI.sendControlChange(cc_pot2, outval, CCchannel);
    pot2val = pot2valnew;
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
            MIDI.sendProgramChange(currentProgram, PCchannel);
          }
          else
            MIDI.sendControlChange(cc_left, 127, CCchannel);
        } else if (btn.is(sw_right)){
          if(mode == "PC") {
            if(currentProgram < 63)
              currentProgram++;
            else
              currentProgram = 0;
            MIDI.sendProgramChange(currentProgram, PCchannel);
          }
          else
            MIDI.sendControlChange(cc_right, 127, CCchannel);
        } else if (btn.is(sw_center)) {
          if(mode == "PC")
            setCCmode();
          else
            MIDI.sendControlChange(cc_center, 127, CCchannel);
        }
}

void onButtonReleased(Button& btn, uint16_t duration){
        if(btn.is(sw_left)) {
          if(mode == "CC")
            MIDI.sendControlChange(cc_left, 0, CCchannel);
        } else if (btn.is(sw_right)){
          if(mode == "CC")
            MIDI.sendControlChange(cc_right, 0, CCchannel);
        } else if (btn.is(sw_center)) {
          if(mode == "CC")
            MIDI.sendControlChange(cc_center, 0, CCchannel);
        }
}

void setCCmode() {
  mode = "CC";
}

void setPCmode() {
  MIDI.sendControlChange(cc_center, 0, CCchannel);
  mode = "PC";
}

void debugControl(String n, int v) {
  if (debug) {
    Serial.print(n);
    Serial.print(":\t");
    Serial.println(v); 
  }
}

int mapAndClamp(int input) {
  if (debug) {
    Serial.print("input: ");
    Serial.println(input);
  }
  int outval = map(input, 5, 1015, 0, 127);
  if(outval < 0) outval = 0;
  if(outval>127) outval = 127;
  return (outval);
}

