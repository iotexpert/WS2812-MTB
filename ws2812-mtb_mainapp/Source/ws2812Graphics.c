/** __Code for the WS2812 Example Application_
**
** @author Copyright (C) June 15, 2019 __Alan Hawse__
** @version __1.0__   __Gen 1__</replaceable>
** @modified ____7/25/2019__Hassane El-Khoury__  __description of edit__</replaceable>
** @@
**
**Library ported from the great work done by:
**https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
**Ported into PSoC6 and ModusToolbox, with FreeRTOS
********************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ws2812Graphics.h"

/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */
#if( ws2812_GAMMA_CORRECTION == ws2812_GAMMA_ON )
const uint8_t ws2812_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
#endif

const uint32 ws2812_CLUT[ ] = {
//xxBBRRGG (WS2812)
//     (24)   [ Index = 0 ]
0x0000FFFF,  // 0 Yellow
0x0000CCFF,
0x000099FF,
0x000033FF,
0x000000FF,  // 5  Green
0x006600B3,
0x00990099,
0x00B30066,
0x00CC0033,  // 9 Blue
0x00B31919,
0x00993300,
0x00994000,
0x00996600,
0x00999900,
0x0099CC00,
0x0066E600,
0x0000FF00, // Red
0x0000FF33,
0x0000FF66,
0x0000FF80,
0x0000FF99,  // 20 Orange
0x0000FFB2,
0x0000FFCC,
0x0000FFE5,
// #24

//xxBBRRGG  (64)  [ Index = 24 ]
0x00000000, 0x00550000, 0x00AA0000, 0x00FF0000,  // 0, Black,  LtBlue, MBlue, Blue
0x00000055, 0x00550055, 0x00AA0055, 0x00FF0055,  // 4, LtGreen
0x000000AA, 0x005500AA, 0x00AA00AA, 0x00FF00AA,  // 8, MGreen
0x000000FF, 0x005500FF, 0x00AA00FF, 0x00FF00FF,  // 12 Green

0x00005500, 0x00555500, 0x00AA5500, 0x00FF5500,  // 16, LtRed
0x00005555, 0x00555555, 0x00AA5555, 0x00FF5555,  // 20, LtYellow
0x000055AA, 0x005555AA, 0x00AA55AA, 0x00FF55AA,  // 24,
0x000055FF, 0x005555FF, 0x00AA55FF, 0x00FF55FF,  // 28,

0x0000AA00, 0x0055AA00, 0x00AAAA00, 0x00FFAA00,  // 32, MRed
0x0000AA55, 0x0055AA55, 0x00AAAA55, 0x00FFAA55,  // 36,
0x0000AAAA, 0x0055AAAA, 0x00AAAAAA, 0x00FFAAAA,  // 55,
0x0000AAFF, 0x0055AAFF, 0x00AAAAFF, 0x00FFAAFF,  // 44,

0x0000FF00, 0x0055FF00, 0x00AAFF00, 0x00FFFF00,  // 48, Red, ??, ??, Magenta
0x0000FF55, 0x0055FF55, 0x00AAFF55, 0x00FFFF55,  // 52,
0x0000FFAA, 0x0055FFAA, 0x00AAFFAA, 0x00FFFFAA,  // 56,
0x0000FFFF, 0x0055FFFF, 0x00AAFFFF, 0x00FFFFFF,  // 60, Yellow,??, ??, White

// Misc ( 16 )  [ Index = 88 ]
0x000080FF,  // SPRING_GREEN
0x008000FF,  // TURQUOSE
0x00FF00FF,  // CYAN
0x00FF0080,  // OCEAN
0x00FF8000,  // VIOLET
0x0080FF00,  // RASPBERRY
0x000000FF,  // GREEN
0x00202020,  // DIM WHITE
0x00200000,  // DIM BLUE
0x10000000,  // INVISIBLE
0x0000FF00,  // Fire_Dark
0x0000FF30,  // Fire_Light  // 00FF80
0xFFFFFFFF,  // Full White
0xFF000000,  // LED_WHITE
0xFF808080,  // WHITE_RGB50
0xFF404040,  // WHITE_RGB25

// Temperture Color Blue to Red (16) [ Index = 104 ]
0x00FF0000, 0x00F01000, 0x00E02000, 0x00D03000,
0x00C04000, 0x00B05000, 0x00A06000, 0x00907000,
0x00808000, 0x00709000, 0x0060A000, 0x0050B000,
0x0040C000, 0x0030D000, 0x0020E000, 0x0000FF00
};


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
void ws2812_MixColorRGB(uint32_t stringNumber, uint32_t row, uint32_t numLEDs);
void ws2812_FadeInFadeOutRGB(uint8_t stringNumber, uint32_t row, uint32_t column, uint8_t red, uint8_t blue, uint8_t green, bool fadeIn, bool fadeOut);
void ws2812_FadeInFadeOutColor(uint8_t stringNumber,  uint32_t row, uint32_t column, uint32_t color, bool fadeIn, bool fadeOut);
void ws2812_StrobeRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
void ws2812_StrobeColor(uint8_t stringNumber, uint32_t colorRequested, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
void ws2812_NewKITT(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_CenterToOutside(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_OutsideToCenter(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_LeftToRight(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_RightToLeft(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);

void ws2812_HalloweenEyesRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeWidth, uint32_t EyeSpace,
		bool Fade, uint32_t Steps, uint32_t FadeDelay, uint32_t EndPause);
void ws2812_TwinkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne);
void ws2812_TwinkleRandom(uint8_t stringNumber, uint32_t numLEDs, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne) ;
void ws2812_SparkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay);
void ws2812_SnowSparkle(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SparkleDelay, uint32_t SpeedDelay);
void ws2812_RunningLights(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t WaveDelay);
void ws2812_ColorWipe(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay);
void ws2812_Rainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t startColor, uint32_t SpeedDelay);
void ws2812_RainbowCycle(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay);
void ws2812_TheaterChase(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay);
void ws2812_TheaterChaseRainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay);
void ws2812_Fire(uint8_t stringNumber, uint32_t numLEDs, uint32_t Cooling, uint32_t Sparking, uint32_t SpeedDelay);
void setPixelHeatColor (uint8_t stringNumber, uint32_t Pixel, uint8_t temperature);
void ws2812_BouncingBalls(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t BallCount);
void ws2812_BouncingColoredBalls(uint8_t stringNumber, uint32_t numLEDs, uint32_t BallCount, uint8_t colors[][3]);

