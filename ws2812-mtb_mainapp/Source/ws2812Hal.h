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

/* Define the system Gamma correction on or off to be used. Same for all channels */
#define ws2812_GAMMA_CORRECTION   	ws2812_GAMMA_ON
#define ws2812_GAMMA_ON            	1
#define ws2812_GAMMA_OFF           	0

/* Define the Coordinate warp to be used. Same for all channels */
#define ws2812_COORD_WRAP        ws2812_COORD_NONE
#define ws2812_COORD_NONE        (0u)
#define ws2812_COORD_XAXIS       (1u)
#define ws2812_COORD_YAXIS       (2u)
#define ws2812_COORD_XYAXIS      (3u)

/* Define the LED Layout used. Each channel has its own layout */
#define ws2812_LED_LAYOUT_STANDARD  (0u)
#define ws2812_LED_LAYOUT_SPIRAL    (1u)
#define ws2812_LED_LAYOUT_GRID16X16 (2u)

#define ws2812_FLIP_X_COORD      0
#define ws2812_FLIP_Y_COORD      0

#define ws2812_FLIP_COORD         (1u)
#define ws2812_NOT_FLIP_COORD     (0u)

#define ws2812_SWAP_XY_COORD      0
#define ws2812_XY_SWAPED          (1u)

#define ws2812_GRIDMODE_NORMAL  1
#define ws2812_GRIDMODE_WRAP    0
/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
extern uint32_t ws2812HAL_getNumLeds(uint8_t string);
extern void ws2812HAL_Start(void);
int8_t ws2812HAL_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds, uint32_t numRows,
			uint32_t numColumns, uint8_t ledLayout, uint8_t coordWarp);
extern bool ws2812HAL_updateString(uint8_t string);
extern void ws2812HAL_setPixelRGB(uint8_t string, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812HAL_setAllRGB(uint8_t string, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812HAL_setPixelColor(uint8_t string, uint32_t led, uint32_t color);
extern void ws2812HAL_setAllColor(uint8_t string, uint32_t color);
extern void ws2812HAL_setPixelArrayColor(uint8_t string, uint32_t y, uint32_t x, uint32_t color);
extern void ws2812HAL_setPixelArrayRGB(uint8_t string, uint32_t y, uint32_t x, uint8_t red, uint8_t green, uint8_t blue);
extern uint32_t ws2812_GetPixelColor(uint8_t string, int32_t x, int32_t y);

#endif
