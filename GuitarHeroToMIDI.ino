#include "MIDIUSB.h"

#define NUM_BUTTONS  10
#define NUM_POTS 3

const int ledPin =  16;// the number of the LED pin
int ledState = LOW;

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

const uint8_t frets[NUM_BUTTONS] = {fret1, fret2, fret3, fret4, fret5, fret6, fret7, fret8, fret9, fret10};
const uint8_t pots[NUM_POTS] = {whammy, pot1, pot2};

uint8_t intensity[NUM_POTS];
uint8_t lastintensity[NUM_POTS];
uint8_t valRaw[NUM_POTS];
uint8_t valMap[NUM_POTS];
uint8_t lastValMap[NUM_POTS];
uint8_t minMap[NUM_POTS] = {21, 0, 0};
uint8_t maxMap[NUM_POTS] = {80, 127, 127};

const byte notePitches[NUM_BUTTONS] = {52, 55, 57, 59, 62, 64, 67, 69, 71, 74}; //E minor pentatonic repeated through 2 octaves to assign to up to 10 buttons

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

int velocityDefault = 80;

bool strummer = true; // permanently true until strummer sensing is implemented

void setup() {
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(frets[i], INPUT_PULLUP);
}

void loop() {
  readNotes();
  readCC();
  playNotes();
  MidiUSB.flush();
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
          noteOn(0, notePitches[i], velocityDefault);
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
