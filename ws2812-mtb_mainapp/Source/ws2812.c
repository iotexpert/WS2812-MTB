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

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ws2812.h"
#include "ws2812Hal.h"
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
/* Define delay to wait for Queue to be free */
#define WS2812_QUEUE_DELAY_MS	(pdMS_TO_TICKS(2))
/* The length of the queue used to hold the WS2812 messages */
#define WS2812_QUEUE_SIZE       	20
/* The Auto-Update Timer */
#define WS2812_AUTOUPDATE_TIMER_MS	pdMS_TO_TICKS(30)
/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */

/* Declare queue for the WS2812 Task */
QueueHandle_t ws2812QueueHandle = NULL;

/* Declare the LED refresh timer handle */
TimerHandle_t ws2812TimerHandle = NULL;

/* Declare the LED Auto-Update flag and set the default to true */
bool wsAutoUpdateState = false;

uint8_t channelStatus[MAX_LED_STRINGS] = {0};

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
void ws2812_update(uint8_t stringNumber);
void ws2812_autoUpdate(bool option);

void ws2812_setPixelRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setPixelColor(uint8_t stringNumber, uint32_t led, uint32_t colorRequested);
void ws2812_setAllRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setAllColor(uint8_t stringNumber, uint32_t colorRequested);

void ws2812CallbackFunction( TimerHandle_t xTimer );
/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */

/*******************************************************************************
* Function Name: ws2812Task
********************************************************************************
* Summary:
*  Function This is the main RGB controller task
*
* Parameters:
*  arg
*
* Return:
*  void
*
*******************************************************************************/
void ws2812Task(void *arg)
{
	(void)arg;

	/* Declare message struct. */
	ws2812_msg_t msg;

	printf("Starting WS2812 task\r\n");

	int8_t retChannelStatus = -1;
	/* Create and check if channel was properly created */
//	retChannelStatus = ws2812HAL_CreateString(GPIO_PRT8, 0, 1, 3, 1, ws2812_LED_LAYOUT_STANDARD);
//	if(retChannelStatus == -1)
//	{
//		printf("First channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
//	retChannelStatus = ws2812HAL_CreateString(GPIO_PRT10, 0, 3, 3, 1, ws2812_LED_LAYOUT_STANDARD);
//	if(retChannelStatus == -1)
//	{
//		printf("Second channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
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
//	retChannelStatus = ws2812HAL_CreateString(GPIO_PRT6, 0, 3, 3, 1, ws2812_LED_LAYOUT_STANDARD);
//	if(retChannelStatus == -1)
//	{
//		printf("Fourth channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
	printf("All channels configured\r\n");

	/* This queue handles messages sent to the queue to execute */
	ws2812QueueHandle = xQueueCreate(WS2812_QUEUE_SIZE, sizeof(ws2812_msg_t));

	/* This timer calls the update function every 30ms if it is turned on. */
	ws2812TimerHandle = xTimerCreate(	/* A text name, purely to help debugging. */
									"ws2812 timer",
									/* The timer period, in ms */
									WS2812_AUTOUPDATE_TIMER_MS,
									/* This is a periodic timer, so xAutoReload is set to pdTRUE. */
									pdTRUE,
									/* The ID is not used, so can be set to anything. */
									( void * ) 0,
									/* The callback function */
									ws2812CallbackFunction
							);
    while(1)
    {
    	/* Block task until a message is received in the Queue */
    	xQueueReceive(ws2812QueueHandle, &msg, portMAX_DELAY);
    	/* Switch Statement to execute any message in the Queue */
    	switch(msg.cmd)
    	{
			case ws2812_cmd_update:
				if(!wsAutoUpdateState)
				{
					ws2812HAL_updateString(msg.stringNumber);
				}
				break;
			case ws2812_cmd_autoUpdate:
				if(wsAutoUpdateState && msg.data == false)
				{
					/* Stop the refresh timer */
					xTimerStop(ws2812TimerHandle, 0);
				}
				else if(!wsAutoUpdateState && msg.data == true)
				{
					/* Start the refresh timer */
					xTimerStart(ws2812TimerHandle, 0);
					/* Reset the refresh timer to start from 0 */
					xTimerReset(ws2812TimerHandle, 0);
				}
				/* Load the last state of the Auto-Update flag */
				wsAutoUpdateState = msg.data;
				break;
			case ws2812_cmd_setPixelRGB:
				ws2812HAL_setPixelRGB(msg.stringNumber, msg.data, msg.red, msg.green, msg.blue);
				break;
			case ws2812_cmd_setAllRGB:
				ws2812HAL_setAllRGB(msg.stringNumber, msg.red, msg.green, msg.blue);
				break;
			default:
				break;
    	}
    }
}

