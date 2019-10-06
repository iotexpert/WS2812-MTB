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
#include "ws2812Hal.h"

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */


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
extern void ws2812_MixColorRGB(uint32_t stringNumber, uint32_t row, uint32_t numLEDs);
extern void ws2812_FadeInFadeOutRGB(uint8_t stringNumber, uint32_t row, uint32_t column, uint8_t red, uint8_t blue, uint8_t green, bool fadeIn, bool fadeOut);
extern void ws2812_FadeInFadeOutColor(uint8_t stringNumber,  uint32_t row, uint32_t column, uint32_t color, bool fadeIn, bool fadeOut);
extern void ws2812_StrobeRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
extern void ws2812_StrobeColor(uint8_t stringNumber, uint32_t colorRequested, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
extern void ws2812_NewKITT(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
extern void ws2812_CenterToOutside(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
extern void ws2812_OutsideToCenter(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
extern void ws2812_LeftToRight(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
extern void ws2812_RightToLeft(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);

extern void ws2812_HalloweenEyesRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeWidth, uint32_t EyeSpace,
		bool Fade, uint32_t Steps, uint32_t FadeDelay, uint32_t EndPause);
extern void ws2812_TwinkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne);
extern void ws2812_TwinkleRandom(uint8_t stringNumber, uint32_t numLEDs, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne) ;
extern void ws2812_SparkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay);
extern void ws2812_SnowSparkle(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SparkleDelay, uint32_t SpeedDelay);
extern void ws2812_RunningLights(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t WaveDelay);
extern void ws2812_ColorWipe(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay);
extern void ws2812_Rainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t startColor, uint32_t SpeedDelay);
extern void ws2812_RainbowCycle(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay);
extern void ws2812_TheaterChase(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay) ;
extern void ws2812_TheaterChaseRainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay) ;
extern void ws2812_Fire(uint8_t stringNumber, uint32_t numLEDs, uint32_t Cooling, uint32_t Sparking, uint32_t SpeedDelay);
extern void ws2812_BouncingBalls(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t BallCount);
extern void ws2812_BouncingColoredBalls(uint8_t stringNumber, uint32_t numLEDs, uint32_t BallCount, uint8_t colors[][3]);

extern void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color);

#endif
