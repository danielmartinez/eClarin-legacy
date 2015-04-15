/*
 * V1053MidiSynth.cpp
 * A library for MusicShield 2.0
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Jack Shao (jacky.shaoxg@gmail.com)
 * Create Time: Mar 2014
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "V1053MidiSynth.h"
#include  <avr/pgmspace.h>

V1053MidiSynth::V1053MidiSynth(uint8_t _VS_XRESET, uint8_t _VS_DREQ,
  uint8_t _VS_XDCS, uint8_t _VS_XCS):
  VS_XRESET(_VS_XRESET), VS_DREQ(_VS_DREQ), VS_XDCS(_VS_XDCS), VS_XCS(_VS_XCS)
{
}

/**********************************Midi Player Part***************************/

/*
 **@ function:beginInMidiFmt
 **@ usage:Init the Midi Player
 **@ input:none
 **@ output:none
*/
void V1053MidiSynth::begin(void)
{

  //Init VS105B in Midi Format
  VS10XX vs1053(VS_XRESET,VS_DREQ,VS_XDCS,VS_XCS);
  Serial.print("Init vs10xx in MIDI format...");
  vs1053.initForMidiFmt();
  Serial.print("done\r\n");

}

/*
 **@ function:midiWriteData
 **@ usage:Write data to Midi Player
 **@ input:
 **@ output:none
*/
void V1053MidiSynth::midiWriteData(byte cmd, byte high, byte low)
{
  while(!digitalRead(VS_DREQ));
  digitalWrite(VS_XDCS, LOW);
  midiSendByte(cmd);
  if((cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0)
  {
    midiSendByte(high);
    midiSendByte(low);
  }
  else
  {
    midiSendByte(high);
  }
  digitalWrite(VS_XDCS, HIGH);
}


void V1053MidiSynth::midiNoteOn(byte channel, byte note, byte rate)
{
  midiWriteData((0x90 | channel), note, rate);
}

void V1053MidiSynth::midiNoteOff(byte channel, byte note, byte rate)
{
  midiWriteData((0x80 | channel), note, rate);
}

void V1053MidiSynth::midiSendByte(byte data)
{
  SPI.transfer(0x00);
  SPI.transfer(data);
}

/*
 **@ function:demoPlayer
 **@ usage:A Midi demo Player
 **@ input:none
 **@ output:none
*/
void V1053MidiSynth::midiDemoPlayer(void)
{
  delay(1000);
  midiWriteData(0xB0, 0x07, 120);

  //GM2 Mode
  Serial.print("Fancy Midi Sounds\r\n");
  midiWriteData(0xB0, 0, 0x78);
  for(int instrument = 30 ; instrument < 31 ; instrument++)
  {
    Serial.print(" Instrument: ");
    Serial.println(instrument, DEC);

    midiWriteData(0xC0, instrument, 0);    //Set instrument number. 0xC0 is a 1 data byte command

    //Play notes from F#-0 (30) to F#-5 (90):
    for (int note = 27 ; note < 87 ; note++)
    {
      Serial.print("N:");
      Serial.println(note, DEC);
      //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
      midiNoteOn(0, note, 127);
      delay(50);

      //Turn off the note with a given off/release velocity
      midiNoteOff(0, note, 127);
      delay(50);
    }

    //delay 100ms between each instruments
    delay(100);
    }
}