void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color);
uint32_t ws2812_getColor( uint32_t color);
uint32_t ws2812_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct);
uint32_t ws2812_ColorInc(uint32_t incValue);
static uint32_t ws2812_GetRandom(uint32_t min, uint32_t max);

/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: ws2812_DisplayClear
********************************************************************************
* Summary:
*   returns a random number between min and max
*
* Parameters:
*  void
*
* Return:
*  random number
*
*******************************************************************************/
static uint32_t ws2812_GetRandom(uint32_t max, uint32_t min)
{
	/* Declare the randomNumber and set to 0 */
	uint32_t randomNumber = 0;

	//TODO: Need to get a seed from an ADC or something, so there is no overrun if system is running longer than uint32
	/* Seed the random number generator with a semi-random number */
	srand(xTaskGetTickCount());

	/* Retrieve the random number between the min and max values provided */
	randomNumber = (random() % (max - min + 1)) + min;

	/* Return the value */
	return randomNumber;
}

/*******************************************************************************
* Function Name: ws2812_ColorInc
********************************************************************************
* Summary:
*  Increment color through the color lookup table.
*
* Parameters:
*  uint32 incValue: Increment through color table by incValue.
*
* Return: Color at next location.
*
*
*******************************************************************************/
uint32_t ws2812_ColorInc(uint32_t incValue)
{
	/* Declare the variables used */
	uint32_t color;
	extern const uint32_t ws2812_CLUT[];
	/* Holds the index value between calls */
	static uint32_t colorIndex = 0;

	/* Increase the index value with the offset provided */
	colorIndex += incValue;
	colorIndex = colorIndex % ws2812_CWHEEL_SIZE;

	/* Retrieve and return the color code from the LUT */
	color = ws2812_CLUT[ colorIndex ];
    return(color);
}

