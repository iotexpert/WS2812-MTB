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

#ifndef WS2812_H_
#define WS2812_H_
/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"
#include "stdbool.h"
#include "ws2812Hal.h"

#include "FreeRTOS.h"
#include "queue.h"

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */
typedef enum {
	ws2812_cmd_update,            /* no arguments */
	ws2812_cmd_autoUpdate,        /* data is a binary true for autoupdate false for no update  */
	ws2812_cmd_setRGB,            /* data is pixel number + rgb                                */
	ws2812_cmd_setRange,          /* data is 0xFF00 bits for start and 0x00FF bits for y + rgb */
	ws2812_cmd_initMixColorRGB,   /* no arguments, turns string to rgbrgbrgb...                */
}ws2812_cmd_t;

typedef struct {
	ws2812_cmd_t cmd;				/* Command from ws2812_cmd_t enum */
	uint8_t stringNumber;			/* String Number = 1, 2, 3, 4, or 5 */
	uint32_t data;					/**/
	uint8_t red;					/**/
	uint8_t green;					/**/
	uint8_t blue;					/**/
} ws2812_msg_t;

/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */
extern uint8_t channelStatus[MAX_LED_STRINGS];

extern QueueHandle_t ws2812QueueHandle;

/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
extern void ws2812_update(uint8_t stringNumber);
extern void ws2812_autoUpdate(bool option);
extern void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812_initMixColorRGB(uint8_t stringNumber);

extern void ws2812Task(void *arg);

#endif /* WS2812_H_ */
