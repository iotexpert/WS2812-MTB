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

void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_initMixColorRGB(uint8_t stringNumber);

void ws2812CallbackFunction( TimerHandle_t xTimer );
/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */

/*******************************************************************************
* Function Name: configure_LedChannels
********************************************************************************
* Summary:
*  Function This function sets up the channels...and the resources required to
*  make the channel work
*
* Parameters:
*  stringNumbner: The string number to address
*
* Return:
*  void
*
*******************************************************************************/
//void configure_LedChannels(void)

//			/* Allocate the size of x, y array for the rows and columns of the string used */
//			#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
//			channelConfig[i].ledArray = malloc(channelConfig[i].rows * sizeof(uint32_t));
//			if (channelConfig[i].ledArray == NULL)
//			{
//				printf("Error creating ledArray buffer %d\r\n", i);
//			}
//			else
//			{
//				memset(channelConfig[i].ledArray, 0, (channelConfig[i].rows * sizeof(uint32_t)));
//				printf("ledArray Length is %lu\r\n", (channelConfig[i].rows * sizeof(uint32_t)));
//			}
//			#else
//			channelConfig[i].ledArray = malloc(channelConfig[i].rows * sizeof(uint8_t));
//			memset(channelConfig[i].ledArray, 0, (channelConfig[i].rows * sizeof(uint8_t)));
//			#endif
//			for (uint32_t r = 0; r < channelConfig[i].arrayRows; r++)
//			{
//				#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
//				channelConfig[i].ledArray[r] = malloc(channelConfig[i].arrayColumns * sizeof(uint32_t));
//				memset(channelConfig[i].ledArray[r], 0, (channelConfig[i].arrayColumns * sizeof(uint32_t)));
//				#else
//				channelConfig[i].ledArray[r] = malloc(channelConfig[i].arrayColumns * sizeof(uint8_t));
//				memset(channelConfig[i].ledArray[r], 0, (channelConfig[i].arrayColumns * sizeof(uint8_t)));
//				#endif
//			}
//			if (!channelConfig[i].ledArray) printf("Error creating LED Array %d\r\n", i);

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
//	retChannelStatus = WS_CreateString(GPIO_PRT8, 0, 3);
//	if(retChannelStatus == -1)
//	{
//		printf("First channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
//	retChannelStatus = WS_CreateString(GPIO_PRT10, 0, 3);
//	if(retChannelStatus == -1)
//	{
//		printf("Second channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
//	retChannelStatus = WS_CreateString(GPIO_PRT9, 0, 3);
//	if(retChannelStatus == -1)
//	{
//		printf("Third channel Not Created\r\n");
//	}
//	else
//	{
//		channelStatus[retChannelStatus] = 1;
//		printf("Channel %d Created\r\n", retChannelStatus);
//	}
	retChannelStatus = WS_CreateString(GPIO_PRT6, 0, 3);
	if(retChannelStatus == -1)
	{
		printf("Fourth channel Not Created\r\n");
	}
	else
	{
		channelStatus[retChannelStatus] = 1;
		printf("Channel %d Created\r\n", retChannelStatus);
	}
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
					WS_updateString(msg.stringNumber);
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
			case ws2812_cmd_setRGB:
				WS_setRGB(msg.stringNumber, msg.data, msg.red, msg.green ,msg.blue);
				break;
			case ws2812_cmd_setRange:
				WS_setRange(msg.stringNumber, msg.data>>16 & 0xFFFF, msg.data&0xFFFF, msg.red,msg.green ,msg.blue);
				break;
			case ws2812_cmd_initMixColorRGB:
				WS_initMixColorRGB(msg.stringNumber);
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
* Function Name: ws2812_setRGB
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
void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setRGB;
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
* Function Name: ws2812_setRange
********************************************************************************
* Summary:
*  Helper function to transmit the data to control the LEDs in the range passed
*  The function will configure the message to be sent to the Queue for the LED
*  controlling task
*
* Parameters:
*  stringNumbner: The string number to address
*  start: the index of the first LED to address
*  end: the index of the last LED to address
*  red: Hex code for RED
*  green: Hex code for GREEN
*  blue: HEX code for BLUE
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_setRange;
	msg.stringNumber = stringNumber;
	msg.red = red;
	msg.blue = blue;
	msg.green = green;
	msg.data = start << 16 | end;

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
* Function Name: ws2812_initMixColorRGB
********************************************************************************
* Summary:
*  Helper function to alternate the color of LEDs in a string passed
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
void ws2812_initMixColorRGB(uint8_t stringNumber)
{
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Configure command Parameters */
	msg.cmd = ws2812_cmd_initMixColorRGB;
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
			WS_updateString(i);
		}
	}
}