/*******************************************************************************
* Function Name: ws2812_getColor
********************************************************************************
*
* Summary:
*  Get Pixel Color from the LUT
*
* Parameters:
*  color index
*
* Return:
*  color code
*******************************************************************************/
uint32_t ws2812_getColor( uint32_t color)
{
    color = ws2812_CLUT[color];
    return(color);
}

/*******************************************************************************
* Function Name: ws2812_RgbBlend()
********************************************************************************
* Summary:  Blend two colors into one.
*
*    newColor = (pct * toColor)  + ((100-pct) * fromColor)
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
uint32_t ws2812_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct)
{
    uint32_t newColor = 0;

    newColor |= (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
    newColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
    newColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;

   return(newColor);
}

/*******************************************************************************
* Function Name: ws2812_DisplayClear
********************************************************************************
* Summary:
*   Clear memory with a given value and update the display.
*
* Parameters:
*  uint32 color: Color to clear display.
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color)
{
	/* Clear the LEDs by setting the color selected. Black is for off */
	ws2812HAL_setAllColor(stringNumber, color);
	/* Update the LEDs in the string */
    while(ws2812HAL_updateString(stringNumber) == false);
}

/*******************************************************************************
* Function Name: ws2812_MixColorRGB
********************************************************************************
* Summary:
*  This function changes the LEDs to RGBRGBRGB....RGB
*
* Parameters:
*  ledString: The number of the string to be changed
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_MixColorRGB(uint32_t stringNumber, uint32_t row, uint32_t numLEDs)
{
	/* Cycle through all LEDs and set the RGB pattern to all LEDs in the string passed */
    for(uint32_t i = 0; i < numLEDs; i++)
    {
        switch(i % 3)
        {
            case 0:
            	ws2812HAL_setPixelArrayColor(stringNumber, row, i, ws2812_RED); // red
                break;
            case 1:
            	ws2812HAL_setPixelArrayColor(stringNumber, row, i, ws2812_GREEN); // green
                break;
            case 2:
            	ws2812HAL_setPixelArrayColor(stringNumber, row, i, ws2812_BLUE); // blue
                break;
        }
    }
    while(ws2812HAL_updateString(stringNumber) == false);
}

/*******************************************************************************
* Function Name: ws2812_FadeInFadeOutRGB
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  bool fadeIn: request fade in
*  bool fadeOut: request fade out
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_FadeInFadeOutRGB(uint8_t stringNumber, uint32_t row, uint32_t column, uint8_t red, uint8_t green, uint8_t blue, bool fadeIn, bool fadeOut)
{
	float r, g, b;

	/* If flag to Fade In is set, execute the FadeIn sequence */
	if(fadeIn)
	{
		for(uint32_t k = 0; k < 256; k=k+1)
		{
			r = (k/256.0)*red;
			g = (k/256.0)*green;
			b = (k/256.0)*blue;
			/* Update the LEDs in the string to the new value */
			ws2812HAL_setPixelArrayRGB(stringNumber, row, column, r, g, b);
			/* Update the LEDs in the string */
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
	/* If flag to Fade Out is set, execute the FadeIn sequence */
	if(fadeOut)
	{
		for(uint32_t k = 255; k >= 0; k=k-2)
		{
			r = (k/256.0)*red;
			g = (k/256.0)*green;
			b = (k/256.0)*blue;
			/* Update the LEDs in the string to the new value */
			ws2812HAL_setPixelArrayRGB(stringNumber, row, column, r, g, b);
			/* Update the LEDs in the string */
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
}

/*******************************************************************************
* Function Name: ws2812_FadeInFadeOutColor
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint32 color: Color to clear display.
*  bool fadeIn: request fade in
*  bool fadeOut: request fade out
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_FadeInFadeOutColor(uint8_t stringNumber, uint32_t row, uint32_t column, uint32_t colorRequested, bool fadeIn, bool fadeOut)
{
	typedef union {
			uint8_t uint8_ts[4];
			uint32_t word;
		} WS_colorUnion;

	WS_colorUnion color;
	color.word = colorRequested;
	/* Parse the color code into the three components */
	uint8_t red = color.uint8_ts[1];
	uint8_t blue = color.uint8_ts[2];
	uint8_t green = color.uint8_ts[0];

	/* Pass the parameters to the RGB equivalent function */
	ws2812_FadeInFadeOutRGB(stringNumber, row, column, red, green, blue, fadeIn, fadeOut);
}

/*******************************************************************************
* Function Name: ws2812_StrobeRGB
********************************************************************************
* Summary:
*   Strobes the string with a defined RGB value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  uint32_t StrobeCount: how many flashes to perform
*  uint32_t FlashDelay: the delay between flashes
*  uint32_t EndPause: the end of the cycle delay
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_StrobeRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause)
{
  for(uint32_t j = 0; j < StrobeCount; j++)
  {
	  /* Set the RGB color and update the string */
	  ws2812HAL_setAllRGB(stringNumber, red, green, blue);
	  while(ws2812HAL_updateString(stringNumber) == false);
	  /* Delay for a flash before turning off */
	  vTaskDelay(pdMS_TO_TICKS(FlashDelay));
	  /* Turn off the string and update */
	  ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	  while(ws2812HAL_updateString(stringNumber) == false);
	  /* Delay for a flash before turning back on */
	  vTaskDelay(pdMS_TO_TICKS(FlashDelay));
  }
  /* Delay before exiting */
  vTaskDelay(pdMS_TO_TICKS(EndPause));
}

