
#ifndef WS2812HAL_H
#define WS2812HAL_H

#include "cy_gpio.h"
#include <stdint.h>


uint32_t WS_getNumLeds(uint32_t string);

void WS_setRGB(int string,int led,uint8_t red, uint8_t green, uint8_t blue);
void WS_setRange(int string,int start, int end, uint8_t red,uint8_t green ,uint8_t blue);

void WS_runTest();
void WS_Start();
int WS_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin,int numPixels);

void WS_updateString(uint32_t string);

#endif