/*******************************************************************************
* Function Name: ws2812_update
********************************************************************************
* Summary:
*  Helper function to transmit the data to control the LEDs of the passed
*  stringNumber
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_update(uint8_t stringNumber)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_update;
	msg.stringNumber = stringNumber;

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}

}

/*******************************************************************************
* Function Name: ws2812_autoUpdate
********************************************************************************
* Summary:
*  Helper function to transmit the data to turn on the Auto-Update
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  option: 	true = Auto-Update ON
*  			false = Auto-Update OFF
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_autoUpdate(bool option)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_autoUpdate;
	msg.data = option;

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}
}

/*******************************************************************************
* Function Name: ws2812_setPixelRGB
********************************************************************************
* Summary:
*  Helper function to control a single LED
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*  led: the number of the LED to address
*  red: Hex code for RED
*  green: Hex code for GREEN
*  blue: HEX code for BLUE
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_setPixelRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setPixelRGB;
	msg.stringNumber = stringNumber;
	msg.red = red;
	msg.blue = blue;
	msg.green = green;
	msg.data = led;

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}
}

/*******************************************************************************
* Function Name: ws2812_setAllRGB
********************************************************************************
* Summary:
*  Helper function to control a single LED
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*  led: the number of the LED to address
*  red: Hex code for RED
*  green: Hex code for GREEN
*  blue: HEX code for BLUE
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_setAllRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setAllRGB;
	msg.stringNumber = stringNumber;
	msg.red = red;
	msg.blue = blue;
	msg.green = green;

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}
}

/*******************************************************************************
* Function Name: ws2812_setPixelColor
********************************************************************************
* Summary:
*  Helper function to control a single LED
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*  led: the number of the LED to address
*  color: the RGB of the requested color
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_setPixelColor(uint8_t stringNumber, uint32_t led, uint32_t colorRequested)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	typedef union {
		uint8_t bytes[4];
		uint32_t word;
	} WS_colorUnion;

	WS_colorUnion color;

	color.word = colorRequested;
	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setPixelRGB;
	msg.stringNumber = stringNumber;
	msg.red = color.bytes[1];
	msg.blue = color.bytes[2];
	msg.green = color.bytes[0];
	msg.data = led;

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}
}

/*******************************************************************************
* Function Name: ws2812_setAllColor
********************************************************************************
* Summary:
*  Helper function to control all LEDs
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*  color: the RGB of the requested color
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_setAllColor(uint8_t stringNumber, uint32_t colorRequested)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	typedef union {
		uint8_t bytes[4];
		uint32_t word;
	} WS_colorUnion;

	WS_colorUnion color;

	color.word = colorRequested;
	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setAllRGB;
	msg.stringNumber = stringNumber;
	msg.red = color.bytes[1];
	msg.blue = color.bytes[2];
	msg.green = color.bytes[0];

	/* Transmit the data to the queue safely */
	if(ws2812QueueHandle != NULL)
	{
		/* Send to Queue and wait forever */
		if( xQueueSend( ws2812QueueHandle, ( void * ) &msg, portMAX_DELAY  ) != pdPASS)
		{
			printf("ws2812QueueHandle Full even after waiting\r\n");
		}
	}
	else
	{
		printf("ws2812QueueHandle does not exist\r\n");
	}
}

/*******************************************************************************
* Function Name: ws2812CallbackFunction
********************************************************************************
* Summary:
*  Function This function is called by the software timer which is used to
*  auto-update the LEDs. It checks to make sure that the DMA is done...
*  if not it doesn't do anything
*
* Parameters:
*  xTimer: Timer callback handle
*
* Return:
*  void
*
*******************************************************************************/
void ws2812CallbackFunction( TimerHandle_t xTimer )
{
	/* Check all created channels */
	for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
	{
		/* If channel is enabled, trigger the DMA */
		if(channelStatus[i] == 1)
		{
			ws2812HAL_updateString(i);
		}
	}
}
