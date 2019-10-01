/** __Code for the WS2812 Example Application_
**
** @author Copyright (C) June 15, 2019 __Alan Hawse__
** @version __1.0__   __Gen 1__</replaceable>
** @modified ____7/25/2019__Hassane El-Khoury__  __description of edit__</replaceable>
** @@
**
********************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>
#include <stdlib.h>
#include "ws2812HAL.h"
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
/* Declare the array to hold the number of LEDs per string */
uint8_t numOfLeds[MAX_LED_STRINGS] = {0};

uint8_t channelStatus[MAX_LED_STRINGS] = {0};

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */


/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: void ws2812LightShowTask (void *pvParameters)
********************************************************************************
* Summary:
*  Task that processes the main application for the light show
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  void
*
*******************************************************************************/
void ws2812LightShowTask(void *arg)
{
	(void)arg;
	printf("Starting LightShow task\r\n");
	vTaskDelay(pdMS_TO_TICKS(1000));

	int8_t retChannelStatus = -1;

	retChannelStatus = ws2812HAL_CreateString(GPIO_PRT9, 0, 24, 1, 24, ws2812_LED_LAYOUT_STANDARD);
	if(retChannelStatus == -1)
	{
		printf("Third channel Not Created\r\n");
	}
	else
	{
		channelStatus[retChannelStatus] = 1;
		printf("Channel %d Created\r\n", retChannelStatus);
	}

	/* If channel is enabled, get the number of LEDs for the channel */
	for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
	{
		if(channelStatus[i] == 1)
		{
			numOfLeds[i] = ws2812HAL_getNumLeds(i);
		}
	}

	/* Test the Display Clear */
//	ws2812_DisplayClear(0, ws2812_ORANGE);
//	vTaskDelay(pdMS_TO_TICKS(500));
//	/* Test the RGB Mix */
//	ws2812_MixColorRGB(0, numOfLeds[0]);
//	vTaskDelay(pdMS_TO_TICKS(500));
//	/* Test the FadeInFadeOutRGB */
//	ws2812_FadeInFadeOutRGB(0, 0xFF, 0xF0, 0xF0, 1, 1);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the FadeInFadeOutColor */
//	ws2812_FadeInFadeOutColor(0, ws2812_GREEN, 1, 1);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_StrobeRGB */
//	ws2812_StrobeRGB(0, 0xFF, 0xFF, 0xFF, 20, 50, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_StrobeColor */
//	ws2812_StrobeColor(0, ws2812_FIRE_LIGHT, 20, 50, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_NewKITT */
//	ws2812_NewKITT(0, numOfLeds[0], 0xFF, 0x00, 0x00, 4, 100, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
	/* Test the ws2812_HalloweenEyesRGB */
	ws2812_HalloweenEyesRGB(0, numOfLeds[0], 0xFF, 0x00, 0x00, 1, 3, true, 50, 60, 1000);
	vTaskDelay(pdMS_TO_TICKS(1000));
	/* Test the ws2812_TwinkleRGB */
//	ws2812_TwinkleRGB(0, numOfLeds[0], 0xFF, 0xFF, 0xFF, 10, 100, true);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_TwinkleRGB */
//	ws2812_TwinkleRGB(0, numOfLeds[0], 0xFF, 0xFF, 0xFF, 10, 100, false);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the Display Clear */
//	ws2812_DisplayClear(0, ws2812_BLACK);
//	vTaskDelay(pdMS_TO_TICKS(500));

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