/*******************************************************************************
* Function Name: ws2812_StrobeColor
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  uint32_t StrobeCount: how many flashes to perform
*  uint32_t FlashDelay: the delay between flashes
*  uint32_t EndPause: the end of the cycle delay
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_StrobeColor(uint8_t stringNumber, uint32_t colorRequested, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause)
{
	typedef union {
		uint8_t uint8_ts[4];
		uint32_t word;
	} WS_colorUnion;

	WS_colorUnion color;
	color.word = colorRequested;
	/* Parse the color code into the three components */
	uint8_t red = color.uint8_ts[1];
	uint8_t blue = color.uint8_ts[2];
	uint8_t green = color.uint8_ts[0];
	/* Pass the parameters to the RGB equivalent function */
	ws2812_StrobeRGB(stringNumber, red, green, blue, StrobeCount, FlashDelay, EndPause);
}

/*******************************************************************************
* Function Name: ws2812_NewKITT
********************************************************************************
* Summary:
*   This function creates the KITT pattern using all the functions below it
*
* Parameters:
*  uint8_t stringNumber: string to control
*  numLEDs the total number of LEDs in the string
*  row: the row on which to display the pattern
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  uint32_t StrobeCount: how many flashes to perform
*  uint32_t FlashDelay: the delay between flashes
*  uint32_t EndPause: the end of the cycle delay
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_NewKITT(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue,
		uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	ws2812_RightToLeft(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_LeftToRight(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_OutsideToCenter(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_CenterToOutside(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_LeftToRight(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_RightToLeft(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_OutsideToCenter(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_CenterToOutside(stringNumber, numLEDs, row, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void ws2812_CenterToOutside(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue,
		uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i =((numLEDs - EyeSize)/2); i >= 0; i--)
	{
		/* Turn off the string and update */
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
		while(ws2812HAL_updateString(stringNumber) == false);

		ws2812HAL_setPixelArrayRGB(stringNumber, row, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, i+j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i+EyeSize+1, red/10, green/10, blue/10);

		ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i-j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i-EyeSize-1, red/10, green/10, blue/10);

		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_OutsideToCenter(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = 0; i<=((numLEDs-EyeSize)/2); i++)
	{
		/* Turn off the string and update */
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
		while(ws2812HAL_updateString(stringNumber) == false);

		ws2812HAL_setPixelArrayRGB(stringNumber, row, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, i+j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i+EyeSize+1, red/10, green/10, blue/10);

		ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i-j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, numLEDs-i-EyeSize-1, red/10, green/10, blue/10);

		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_LeftToRight(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = 0; i < numLEDs-EyeSize-2; i++) {
		/* Turn off the string and update */
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
		while(ws2812HAL_updateString(stringNumber) == false);
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, i+j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i+EyeSize+1, red/10, green/10, blue/10);
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_RightToLeft(uint8_t stringNumber, uint32_t numLEDs, uint8_t row, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = numLEDs-EyeSize-2; i > 0; i--)
	{
		/* Turn off the string and update */
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
		while(ws2812HAL_updateString(stringNumber) == false);
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812HAL_setPixelArrayRGB(stringNumber, row, i+j, red, green, blue);
		}
		ws2812HAL_setPixelArrayRGB(stringNumber, row, i+EyeSize+1, red/10, green/10, blue/10);
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}


