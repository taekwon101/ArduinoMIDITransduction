#include "MIDIUSB.h"

#define NUM_BUTTONS  10
#define NUM_POTS 3

//transducer input assignments
const uint8_t fret1 = 2;
const uint8_t fret2 = 3;
const uint8_t fret3 = 4;
const uint8_t fret4 = 5;
const uint8_t fret5 = 6;
const uint8_t fret6 = 7;
const uint8_t fret7 = 8;
const uint8_t fret8 = 9;
const uint8_t fret9 = 10;
const uint8_t fret10 = 11;

const uint8_t whammy = A0;
const uint8_t pot1 = A1;
const uint8_t pot2 = A2;

//const int ledPin =  16;

//lists for loop cycling
const uint8_t frets[NUM_BUTTONS] = {fret1, fret2, fret3, fret4, fret5, fret6, fret7, fret8, fret9, fret10};
const uint8_t pots[NUM_POTS] = {whammy, pot1, pot2};

//transduction value and statechange detection
uint8_t valRaw[NUM_POTS];
uint8_t valMap[NUM_POTS];
uint8_t lastValMap[NUM_POTS];

//voltage mapping for valid/desirable MIDI signal ranges (21-80 for limited range whammy bar pot)
uint8_t minMap[NUM_POTS] = {21, 0, 0};
uint8_t maxMap[NUM_POTS] = {80, 1023, 1023};

//MIDI note value assignment
const byte notePitches[NUM_BUTTONS] = {52, 55, 57, 59, 62, 64, 67, 69, 71, 74}; //E minor pentatonic repeated through 2 octaves to assign to up to 10 buttons

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

//temp bandaid values while prototyping
bool strummer = true; // permanently true until strummer sensing is implemented
int strumVelocity = 80; // permanently 80 until strummer intensity is implemented

void setup() {
  //pinMode(ledPin, OUTPUT);
  int ledState = LOW;
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(frets[i], INPUT_PULLUP);
}

void loop() {
  readNotes(); // check fret button state and record all that have changed
  readCC(); // check potentiometer voltage state and queue remapped MIDI CC values for all that have changed
  playNotes(); // queue note changes and with strum velocities and check strum condition for those intended to require it
  MidiUSB.flush(); // send queued MIDI data through USB
}

void readNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(frets[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      //delay(50); replace with debounce?
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void readCC() 
{
  for (int i = 0; i < NUM_POTS; i++)
  {
    valRaw[i] = analogRead(pots[i]);
    valMap[i] = (byte) (map(valRaw[i], minMap[i], maxMap[i], 0, 127));
    if (valMap[i] != lastValMap[i]) 
    {
      midiEventPacket_t event = {0x0B, 0xB0 | i, 1, valMap};
      MidiUSB.sendMIDI(event);
      valMap[i] = lastValMap[i];  
    }
  }
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        if ((i <= 5) || (strummer == true))
        {
          bitWrite(previousButtons, i , 1);
          noteOn(0, notePitches[i], strumVelocity);
        }
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
      }
    }
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
