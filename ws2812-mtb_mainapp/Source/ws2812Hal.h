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

#define ws2812_CWHEEL_SIZE  24

#define ws2812_RED_MASK   0x0000FF00
#define ws2812_GREEN_MASK 0x000000FF
#define ws2812_BLUE_MASK  0x00FF0000

#define ws2812_YELLOW      ws2812HAL_getColor(1)
#define ws2812_GREEN       ws2812HAL_getColor((70 + ws2812_CWHEEL_SIZE))
#define ws2812_ORANGE      ws2812HAL_getColor(20)
#define ws2812_BLACK       ws2812HAL_getColor((0 + ws2812_CWHEEL_SIZE))
#define ws2812_OFF         ws2812HAL_getColor((0 + ws2812_CWHEEL_SIZE))
#define ws2812_LTBLUE      ws2812HAL_getColor((1 + ws2812_CWHEEL_SIZE))
#define ws2812_MBLUE       ws2812HAL_getColor((2 + ws2812_CWHEEL_SIZE))
#define ws2812_BLUE        ws2812HAL_getColor((3 + ws2812_CWHEEL_SIZE))
#define ws2812_LTGREEN     ws2812HAL_getColor((4 + ws2812_CWHEEL_SIZE))
#define ws2812_MGREEN      ws2812HAL_getColor((8 + ws2812_CWHEEL_SIZE))
//#define ws2812_GREEN       (12 + ws2812_CWHEEL_SIZE)
#define ws2812_LTRED       ws2812HAL_getColor((16 + ws2812_CWHEEL_SIZE))
#define ws2812_LTYELLOW    ws2812HAL_getColor((20 + ws2812_CWHEEL_SIZE))
#define ws2812_MGRED       ws2812HAL_getColor((32 + ws2812_CWHEEL_SIZE))
#define ws2812_RED         ws2812HAL_getColor((48 + ws2812_CWHEEL_SIZE))
#define ws2812_MAGENTA     ws2812HAL_getColor((51 + ws2812_CWHEEL_SIZE))
#define ws2812_RGB_WHITE   ws2812HAL_getColor((63 + ws2812_CWHEEL_SIZE))

#define ws2812_SPRING_GREEN ws2812HAL_getColor((64 + ws2812_CWHEEL_SIZE))
#define ws2812_TURQUOSE    ws2812HAL_getColor((65 + ws2812_CWHEEL_SIZE))
#define ws2812_CYAN        ws2812HAL_getColor((66 + ws2812_CWHEEL_SIZE))
#define ws2812_OCEAN       ws2812HAL_getColor((67 + ws2812_CWHEEL_SIZE))
#define ws2812_VIOLET      ws2812HAL_getColor((68 + ws2812_CWHEEL_SIZE))
#define ws2812_RASPBERRY   ws2812HAL_getColor((69 + ws2812_CWHEEL_SIZE))
#define ws2812_DIM_WHITE   ws2812HAL_getColor((71 + ws2812_CWHEEL_SIZE))
#define ws2812_DIM_BLUE    ws2812HAL_getColor((72 + ws2812_CWHEEL_SIZE))
#define ws2812_INVISIBLE   ws2812HAL_getColor((73 + ws2812_CWHEEL_SIZE))

#define ws2812_COLD_TEMP   ws2812HAL_getColor((80 + ws2812_CWHEEL_SIZE))
#define ws2812_HOT_TEMP    ws2812HAL_getColor((95 + ws2812_CWHEEL_SIZE))

#define ws2812_FIRE_DARK   ws2812HAL_getColor((74 + ws2812_CWHEEL_SIZE))
#define ws2812_FIRE_LIGHT  ws2812HAL_getColor((75 + ws2812_CWHEEL_SIZE))

#define ws2812_FULL_WHITE  ws2812HAL_getColor((76 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_LED   ws2812HAL_getColor((77 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_RGB50 ws2812HAL_getColor((78 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_RGB25 ws2812HAL_getColor((79 + ws2812_CWHEEL_SIZE))

#define ws2812_CLUT_SIZE  (96 + ws2812_CWHEEL_SIZE)

//#define ws2812_WHITE   ws2812HAL_getColor((63 + ws2812_CWHEEL_SIZE))
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

extern uint32_t ws2812HAL_getColor( uint32_t color);
extern uint32_t ws2812HAL_ColorInc(uint32_t incValue);
extern uint32_t ws2812HAL_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct);

#endif