/*******************************************************************************
* Function Name: ws2812_HalloweenEyesRGB()
********************************************************************************
* Summary: This function shows two red eyes at a random spot on the strip
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   EyeWidth	How many LEDs per eye
*   EyeSpace	Number of LEDs between the eyes
*   Fade	 	Fade out or not
*   Steps	 	Number of steps on fade out
*   FadeDelay	Delay between each fade out level
*   EndPause	Delay after everything is completed
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_HalloweenEyesRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeWidth,
		uint32_t EyeSpace, bool Fade, uint32_t Steps, uint32_t FadeDelay, uint32_t EndPause)
{

	uint32_t i;
	uint32_t StartPoint  = ws2812_GetRandom(0, (numLEDs - (2 * EyeWidth) - EyeSpace)); //random() % (numLEDs - (2 * EyeWidth) - EyeSpace);
	uint32_t Start2ndEye = StartPoint + EyeWidth + EyeSpace;

	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);

	for(i = 0; i < EyeWidth; i++)
	{
		ws2812HAL_setPixelRGB(stringNumber, StartPoint + i, red, green, blue);
		ws2812HAL_setPixelRGB(stringNumber, Start2ndEye + i, red, green, blue);
	}
	/* Update the LEDs in the string */
	while(ws2812HAL_updateString(stringNumber) == false);

	if(Fade == true)
	{
		float r, g, b;

		for(uint32_t j = Steps; j >= 0; j--)
		{
			r = j*(red/Steps);
			g = j*(green/Steps);
			b = j*(blue/Steps);

			for(i = 0; i < EyeWidth; i++)
			{
				ws2812HAL_setPixelRGB(stringNumber, StartPoint + i, r, g, b);
				ws2812HAL_setPixelRGB(stringNumber, Start2ndEye + i, r, g, b);
			}

			/* Update the LEDs in the string */
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(FadeDelay));
		}
	}

	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);

	vTaskDelay(pdMS_TO_TICKS(EndPause));
}

/*******************************************************************************
* Function Name: ws2812_TwinkleRGB()
********************************************************************************
* Summary: This function will twinkle one or more LEDs in a given color
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   Count		How many LEDs
*   SpeedDelay	Delay between each LED
*   OnlyOne		True to show only one LED at a time
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_TwinkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne)
{
	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	for (uint32_t i = 0; i < Count; i++)
	{
		uint32_t randLedIndex = ws2812_GetRandom((numLEDs - 1), 0); //(random() % (numLEDs - 1));
		ws2812HAL_setPixelRGB(stringNumber, randLedIndex, red, green, blue);
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		if(OnlyOne)
		{
			/* Turn off the string and update */
			ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		}
	}

	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

/*******************************************************************************
* Function Name: ws2812_TwinkleRandom()
********************************************************************************
* Summary: This function will twinkle one or more LEDs in a random color
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   Count		How many LEDs
*   SpeedDelay	Delay between each LED
*   OnlyOne		True to show only one LED at a time
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_TwinkleRandom(uint8_t stringNumber, uint32_t numLEDs, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne)
{
	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	for (uint32_t i = 0; i < Count; i++)
	{
		ws2812HAL_setPixelRGB(stringNumber, ws2812_GetRandom(numLEDs, 0), ws2812_GetRandom(255, 0), ws2812_GetRandom(255, 0), ws2812_GetRandom(255, 0));
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		if(OnlyOne)
		{
			/* Turn off the string and update */
			ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		}
	}

	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

/*******************************************************************************
* Function Name: ws2812_SparkleRGB()
********************************************************************************
* Summary: This function will twinkle one or more LEDs in a random color
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   SpeedDelay	Delay between each LED
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_SparkleRGB(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay)
{
	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	uint32_t Pixel = ws2812_GetRandom(numLEDs, 0);
	ws2812HAL_setPixelRGB(stringNumber, Pixel, red, green, blue);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	/* Turn off the string and update */
	ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

