//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

#include <SPI.h>
#include "pga113_fns.h"

//PGA active low Chip select
//#define CS1 16
#define CS 2

void pgaSetup() {
  digitalWrite(CS, 1);
  delayMicroseconds(2);

  //Shutdown enable
  digitalWrite(CS, 0);
  SPI.transfer16(0xE1F1);
  digitalWrite(CS, 1);

  delayMicroseconds(10);

  //Shutdown disable
  digitalWrite(CS, 0);
  SPI.transfer16(0xE100);
  digitalWrite(CS, 1);

  delayMicroseconds(5);

  //Set lowest gain and set to channel 1
  digitalWrite(CS, 0);
  SPI.transfer16(0x2A01);
  digitalWrite(CS, 1);
}

void setGain(int gain) {
  digitalWrite(CS, 0);
  
  //To write the gain to the PGA's channel 1: 
  //General hex:  0x2Ax1
  //Gain =  1:    0x2A01
  //Gain =  2:    0x2A11
  //Gain =  5:    0x2A21
  //Gain = 10:    0x2A31
  switch (gain) {
      case (1):
        SPI.transfer16(0x2A01);
      break;
      case (2):
        SPI.transfer16(0x2A11);
      break;
      case (5):
        SPI.transfer16(0x2A21);
      break;
      case (10):
        SPI.transfer16(0x2A31);
      break;
    }
    
  digitalWrite(CS, 1);
}
