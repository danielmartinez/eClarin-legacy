/*
 Copyright (C) 2015 Daniel Martinez <entaltoaragon@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/
#include <Arduino.h>

/**************************************************************/
// CONFIGURATION
#define SENSOR_PINS             {2,3,4,5,6,7,8,9}
//#define BORDON_SENSOR           A4

#define VS1053_MIDI_SYNTH
#ifdef VS1053_MIDI_SYNTH
  #define XCS_PIN               A3
  #define XDCS_PIN              A2
  #define DREQ_PIN              A1
  #define XRST_PIN              A0
#else
  #define LED                   13 //can't use pin 13 for led if SPI communication is in use
#endif

#define SERIAL_MIDI              1

#define TRIGGER_VAL              8 // sensor trigger value (<val is on, >= val is off)
#ifdef BORDON_SENSOR
  #define BORDON_TRIGGER_VAL    20
  #define BORDONETA_TRIGGER_VAL 35
  #define CLARIN_TRIGGER_VAL    55
#endif

#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
  //#define VERBOSE_DEBUG0   //Digitation events
  //#define VERBOSE_DEBUG1   //Start/stop events
  #undef SERIAL_MIDI
#endif

//#define STORE_CONFIG_ON_EEPROM
#ifdef STORE_CONFIG_ON_EEPROM
  #define EEPROM_ADDR_INSTRUMENT 0
  #define EEPROM_ADDR_BORDON 1
  #define EEPROM_ADDR_BORDONETA 2
  #define EEPROM_ADDR_VOLUME 3
  #define CONFIG_RESET_START_POSITION B00000000
#endif
/**************************************************************/
// AUTOMATIC DEFINES
#ifdef VS1053_MIDI_SYNTH
  #include <SPI.h>
  #include "V1053MidiSynth.h"
  V1053MidiSynth synth(XRST_PIN,DREQ_PIN,XDCS_PIN,XCS_PIN);
#endif
#ifdef SERIAL_MIDI
  #undef SERIAL_DEBUG
  #include <SoftwareSerial.h>
  SoftwareSerial midiSerial(0, SERIAL_MIDI);
#endif
#ifdef STORE_CONFIG_ON_EEPROM
  #include <EEPROM.h>
#endif
#ifdef SERIAL_DEBUG
  #include "GM1.h"
#endif
/**************************************************************/
//INITIALIZATION AND GLOBAL VARS
uint8_t pins[]=SENSOR_PINS;
byte currentpos=0, previouspos;
uint8_t previous=0x48;
uint8_t instrument=109, vol=127;
int fsrValue;
boolean bordon=true, bordoneta=true;
boolean bordonPlaying=false, bordonetaPlaying=false;
boolean playing=false;
/**************************************************************/
int msgMidi(int cmd, int note, int vel) {
#ifdef SERIAL_MIDI
  midiSerial.write(cmd);
  midiSerial.write(note);
  midiSerial.write(vel);
#endif
#ifdef VS1053_MIDI_SYNTH
  synth.midiWriteData(cmd,note,vel);
#endif
}

void setInstrument(uint8_t channel, uint8_t instrument) {
  msgMidi((0xC0 | channel),instrument,0);
}

void noteOn(uint8_t channel, uint8_t note) {
  msgMidi((0x90 | channel),note,120);
}

void noteOff(uint8_t channel, uint8_t note) {
  msgMidi((0x80 | channel),note,120);
}

void playNote(uint8_t want, uint8_t have) {
  if ((have!=want) && playing) {
    noteOff(0,have);
    noteOn(0,want);
    previous=want;
  }
}

void setInstrument() {
  stopPlayback();
  setInstrument(0,instrument);
  setInstrument(1,instrument);
  setInstrument(2,instrument);
  #ifdef SERIAL_DEBUG
    char buffer[30];
    Serial.print("Instrument set to ");
    strcpy_P(buffer,(char*)pgm_read_word(&(instrumentNames[instrument])));
    Serial.println(buffer);
  #endif
  startPlayback();
}