/*******************************************************************************
* Function Name: ws2812_SnowSparkle()
********************************************************************************
* Summary: This function will twinkle one or more LEDs in a random color
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   SparkleDelay Delay for the sparkle to remain
*   SpeedDelay	Delay between each LED
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_SnowSparkle(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SparkleDelay, uint32_t SpeedDelay)
{
	/* Turn the string to the specified color and update */
	ws2812HAL_setAllRGB(stringNumber, red, green, blue);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	uint32_t Pixel = ws2812_GetRandom(numLEDs, 0);
	ws2812HAL_setPixelRGB(stringNumber, Pixel, 0xFF, 0xFF, 0xFF);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SparkleDelay));

	/* Turn the string to the specified color and update */
	ws2812HAL_setAllRGB(stringNumber, red, green, blue);
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

/*******************************************************************************
* Function Name: ws2812_RunningLights()
********************************************************************************
* Summary: This function will makes multiple groups of LEDs chase each other.
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   WaveDelay	Delay for the chase effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_RunningLights(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t WaveDelay)
{
	uint32_t Position = 0;

	for(uint32_t j = 0; j < numLEDs*2; j++)
	{
		Position++; // = 0; //Position + Rate;
		for(uint32_t i = 0; i < numLEDs; i++)
		{
			// sine wave, 3 offset waves make a rainbow!
			//float level = sin(i+Position) * 127 + 128;
			//setPixel(i,level,0,0);
			//float level = sin(i+Position) * 127 + 128;
			ws2812HAL_setPixelRGB(stringNumber, i, ((sin(i+Position) * 127 + 128)/255)*red,
					((sin(i+Position) * 127 + 128)/255)*green,
					((sin(i+Position) * 127 + 128)/255)*blue);
		}
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(WaveDelay));
	}
}

/*******************************************************************************
* Function Name: ws2812_ColorWipe()
********************************************************************************
* Summary: This function will makes multiple groups of LEDs chase each other.
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_ColorWipe(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay)
{
	for(uint32_t i = 0; i < numLEDs; i++)
	{
		/* Turn the selected LED and update */
		ws2812HAL_setPixelRGB(stringNumber, i, red, green, blue);
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
}

/*******************************************************************************
* Function Name: ws2812_Rainbow()
********************************************************************************
* Summary: This function will display a rainbow
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
* 	startColor	index for the LUT start color (0...23)
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_Rainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t startColor, uint32_t SpeedDelay)
{
	uint32_t ledPosition = 0; // LED position when setting color
    uint32_t color = 0;       // Temp color to set LED

    /* Set new start color */
    color = startColor;

    /* Loop through all LEDs giving each one a different color from the color wheel */
    for(ledPosition = 0; ledPosition <= numLEDs; ledPosition++)
    {
    	ws2812HAL_setPixelColor(stringNumber, ledPosition, ws2812_getColor( color ));
    	color++;
    	if(color >= ws2812_CWHEEL_SIZE) color = 0;
    }

    /* Show the LEDs */
    while(ws2812HAL_updateString(stringNumber) == false);
    vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

/*******************************************************************************
* Function Name: ws2812_RainbowCycle()
********************************************************************************
* Summary: This function will display a rainbow cycle
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_RainbowCycle(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay)
{
	/* Cycles of all colors on wheel */
	for(uint16_t j = 0; j < ws2812_CWHEEL_SIZE; j++)
	{
		ws2812_Rainbow(stringNumber, numLEDs, j, SpeedDelay);
	}
}

/*******************************************************************************
* Function Name: ws2812_TheaterChase()
********************************************************************************
* Summary: This function will show the effect LEDs are chasing each other
* like what you’d see in an old Theater
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_TheaterChase(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t SpeedDelay)
{
	//do 10 cycles of chasing
	for (uint32_t j = 0; j < 10; j++)
	{
		for (uint32_t q = 0; q < 3; q++)
		{
			for (uint32_t i = 0; i < numLEDs; i = i+3)
			{
				ws2812HAL_setPixelRGB(stringNumber, i+q, red, green, blue);    //turn every third pixel on
			}
			/* Show the LEDs */
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

			for (uint32_t i = 0; i < numLEDs; i = i+3)
			{
				ws2812HAL_setPixelRGB(stringNumber, i+q, 0, 0, 0);        //turn every third pixel off
			}
		}
	}
}

