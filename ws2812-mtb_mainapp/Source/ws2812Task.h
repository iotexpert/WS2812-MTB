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
#ifndef WS2812GRAPHICS_H
#define WS2812GRAPHICS_H

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
#include "ws2812HAL.h"

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */


/* Define the enum for the theme and LED light show control */
typedef enum {
	themeChristmas,
	themeFourthJuly,
	themeNewYear,
	themeHalloween,
}themeLight_t;

/* Define the enum for what type of animation to use */
typedef enum {
	animationChaseSingle,
	animationChaseTail,
	animationOnOff,
	animationRainbow,
	animationAlternate,
	animationBlend,
	animationRoundRobin,
}animation_t;

/* Define the struct for the Theme being loaded */
typedef struct {
	themeLight_t themeLight;
	uint32_t themeColor[4];
	animation_t themeAnimation;
	uint8_t themeChannelOrder[MAX_LED_STRINGS];
	uint32_t themeDelay;
}themeParameters_t;

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
extern void ws2812LightShowTask(void *arg);

#endif
