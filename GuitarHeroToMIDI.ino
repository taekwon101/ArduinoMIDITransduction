#include "MIDIUSB.h"
#define NUM_BUTTONS  4

const uint8_t button1 = 2;
const uint8_t button2 = 3;
const uint8_t button3 = 4;
const uint8_t button4 = 5;
const uint8_t button5 = 6;
const uint8_t button6 = 7;
const uint8_t button7 = 8;

const int ledPin =  16;// the number of the LED pin
int ledState = LOW;

const int intensityPot0 = A0;  //A0 input
const int intensityPot1 = A1;  //A1 input

const uint8_t buttons[NUM_BUTTONS] = {button1}; //, button2, button3, button4, button5, button6, button7};

//const byte notePitches[NUM_BUTTONS] = {48}; //, 50, 52, 53, 55, 56, 58}; //C major scale to assign to up to 7 buttons
const byte notePitches[NUM_BUTTONS] = {52, 55, 57, 59, 62}; //E minor pentatonic to assign to up to 5 buttons


uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

uint8_t intensity0;
uint8_t lastintensity0;
uint8_t intensity1;
uint8_t lastintensity1;

int velocityDefault = 80;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);
}

void loop() {
  readButtons();
  readIntensity();
  if (intensity0 != lastintensity0) {
    controlChange(1, 1, intensity0); // (channel number, control number, control value)
    lastintensity0 = intensity0;
  }
  if (intensity1 != lastintensity1) {
    controlChange(2, 2, intensity1); // (channel number, control number, control value)
    lastintensity1 = intensity1;
  }
  playNotes();
}

void readButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      //delay(50); replace with debounce?
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void readIntensity()
{
  int val0 = analogRead(intensityPot0);
  intensity0 = (uint8_t) (map(val0, 21, 80, 0, 127));
  int val1 = analogRead(intensityPot1);
  intensity1 = (uint8_t) (map(val1, 0, 1023, 0, 127));
}

void controlChange(byte channel, byte control, byte value) 
{
  // First parameter is the event type (0x0B = control change).
  // Second parameter is the event type, combined with the channel.
  // Third parameter is the control number number (0-119).
  // Fourth parameter is the control value (0-127).

  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], velocityDefault);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void troubleshootLED(){
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }
  digitalWrite(ledPin, ledState);
}