/*******************************************************************************
* Function Name: ws2812_TheaterChaseRainbow()
********************************************************************************
* Summary: This function will combine Rainbow and TheatreChase.
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_TheaterChaseRainbow(uint8_t stringNumber, uint32_t numLEDs, uint32_t SpeedDelay)
{
	uint32_t color = 0;       // Temp color to set LED
	/* Cycles of all colors on wheel */
	for(uint16_t j = 0; j < ws2812_CWHEEL_SIZE; j++)
	{
		for (uint32_t q = 0; q < 3; q++)
		{
			for (uint32_t i = 0; i < numLEDs; i = i+3)
			{
				ws2812HAL_setPixelColor(stringNumber, i + q, ws2812_getColor( color ));    //turn every third pixel on
				color++;
				if(color >= ws2812_CWHEEL_SIZE) color = 0;
			}
			/* Show the LEDs */
			while(ws2812HAL_updateString(stringNumber) == false);
			vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

			for (uint32_t i = 0; i < numLEDs; i = i+3)
			{
				ws2812HAL_setPixelRGB(stringNumber, i+q, 0, 0, 0);        //turn every third pixel off
			}
		}
	}

}

/*******************************************************************************
* Function Name: ws2812_Fire()
********************************************************************************
* Summary: This function simulates a one LED wide “fire”,
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
* 	Cooling: how fast a flame cools down
* 	Sparking: the chance (out of 255) that a spark will ignite. Higher values
* 	 make the fire more active
*   SpeedDelay	Delay for the effect
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_Fire(uint8_t stringNumber, uint32_t numLEDs, uint32_t Cooling, uint32_t Sparking, uint32_t SpeedDelay)
{
	//TODO: this parameter needs to be a constant..now set at 24...need a better solution
	static uint8_t heat[24];
	uint32_t cooldown;

	/* Step 1.  Cool down every cell a little */
	for(uint32_t i = 0; i < numLEDs; i++)
	{
		cooldown = ws2812_GetRandom(((Cooling * 10) / numLEDs) + 2, 0);

		if(cooldown > heat[i])
		{
			heat[i] = 0;
		}
		else
		{
			heat[i] = heat[i] - cooldown;
		}
	}

	/* Step 2.  Heat from each cell drifts 'up' and diffuses a little */
	for(uint32_t k = numLEDs - 1; k >= 2; k--)
	{
		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
	}

	/* Step 3.  Randomly ignite new 'sparks' near the bottom */
	if( ws2812_GetRandom(255, 0) < Sparking )
	{
		uint32_t y = ws2812_GetRandom(7, 0);
		heat[y] = heat[y] + ws2812_GetRandom(255, 160);

	}

	/* Step 4.  Convert heat to LED colors */
	for(uint32_t j = 0; j < numLEDs; j++)
	{
		setPixelHeatColor(stringNumber, j, heat[j] );
	}

	/* Show the LEDs */
	while(ws2812HAL_updateString(stringNumber) == false);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}

void setPixelHeatColor (uint8_t stringNumber, uint32_t Pixel, uint8_t temperature)
{
	/* Scale 'heat' down from 0-255 to 0-191 */
	uint8_t t192 = round((temperature/255.0)*191);

	/* calculate ramp up from */
	uint8_t heatramp = t192 & 0x3F; /* 0..63 */
	heatramp <<= 2; /* scale up to 0..252 */

	/* figure out which third of the spectrum we're in: */

	if( t192 > 0x80)
	{
		/* hottest */
		ws2812HAL_setPixelRGB(stringNumber, Pixel, 255, 255, heatramp);
	}
	else if( t192 > 0x40 )
	{
		/* middle */
		ws2812HAL_setPixelRGB(stringNumber, Pixel, 255, heatramp, 0);
	}
	else
	{
		/* coolest */
		ws2812HAL_setPixelRGB(stringNumber, Pixel, heatramp, 0, 0);
	}
}