uint8_t readCapacitivePin(int pinToMeasure) {
  // Variables used to translate from Arduino to AVR pin naming
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  port = portOutputRegister(digitalPinToPort(pinToMeasure));
  ddr = portModeRegister(digitalPinToPort(pinToMeasure));
  bitmask = digitalPinToBitMask(pinToMeasure);
  pin = portInputRegister(digitalPinToPort(pinToMeasure));
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  uint8_t SREG_old = SREG; //back up the AVR Status Register
  // Prevent the timer IRQ from disturbing our measurement
  noInterrupts();
  // Make the pin an input with the internal pull-up on
  *ddr &= ~(bitmask);
  *port |= bitmask;

  // Now see how long the pin to get pulled up. This manual unrolling of the loop
  // decreases the number of hardware cycles between each read of the pin,
  // thus increasing sensitivity.
  uint8_t cycles = 17;
       if (*pin & bitmask) { cycles =  0;}
  else if (*pin & bitmask) { cycles =  1;}
  else if (*pin & bitmask) { cycles =  2;}
  else if (*pin & bitmask) { cycles =  3;}
  else if (*pin & bitmask) { cycles =  4;}
  else if (*pin & bitmask) { cycles =  5;}
  else if (*pin & bitmask) { cycles =  6;}
  else if (*pin & bitmask) { cycles =  7;}
  else if (*pin & bitmask) { cycles =  8;}
  else if (*pin & bitmask) { cycles =  9;}
  else if (*pin & bitmask) { cycles = 10;}
  else if (*pin & bitmask) { cycles = 11;}
  else if (*pin & bitmask) { cycles = 12;}
  else if (*pin & bitmask) { cycles = 13;}
  else if (*pin & bitmask) { cycles = 14;}
  else if (*pin & bitmask) { cycles = 15;}
  else if (*pin & bitmask) { cycles = 16;}

  // End of timing-critical section; turn interrupts back on if they were on before, or leave them off if they were off before
  SREG = SREG_old;

  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr  |= bitmask;

  return cycles;
}

uint8_t readPins(){
  uint8_t positions=0, val;
  for (uint8_t i = 0; i < 8; i++) {
    val=readCapacitivePin(pins[i]);
    if (val <= TRIGGER_VAL) bitClear(positions,i);
    else bitSet(positions,i);
    #ifdef VERBOSE_DEBUG0
      Serial.print(val);
      Serial.print(' ');
    #endif
  }
  #ifdef VERBOSE_DEBUG0
    Serial.print(positions,BIN);
    Serial.println(" ");
  #endif
  return positions;
}

void readConfig(){
  #ifdef STORE_CONFIG_ON_EEPROM
    instrument=EEPROM.read(EEPROM_ADDR_INSTRUMENT);
    bordon=EEPROM.read(EEPROM_ADDR_BORDON);
    bordoneta=EEPROM.read(EEPROM_ADDR_BORDONETA);
    vol=EEPROM.read(EEPROM_ADDR_VOLUME);
  #endif
  #ifdef SERIAL_DEBUG
    Serial.println("Settings loaded from EEPROM");
  #endif
}

void writeConfig(){
  #ifdef STORE_CONFIG_ON_EEPROM
    EEPROM.write(EEPROM_ADDR_INSTRUMENT,instrument);
    EEPROM.write(EEPROM_ADDR_BORDON,bordon);
    EEPROM.write(EEPROM_ADDR_BORDONETA,bordoneta);
    EEPROM.write(EEPROM_ADDR_VOLUME,vol);
    #ifdef SERIAL_DEBUG
      Serial.print("Settings saved to EEPROM - Instrument: ");
      Serial.print(instrument);
      Serial.print(", bordon ");
      if (bordon) Serial.print("ON");
      else Serial.print("OFF");
      Serial.print(", bordoneta ");
      if (bordoneta) Serial.print("ON");
      else Serial.print("OFF");
      Serial.print(", volume: ");
      Serial.print(vol);
      Serial.println("");
    #endif
  #endif
}

void startClarin() {
  noteOff(0,previous);
  setInstrument(0,instrument);
  playing=true;
  playNote(previous,0);
  #ifndef VS1053_MIDI_SYNTH
    digitalWrite(LED,HIGH);
  #endif
  #ifdef VERBOSE_DEBUG1
    Serial.println("Clarin start");
  #endif
}

void stopClarin() {
  noteOff(0,previous);
  playing=false;
  #ifndef VS1053_MIDI_SYNTH
    digitalWrite(LED,LOW);
  #endif
  #ifdef VERBOSE_DEBUG1
    Serial.println("Clarin stop");
  #endif
}

void startBordon() {
  noteOff(1,0x30);
  setInstrument(1,instrument);
  if (bordon) noteOn(1,0x30);
  bordonPlaying=true;
  #ifdef VERBOSE_DEBUG1
    Serial.println("Bordon start");
  #endif
}

void stopBordon() {
  noteOff(1,0x30);
  bordonPlaying=false;
  #ifdef VERBOSE_DEBUG1
    Serial.println("Bordon stop");
  #endif
}

void startBordoneta() {
  noteOff(2,0x3C);
  setInstrument(2,instrument);
  if (bordoneta) noteOn(2,0x3C);
  bordonetaPlaying=true;
  #ifdef VERBOSE_DEBUG1
    Serial.println("Bordoneta start");
  #endif
}

void stopBordoneta() {
  noteOff(2,0x3C);
  bordonetaPlaying=false;
  #ifdef VERBOSE_DEBUG1
    Serial.println("Bordoneta stop");
  #endif
}

