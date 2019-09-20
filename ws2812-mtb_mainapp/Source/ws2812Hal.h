/** @source __WS2812.c__
**
** __Code for the WS2812 Example Application_
**
** @author Copyright (C) June 15, 2019 __Alan Hawse__
** @version __1.0__   __Gen 1__</replaceable>
** @modified ____7/25/2019__Hassane El-Khoury__  __description of edit__</replaceable>
** @@
**
********************************************************************/
#ifndef WS2812HAL_H
#define WS2812HAL_H
/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
/* Header file includes */
#include "cy_gpio.h"
#include <stdint.h>

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */
/* Maximum number of channels; Hardware limited by number of SCBs */
#define MAX_LED_STRINGS 		(9u)

/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
extern uint32_t WS_getNumLeds(uint8_t string);
extern void WS_Start(void);
extern uint8_t WS_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds);
extern void WS_updateString(uint8_t string);
extern void WS_setRGB(uint8_t string, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
extern void WS_setRange(uint8_t string, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);

#endif