/*******************************************************************************
* Function Name: ws2812_BouncingBalls()
********************************************************************************
* Summary: This function simulates a number of bouncing balls,
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   BallCount	number of balls to see
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_BouncingBalls(uint8_t stringNumber, uint32_t numLEDs, uint8_t red, uint8_t green, uint8_t blue, uint32_t BallCount)
{

	float Gravity = -9.81;
	uint32_t StartHeight = 1;

	float Height[BallCount];
	float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
	float ImpactVelocity[BallCount];
	float TimeSinceLastBounce[BallCount];
	uint32_t   Position[BallCount];
	long  ClockTimeSinceLastBounce[BallCount];
	float Dampening[BallCount];

	for (uint32_t i = 0 ; i < BallCount ; i++)
	{
		ClockTimeSinceLastBounce[i] = xTaskGetTickCount();
		Height[i] = StartHeight;
		Position[i] = 0;
		ImpactVelocity[i] = ImpactVelocityStart;
		TimeSinceLastBounce[i] = 0;
		Dampening[i] = 0.90 - (float)i / pow(BallCount, 2.0);
	}

	//TODO: Need to create a global variable to break this
	while (true)
	{
		for (int i = 0 ; i < BallCount ; i++)
		{
			TimeSinceLastBounce[i] =  xTaskGetTickCount() - ClockTimeSinceLastBounce[i];
			Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

			if ( Height[i] < 0 ) {
				Height[i] = 0;
				ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
				ClockTimeSinceLastBounce[i] = xTaskGetTickCount();

				if ( ImpactVelocity[i] < 0.01 ) {
					ImpactVelocity[i] = ImpactVelocityStart;
				}
			}
			Position[i] = round( Height[i] * (numLEDs - 1) / StartHeight);
		}

		for (int i = 0 ; i < BallCount ; i++) {
			ws2812HAL_setPixelRGB(stringNumber, Position[i],red,green,blue);
		}

		/* Show the LEDs */
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(10));
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);

	}
}

/*******************************************************************************
* Function Name: ws2812_BouncingBalls()
********************************************************************************
* Summary: This function simulates a number of bouncing balls,
*
*
* Parameters:
* 	stringNumber the string to address
* 	numLEDs		the total number of LEDs in the string
*   BallCount	number of balls to see
*   color: 2 dimensional array [balls][color RGB]
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_BouncingColoredBalls(uint8_t stringNumber, uint32_t numLEDs, uint32_t BallCount, uint8_t colors[][3])
{
	float Gravity = -9.81;
	int StartHeight = 1;

	float Height[BallCount];
	float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
	float ImpactVelocity[BallCount];
	float TimeSinceLastBounce[BallCount];
	int   Position[BallCount];
	long  ClockTimeSinceLastBounce[BallCount];
	float Dampening[BallCount];

	for (uint32_t i = 0 ; i < BallCount ; i++)
	{
		ClockTimeSinceLastBounce[i] = xTaskGetTickCount();
		Height[i] = StartHeight;
		Position[i] = 0;
		ImpactVelocity[i] = ImpactVelocityStart;
		TimeSinceLastBounce[i] = 0;
		Dampening[i] = 0.90 - (float)i/pow(BallCount,2);
	}

	//TODO: Need to create a global variable to break this
	while (true)
	{
		for (uint32_t i = 0 ; i < BallCount ; i++)
		{
			TimeSinceLastBounce[i] =  xTaskGetTickCount() - ClockTimeSinceLastBounce[i];
			Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

			if ( Height[i] < 0 )
			{
				Height[i] = 0;
				ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
				ClockTimeSinceLastBounce[i] = xTaskGetTickCount();

				if ( ImpactVelocity[i] < 0.01 )
				{
					ImpactVelocity[i] = ImpactVelocityStart;
				}
			}
			Position[i] = round( Height[i] * (numLEDs - 1) / StartHeight);
		}

		for (uint32_t i = 0 ; i < BallCount ; i++)
		{
			ws2812HAL_setPixelRGB(stringNumber, Position[i], colors[i][0], colors[i][1], colors[i][2]);
		}

		/* Show the LEDs */
		while(ws2812HAL_updateString(stringNumber) == false);
		vTaskDelay(pdMS_TO_TICKS(10));
		ws2812HAL_setAllColor(stringNumber, ws2812_BLACK);
	}
}
