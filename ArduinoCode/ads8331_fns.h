//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

#ifndef ADC_FNS_H
#define ADC_FNS_H
#include <Arduino.h>

void adcSetup();

int readChannel(int next_ch, int gain);

#endif
