#define TRIGGER_VAL 8 // sensor trigger value (<val is on, >= val is off)

byte actual = 0; // temp var for holding tone to play
byte anterior;
//int led = 13;
//boolean sonando=false;

int msgMidi(int cmd, int note, int vel){
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(vel);
}

void tocaNota(byte quiero, byte tengo) {
  //digitalWrite(led,LOW);
  if (tengo!=quiero) {
    msgMidi(0x80,tengo,127);
    msgMidi(0x90,quiero,127);
    anterior=quiero;
  }
}

void setup()
{
  //pinMode(led,OUTPUT);
  Serial.begin(31250);
  //Serial.begin(9600);
  startPlayback();
}

void loop ()
{
    actual=0;
    for(char i = 0; i < 8; i++)
    {
      if (readCapacitivePin(i+2) <= TRIGGER_VAL) {
        bitClear(actual,i);  
      }
      else bitSet(actual,i);
    }
    //sonando = (readCapacitivePin(10) <= TRIGGER_VAL);
    switch (actual) {
     case B01111101: ;; //DoA
     case B00000000: tocaNota(0x54,anterior); break; //DoA
     case B00000001: ;; //Si
     case B01110001: tocaNota(0x53,anterior); break; //Si
     case B00000101: ;; //Sib
     case B00000010: ;; //Sib
     case B01110101: tocaNota(0x52,anterior); break; //Sib
     case B00000011: ;; //La
     case B01110011: tocaNota(0x51,anterior); break; //La
     case B00001011: ;; //Lab
     case B00101011: ;; //Lab
     case B01101011: ;; //Lab
     case B01110110: tocaNota(0x50,anterior); break; //Lab
     case B00000111: ;; //Sol
     case B01110111: tocaNota(0x4F,anterior); break; //Sol
     case B00110111: tocaNota(0x4E,anterior); break; //Fa#
     case B00001111: ;; //Fa
     case B11101111: ;; //Fa
     case B01101111: tocaNota(0x4D,anterior); break; //Fa
     case B00011111: tocaNota(0x4C,anterior); break; //Mi
     case B01011111: tocaNota(0x4B,anterior); break; //Mib
     case B00111111: tocaNota(0x4A,anterior); break; //Re
     case B10111111: tocaNota(0x49,anterior); break; //Do#
     case B01111111: tocaNota(0x48,anterior); break; //Do
     case B11111111: tocaNota(0x47,anterior); break; //SiB
     //default: digitalWrite(led,HIGH);
    }
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


void startPlayback()
{
   msgMidi(0xC0,0,109);
   msgMidi(0xC1,0,109);
   msgMidi(0xC2,0,109);
   msgMidi(0x91,0x30,0x30);
   msgMidi(0x92,0x3C,0x30);
   tocaNota(0x48,0);
}
