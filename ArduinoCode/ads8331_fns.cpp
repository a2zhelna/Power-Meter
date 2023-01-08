//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

#include <SPI.h>
#include "ads8331_fns.h"
#include "pga113_fns.h"

//ADC active-low chip select pin
#define CS 0

//ADC active-low convst (start conversion) pin
#define CONVST 4

void adcSetup() {
  delay(500);       //Should hypothetically give some leeway time for everything to power up before sending any signals.

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  
  SPI.setDataMode(SPI_MODE2);

  digitalWrite(CS, 1);
  digitalWrite(CONVST, 1);

  delayMicroseconds(2);

  //Configuring ADC
  digitalWrite(CS, 0);
  //1110 0110 1111 1101 = 0xE6FD
  SPI.transfer16(0xE6FD);  
  digitalWrite(CS, 1);

  delayMicroseconds(5);

  //SPI_MODE0 is the only mode which properly reads the conversion data
  //(data isn't left shifted)
  SPI.setDataMode(SPI_MODE0);
  delayMicroseconds(5);
  //Setting channel 0 as MUX output
  digitalWrite(CS, 0);
  SPI.transfer16(0x0000);  
  digitalWrite(CS, 1);
  
  delayMicroseconds(2);
}

int readChannel(int next_ch, int gain) {
  //Because with SPI_MODE0 (the required mode for properly reading config data),
  //the sent channel select data is left shifted, I send right shifted channel select data.
  next_ch >>= 1;
  
  //Set appropriate gain
  setGain(gain);
  //This delay is needed to allow the PGA's gain to fully change (>200ns)
  //before reading voltage
  //NOTE: The greater the delay, the less fluctuations/noise at the output
  //of the PGA (due to less frequent switching)
  delayMicroseconds(5);         //This value needs to be properly adjusted
  
  //Get conversion data
  digitalWrite(CONVST, 0);
  delayMicroseconds(2);
  digitalWrite(CONVST, 1);
  
  //Read conversion data
  digitalWrite(CS, 0);
  int readings = SPI.transfer16(next_ch);
  digitalWrite(CS, 1);

  return readings;
}
