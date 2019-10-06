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
    	ws2812HAL_setPixelColor(stringNumber, ledPosition, ws2812HAL_getColor( color ));
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
				ws2812HAL_setPixelColor(stringNumber, i + q, ws2812HAL_getColor( color ));    //turn every third pixel on
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
