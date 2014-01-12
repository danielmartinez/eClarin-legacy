#include <SoftwareSerial.h>

#define MIDISerialPin          1
//#define MIDIResetPin          A0
#define led                   13
#define fsrAnalogPin          A0

#define TRIGGER_VAL            8 // sensor trigger value (<val is on, >= val is off)
#define BORDON_TRIGGER_VAL    20
#define BORDONETA_TRIGGER_VAL 35
#define CLARIN_TRIGGER_VAL    55

byte currentpos=0;
byte previous=0x48;
byte previouspos;
byte instrument=109, volume=127;
int fsrValue;

boolean bordon=true, bordoneta=true;
boolean bordonPlaying=true, bordonetaPlaying=true;
boolean playing=true;

SoftwareSerial midiSerial(0, MIDISerialPin);

int msgMidi(int cmd, int note, int vel) {
  midiSerial.write(cmd);
  midiSerial.write(note);
  midiSerial.write(vel);
}

void playNote(byte want, byte have) {
  if ((have!=want) && playing) {
    msgMidi(0xB0,123,0);
    msgMidi(0x90,want,127);
    previous=want;
  }
}

void instrumentPreview() {
  msgMidi(0xB0,123,0);
  msgMidi(0xB1,123,0);
  msgMidi(0xB2,123,0);
  msgMidi(0xC0,0,instrument);
  msgMidi(0xC1,0,instrument);
  msgMidi(0xC2,0,instrument);
  msgMidi(0x90,0x51,127);
  if (bordon) msgMidi(0x91,0x30,0x30);
  if (bordoneta) msgMidi(0x92,0x3C,0x30);
  previous=0;
}

void setup() {
  pinMode(led,OUTPUT);
  midiSerial.begin(31250);
  /*
  pinMode(MIDIResetPin, OUTPUT);
  digitalWrite(MIDIResetPin, LOW);
  delay(100);
  digitalWrite(MIDIResetPin, HIGH);
  delay(100);
  */
  pinMode(fsrAnalogPin, INPUT);
  startPlayback();
}

void loop () {
    fsrValue=analogRead(fsrAnalogPin);
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
    currentpos=0;
    for (char i = 0; i < 8; i++) {
      if (readCapacitivePin(i+2) <= TRIGGER_VAL) bitClear(currentpos,i);  
      else bitSet(currentpos,i);
    }
    //playing = (readCapacitivePin(10) <= TRIGGER_VAL);
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
     
     case B10000010: msgMidi(0xC0,0,++instrument); instrumentPreview(); break;
     case B10000100: msgMidi(0xC0,0,--instrument); instrumentPreview(); break;
     case B10001000: if (bordon=!bordon) startBordon();
                     else stopBordon(); break;
     case B10010000: if (bordoneta=!bordoneta) startBordoneta();
                     else stopBordoneta(); break;
     case B10100000: if (!(playing=!playing)) stopPlayback();
                     else startPlayback();
                     break;
     case B10000011: msgMidi(0xB0,0x07,volume+=5); break;
     case B10000101: msgMidi(0xB0,0x07,volume-=5); break;
    }
    previouspos=currentpos;
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

void startPlayback() {
  startBordon();
  startBordoneta();
  startClarin();
}

void stopPlayback() {
  stopClarin();
  stopBordoneta();
  stopBordon();
}

void startClarin() {
  msgMidi(0xB0,123,0);
  msgMidi(0xC0,0,instrument);
  playing=true;
  playNote(previous,0);
  digitalWrite(led,HIGH);
}

void stopClarin() {
  msgMidi(0xB0,123,0);
  playing=false;
  digitalWrite(led,LOW);
}

void startBordon() {
  msgMidi(0xB1,123,0);
  msgMidi(0xC1,0,instrument);
  if (bordon) msgMidi(0x91,0x30,0x30);
  bordonPlaying=true;
}

void stopBordon() {
  msgMidi(0xB1,123,0);
  bordonPlaying=false;
}

void startBordoneta() {
  msgMidi(0xB2,123,0);
  msgMidi(0xC2,0,instrument);
  if (bordoneta) msgMidi(0x92,0x3C,0x30);
  bordonetaPlaying=true;
}

void stopBordoneta() {
  msgMidi(0xB2,123,0);
  bordonetaPlaying=false;
}