void startPlayback() {
  startBordon();
  startBordoneta();
  startClarin();
  #ifdef VERBOSE_DEBUG1
    Serial.println("Playback start");
  #endif
}

void stopPlayback() {
  stopClarin();
  stopBordoneta();
  stopBordon();
  #ifdef VERBOSE_DEBUG1
    Serial.println("Playback stop");
  #endif
}

void setup() {
  #ifdef VS1053_MIDI_SYNTH
    synth.begin();
  #else
    pinMode(LED,OUTPUT);
  #endif
  #ifdef SERIAL_MIDI
    midiSerial.begin(31250);
  #elif defined(SERIAL_DEBUG)
    Serial.begin(115200);
  #endif
  #ifdef fsrAnalogPin
    pinMode(fsrAnalogPin, INPUT);
  #endif
  delay(5);
  #ifdef STORE_CONFIG_ON_EEPROM
    if (readPins()==CONFIG_RESET_START_POSITION) {
      writeConfig();
      #ifdef SERIAL_DEBUG
        Serial.print("DEFAULT ");
      #endif
    }
    else {
      readConfig();
      #ifdef SERIAL_DEBUG
        Serial.print("Saved ");
      #endif
    }
  #endif
  #ifdef SERIAL_DEBUG
    Serial.print("Settings loaded - Instrument: ");
    Serial.print(instrument);
    Serial.print(", bordon ");
    if (bordon) Serial.print("ON");
    else Serial.print("OFF");
    Serial.print(", bordoneta ");
    if (bordoneta) Serial.print("ON");
    else Serial.print("OFF");
    Serial.print(", volume: ");
    Serial.print(vol);
    Serial.println("");
  #endif
  setInstrument();
}

void loop() {
    #ifdef BORDON_SENSOR
      fsrValue=analogRead(BORDON_SENSOR);
      if (fsrValue<BORDON_TRIGGER_VAL) {
        if (bordonPlaying) stopBordon();
      }
      else if (!bordonPlaying) startBordon();
      if (fsrValue<BORDONETA_TRIGGER_VAL) {
        if (bordonetaPlaying) stopBordoneta();
      }
      else if (!bordonetaPlaying) startBordoneta();
      if (fsrValue<CLARIN_TRIGGER_VAL)  {
        if (playing) stopClarin();
      }
      else if (!playing) startClarin();
    #endif
    currentpos=readPins();
    if (currentpos!=previouspos)
    switch (currentpos) {
     case B01111101: ;; //DoA
     case B00000000: playNote(0x54,previous); break; //DoA
     case B00000001: ;; //Si
     case B01110001: playNote(0x53,previous); break; //Si
     case B00000101: ;; //Sib
     case B00000010: ;; //Sib
     case B01110101: playNote(0x52,previous); break; //Sib
     case B00000011: ;; //La
     case B01110011: playNote(0x51,previous); break; //La
     case B00001011: ;; //Lab
     case B00101011: ;; //Lab
     case B01101011: ;; //Lab
     case B01110110: playNote(0x50,previous); break; //Lab
     case B00000111: ;; //Sol
     case B01110111: playNote(0x4F,previous); break; //Sol
     case B00110111: playNote(0x4E,previous); break; //Fa#
     case B00001111: ;; //Fa
     case B11101111: ;; //Fa
     case B01101111: playNote(0x4D,previous); break; //Fa
     case B00011111: playNote(0x4C,previous); break; //Mi
     case B01011111: playNote(0x4B,previous); break; //Mib
     case B00111111: playNote(0x4A,previous); break; //Re
     case B10111111: playNote(0x49,previous); break; //Do#
     case B01111111: playNote(0x48,previous); break; //Do
     case B11111111: playNote(0x47,previous); break; //SiB
     
     case B10000010: ++instrument%=128; setInstrument(); writeConfig(); break;
     case B10000100: --instrument%=128; setInstrument(); writeConfig(); break;
     case B10001000: if (bordon=!bordon) startBordon();
                     else stopBordon();
                     #ifdef SERIAL_DEBUG
                       Serial.print("Set bordon ");
                       if (bordon) Serial.println("ON");
                       else Serial.println("OFF");
                     #endif
                     writeConfig(); break;
     case B10010000: if (bordoneta=!bordoneta) startBordoneta();
                     else stopBordoneta();
                     #ifdef SERIAL_DEBUG
                       Serial.print("Set bordoneta ");
                       if (bordoneta) Serial.println("ON");
                       else Serial.println("OFF");
                     #endif
                     writeConfig(); break;
     case B10100000: if (playing=!playing) startClarin();
                     else stopClarin(); break;
     /*case B10000011: msgMidi(0xB0,0x07,vol+=5); break;
     case B10000101: msgMidi(0xB0,0x07,vol-=5); break;*/
    }
    previouspos=currentpos;
}
