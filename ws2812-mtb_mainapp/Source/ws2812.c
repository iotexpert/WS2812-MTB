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

/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */
#define WS_ZOFFSET (1)
#define WS_ONE3  (0b110<<24)
#define WS_ZERO3 (0b100<<24)
#define WS_SPI_BIT_PER_BIT (3)
#define WS_COLOR_PER_PIXEL (3)
#define WS_BYTES_PER_PIXEL (WS_SPI_BIT_PER_BIT * WS_COLOR_PER_PIXEL)

#define WS1_NUM_DESCRIPTORS (sizeof(WS1_frameBuffer) / 256 + 1)
#define WS2_NUM_DESCRIPTORS (sizeof(WS2_frameBuffer) / 256 + 1)
#define WS3_NUM_DESCRIPTORS (sizeof(WS3_frameBuffer) / 256 + 1)
#define WS4_NUM_DESCRIPTORS (sizeof(WS4_frameBuffer) / 256 + 1)
#define WS5_NUM_DESCRIPTORS (sizeof(WS5_frameBuffer) / 256 + 1)

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
static uint8_t WS1_frameBuffer[ws2812_NUM_PIXELS_WS1 * WS_BYTES_PER_PIXEL + WS_ZOFFSET];
static uint8_t WS2_frameBuffer[ws2812_NUM_PIXELS_WS2 * WS_BYTES_PER_PIXEL + WS_ZOFFSET];
static uint8_t WS3_frameBuffer[ws2812_NUM_PIXELS_WS3 * WS_BYTES_PER_PIXEL + WS_ZOFFSET];
static uint8_t WS4_frameBuffer[ws2812_NUM_PIXELS_WS4 * WS_BYTES_PER_PIXEL + WS_ZOFFSET];
static uint8_t WS5_frameBuffer[ws2812_NUM_PIXELS_WS5 * WS_BYTES_PER_PIXEL + WS_ZOFFSET];

static cy_stc_dma_descriptor_t WS1Descriptors[WS1_NUM_DESCRIPTORS];
static cy_stc_dma_descriptor_t WS2Descriptors[WS2_NUM_DESCRIPTORS];
static cy_stc_dma_descriptor_t WS3Descriptors[WS3_NUM_DESCRIPTORS];
static cy_stc_dma_descriptor_t WS4Descriptors[WS4_NUM_DESCRIPTORS];
static cy_stc_dma_descriptor_t WS5Descriptors[WS5_NUM_DESCRIPTORS];

/* Declare queue for the WS2812 Task */
QueueHandle_t ws2812QueueHandle = NULL;

/* Declare the LED refresh timer handle */
TimerHandle_t ws2812TimerHandle = NULL;

/* Declare the LED Auto-Update flag and set the default to true */
bool wsAutoUpdateState = false;

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
* Function Name: WSx_DMAConfigure
********************************************************************************
* Summary:
*  Function WS_DMAConfiguration.
*  This function sets up the DMA and the descriptors
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void WS1_DMAConfigure(void)
{
    /* I copies this structure from the PSoC Creator Component configuration
    	in generated source */
    const cy_stc_dma_descriptor_config_t WS1_DMA_Descriptors_config =
    {
    .retrigger       = CY_DMA_RETRIG_IM,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_ENABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_BYTE,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_1D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 0L,
    .dstYincrement   = 0L,
    .yCount          = 1UL,
    .nextDescriptor  = 0
    };

    for(uint32_t i = 0; i < WS1_NUM_DESCRIPTORS; i++)
    {
        Cy_DMA_Descriptor_Init(&WS1Descriptors[i], &WS1_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WS1Descriptors[i], (uint8_t *)&WS1_frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&WS1Descriptors[i], (void *)&WS1_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WS1Descriptors[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WS1Descriptors[i], &WS1Descriptors[i + 1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WS1Descriptors[WS1_NUM_DESCRIPTORS - 1], sizeof(WS1_frameBuffer) - 256 * (WS1_NUM_DESCRIPTORS - 1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WS1Descriptors[WS1_NUM_DESCRIPTORS - 1], 0);
    Cy_DMA_Descriptor_SetChannelState(&WS1Descriptors[WS1_NUM_DESCRIPTORS - 1], CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS1_DMA_HW);
}

static void WS2_DMAConfigure(void)
{
	/* I copies this structure from the PSoC Creator Component configuration
	    	in generated source */
    const cy_stc_dma_descriptor_config_t WS2_DMA_Descriptors_config =
    {
    .retrigger       = CY_DMA_RETRIG_IM,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_ENABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_BYTE,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_1D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 0L,
    .dstYincrement   = 0L,
    .yCount          = 1UL,
    .nextDescriptor  = 0
    };

    for(uint32_t i = 0; i < WS2_NUM_DESCRIPTORS; i++)
    {
        Cy_DMA_Descriptor_Init(&WS2Descriptors[i], &WS2_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WS2Descriptors[i], (uint8_t *)&WS2_frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&WS2Descriptors[i], (void *)&WS2_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WS2Descriptors[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WS2Descriptors[i], &WS2Descriptors[i + 1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WS2Descriptors[WS2_NUM_DESCRIPTORS - 1], sizeof(WS2_frameBuffer) - 256 * (WS2_NUM_DESCRIPTORS - 1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WS2Descriptors[WS2_NUM_DESCRIPTORS - 1], 0);
    Cy_DMA_Descriptor_SetChannelState(&WS2Descriptors[WS2_NUM_DESCRIPTORS - 1], CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS2_DMA_HW);
}

static void WS3_DMAConfigure(void)
{
	/* I copies this structure from the PSoC Creator Component configuration
	    	in generated source */
    const cy_stc_dma_descriptor_config_t WS3_DMA_Descriptors_config =
    {
    .retrigger       = CY_DMA_RETRIG_IM,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_ENABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_BYTE,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_1D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 0L,
    .dstYincrement   = 0L,
    .yCount          = 1UL,
    .nextDescriptor  = 0
    };

    for(uint32_t i = 0; i < WS3_NUM_DESCRIPTORS; i++)
    {
        Cy_DMA_Descriptor_Init(&WS3Descriptors[i], &WS3_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WS3Descriptors[i], (uint8_t *)&WS3_frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&WS3Descriptors[i], (void *)&WS3_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WS3Descriptors[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WS3Descriptors[i], &WS3Descriptors[i + 1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WS3Descriptors[WS3_NUM_DESCRIPTORS - 1], sizeof(WS3_frameBuffer) - 256 * (WS3_NUM_DESCRIPTORS - 1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WS3Descriptors[WS3_NUM_DESCRIPTORS - 1], 0);
    Cy_DMA_Descriptor_SetChannelState(&WS3Descriptors[WS3_NUM_DESCRIPTORS - 1], CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS3_DMA_HW);
}

static void WS4_DMAConfigure(void)
{
	/* I copies this structure from the PSoC Creator Component configuration
	    	in generated source */
    const cy_stc_dma_descriptor_config_t WS4_DMA_Descriptors_config =
    {
    .retrigger       = CY_DMA_RETRIG_IM,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_ENABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_BYTE,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_1D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 0L,
    .dstYincrement   = 0L,
    .yCount          = 1UL,
    .nextDescriptor  = 0
    };

    for(uint32_t i = 0; i < WS4_NUM_DESCRIPTORS; i++)
    {
        Cy_DMA_Descriptor_Init(&WS4Descriptors[i], &WS4_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WS4Descriptors[i], (uint8_t *)&WS4_frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&WS4Descriptors[i], (void *)&WS4_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WS4Descriptors[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WS4Descriptors[i], &WS4Descriptors[i + 1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WS4Descriptors[WS4_NUM_DESCRIPTORS - 1], sizeof(WS4_frameBuffer) - 256 * (WS4_NUM_DESCRIPTORS - 1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WS4Descriptors[WS4_NUM_DESCRIPTORS - 1], 0);
    Cy_DMA_Descriptor_SetChannelState(&WS4Descriptors[WS4_NUM_DESCRIPTORS - 1], CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS4_DMA_HW);
}

static void WS5_DMAConfigure(void)
{
	/* I copies this structure from the PSoC Creator Component configuration
	    	in generated source */
    const cy_stc_dma_descriptor_config_t WS5_DMA_Descriptors_config =
    {
    .retrigger       = CY_DMA_RETRIG_IM,
    .interruptType   = CY_DMA_DESCR_CHAIN,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_ENABLED,
    .triggerInType   = CY_DMA_1ELEMENT,
    .dataSize        = CY_DMA_BYTE,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_WORD,
    .descriptorType  = CY_DMA_1D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 1L,
    .dstXincrement   = 0L,
    .xCount          = 256UL,
    .srcYincrement   = 0L,
    .dstYincrement   = 0L,
    .yCount          = 1UL,
    .nextDescriptor  = 0
    };

    for(uint32_t i = 0; i < WS5_NUM_DESCRIPTORS; i++)
    {
        Cy_DMA_Descriptor_Init(&WS5Descriptors[i], &WS5_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WS5Descriptors[i], (uint8_t *)&WS5_frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&WS5Descriptors[i], (void *)&WS5_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WS5Descriptors[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WS5Descriptors[i], &WS5Descriptors[i + 1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WS5Descriptors[WS5_NUM_DESCRIPTORS - 1], sizeof(WS5_frameBuffer) - 256 * (WS5_NUM_DESCRIPTORS - 1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WS5Descriptors[WS5_NUM_DESCRIPTORS - 1], 0);
    Cy_DMA_Descriptor_SetChannelState(&WS5Descriptors[WS5_NUM_DESCRIPTORS - 1], CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS5_DMA_HW);
}

/*******************************************************************************
* Function Name: WS_DMATrigger
********************************************************************************
* Summary:
*  Function This function sets up the channel...
*  then enables it to dump the frameBuffer to pixels
*
* Parameters:
*  stringNumbner: The string number to address
*
* Return:
*  void
*
*******************************************************************************/
void WS_DMATrigger(uint8_t stringNumber)
{
	switch(stringNumber)
	{
		case 1:
		{
			cy_stc_dma_channel_config_t channelConfigWS1;
			channelConfigWS1.descriptor  = &WS1Descriptors[0];
			channelConfigWS1.preemptable = false;
			channelConfigWS1.priority    = 3;
			channelConfigWS1.enable      = false;
			Cy_DMA_Channel_Init(WS1_DMA_HW, WS1_DMA_CHANNEL, &channelConfigWS1);
			Cy_DMA_Channel_Enable(WS1_DMA_HW, WS1_DMA_CHANNEL);
			break;
		}
		case 2:
		{
			cy_stc_dma_channel_config_t channelConfigWS2;
			channelConfigWS2.descriptor  = &WS2Descriptors[0];
			channelConfigWS2.preemptable = false;
			channelConfigWS2.priority    = 3;
			channelConfigWS2.enable      = false;
			Cy_DMA_Channel_Init(WS2_DMA_HW, WS2_DMA_CHANNEL, &channelConfigWS2);
			Cy_DMA_Channel_Enable(WS2_DMA_HW, WS2_DMA_CHANNEL);
			break;
		}
		case 3:
		{
			cy_stc_dma_channel_config_t channelConfigWS3;
			channelConfigWS3.descriptor  = &WS3Descriptors[0];
			channelConfigWS3.preemptable = false;
			channelConfigWS3.priority    = 3;
			channelConfigWS3.enable      = false;
			Cy_DMA_Channel_Init(WS3_DMA_HW, WS3_DMA_CHANNEL, &channelConfigWS3);
			Cy_DMA_Channel_Enable(WS3_DMA_HW, WS3_DMA_CHANNEL);
			break;
		}
		case 4:
		{
			cy_stc_dma_channel_config_t channelConfigWS4;
			channelConfigWS4.descriptor  = &WS4Descriptors[0];
			channelConfigWS4.preemptable = false;
			channelConfigWS4.priority    = 3;
			channelConfigWS4.enable      = false;
			Cy_DMA_Channel_Init(WS4_DMA_HW, WS4_DMA_CHANNEL, &channelConfigWS4);
			Cy_DMA_Channel_Enable(WS4_DMA_HW, WS4_DMA_CHANNEL);
			break;
		}
		case 5:
		{
			cy_stc_dma_channel_config_t channelConfigWS5;
			channelConfigWS5.descriptor  = &WS5Descriptors[0];
			channelConfigWS5.preemptable = false;
			channelConfigWS5.priority    = 3;
			channelConfigWS5.enable      = false;
			Cy_DMA_Channel_Init(WS5_DMA_HW, WS5_DMA_CHANNEL, &channelConfigWS5);
			Cy_DMA_Channel_Enable(WS5_DMA_HW, WS5_DMA_CHANNEL);
			break;
		}
		default:
			break;
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
    if((Cy_DMA_Channel_GetStatus(WS1_DMA_HW, WS1_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
        WS_DMATrigger(1);
    }
    if((Cy_DMA_Channel_GetStatus(WS2_DMA_HW, WS2_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
    	WS_DMATrigger(2);
    }
    if((Cy_DMA_Channel_GetStatus(WS3_DMA_HW, WS3_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
    	WS_DMATrigger(3);
    }
    if((Cy_DMA_Channel_GetStatus(WS4_DMA_HW, WS4_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
    	WS_DMATrigger(4);
    }
    if((Cy_DMA_Channel_GetStatus(WS5_DMA_HW, WS5_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
    	WS_DMATrigger(5);
    }
}

/*******************************************************************************
* Function Name: WS_convert3Code
********************************************************************************
* Summary:
*  Function This function takes an 8-bit value representing a color and turns it
*  into a WS2812 bit code... where 1=110 and 0=011.
*  1 input byte turns into three output bytes of a uint32_t
*
* Parameters:
*  input: Input byte to convert
*
* Return:
*  void
*
*******************************************************************************/
static uint32_t WS_convert3Code(uint8_t input)
{
    uint32_t rval=0;
    for(int i=0;i<8;i++)
    {
        if(input%2)
        {
            rval |= WS_ONE3;
        }
        else
        {
            rval |= WS_ZERO3;
        }
        rval = rval >> 3;

        input = input >> 1;
    }
    return rval;
}

/*******************************************************************************
* Function Name: WS_setRGB
********************************************************************************
* Summary:
*  Function This function Takes a position and a three byte rgb value and
*  updates the WS_frameBuffer with the correct 9-bytes
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
static void WS_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    switch(stringNumber)
    {
		case 1:
		{
			WS_colorUnion color;
			color.word = WS_convert3Code(green);
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(red);
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(blue);
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
			WS1_frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
			break;
		}
		case 2:
		{
			WS_colorUnion color;
			color.word = WS_convert3Code(green);
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(red);
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(blue);
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
			WS2_frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
			break;
		}
		case 3:
		{
			WS_colorUnion color;
			color.word = WS_convert3Code(green);
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(red);
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(blue);
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
			WS3_frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
			break;
		}
		case 4:
		{
			WS_colorUnion color;
			color.word = WS_convert3Code(green);
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(red);
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(blue);
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
			WS4_frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
			break;
		}
		case 5:
		{
			WS_colorUnion color;
			color.word = WS_convert3Code(green);
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(red);
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

			color.word = WS_convert3Code(blue);
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
			WS5_frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
			break;
		}
		default:
			break;
    }
}

/*******************************************************************************
* Function Name: WS_setRange
********************************************************************************
* Summary:
*  Function This function Sets all of the pixels from start to end with the
*  red, green, and blue value
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
static void WS_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue)
{
    CY_ASSERT(start >= 0);
    CY_ASSERT(start < end);

    switch(stringNumber)
    {
		case 1:
		{
			CY_ASSERT(end <= ws2812_NUM_PIXELS_WS1 - 1);

			WS_setRGB(1, start, red, green, blue);
			for(uint32_t i = 1; i <= end - start; i++)
			{
				memcpy(&WS1_frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
						&WS1_frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
			}
			break;
		}
		case 2:
		{
			CY_ASSERT(end <= ws2812_NUM_PIXELS_WS2 - 1);

			WS_setRGB(2, start, red, green, blue);
			for(uint32_t i = 1; i <= end - start; i++)
			{
				memcpy(&WS2_frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
						&WS2_frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
			}
			break;
		}
		case 3:
		{
			CY_ASSERT(end <= ws2812_NUM_PIXELS_WS3 - 1);

			WS_setRGB(3, start, red, green, blue);
			for(uint32_t i = 1; i <= end - start; i++)
			{
				memcpy(&WS3_frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
						&WS3_frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
			}
			break;
		}
		case 4:
		{
			CY_ASSERT(end <= ws2812_NUM_PIXELS_WS4 - 1);

			WS_setRGB(4, start, red, green, blue);
			for(uint32_t i = 1; i <= end - start; i++)
			{
				memcpy(&WS4_frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
						&WS4_frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
			}
			break;
		}
		case 5:
		{
			CY_ASSERT(end <= ws2812_NUM_PIXELS_WS5 - 1);

			WS_setRGB(5, start, red, green, blue);
			for(uint32_t i = 1; i <= end - start; i++)
			{
				memcpy(&WS5_frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
						&WS5_frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
			}
			break;
		}
    default:
    	break;
    }
}

// Function: WS_runTest
// This function just runs test-asserts against the pixel calculation functions
static void WS1_runTest()
{
    printf("Size of WS_frameBuffer = %d\r\n", sizeof(WS1_frameBuffer));

    // Some unit tests for the convert3Code Function
    printf("Test 0x00 = %0X\r\n", (unsigned int)WS_convert3Code(0));
    printf("Test 0xFF = %0X\r\n", (unsigned int)WS_convert3Code(0xFF));
    printf("Test 0x80 = %0X\r\n", (unsigned int)WS_convert3Code(0x80));

    // Make sure that WS_convert3Code does the right thing
    CY_ASSERT(WS_convert3Code(0x00) == 0b00000000100100100100100100100100);
    CY_ASSERT(WS_convert3Code(0x80) == 0b00000000110100100100100100100100);
    CY_ASSERT(WS_convert3Code(0xFF) == 0b00000000110110110110110110110110);


    CY_ASSERT(ws2812_NUM_PIXELS_WS1 >= 3); // we are going to test 3 locations
    // Test the WS_setRGB
    WS_setRGB(1, 0, 0x80, 0, 0xFF);

    // 0
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 0] == 0b10010010);
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 1] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 2] == 0b00100100);

    // 0x80
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 3] == 0b11010010);
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 4] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[0 * 9+ WS_ZOFFSET + 5] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 6] == 0b11011011);
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 7] == 0b01101101);
    CY_ASSERT(WS1_frameBuffer[0 * 9 + WS_ZOFFSET + 8] == 0b10110110);

    WS_setRGB(1, 1, 0, 0xFF, 0x80);

    // 0xFF
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 0] == 0b11011011);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 1] == 0b01101101);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 2] == 0b10110110);

    // 0
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 3] == 0b10010010);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 4] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 5] == 0b00100100);

    // 0x80
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 6] == 0b11010010);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 7] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[1 * 9 + WS_ZOFFSET + 8] == 0b00100100);

    WS_setRGB(1, 2, 0xFF, 0x80, 0);

    // 0x80
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 0] == 0b11010010);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 1] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 3] == 0b11011011);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 4] == 0b01101101);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 5] == 0b10110110);

    // 0
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 6] == 0b10010010);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 7] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 8] == 0b00100100);


    // change the values of the range
    WS_setRGB(1, 2, 0, 0, 0);
    WS_setRGB(1, 3, 0, 0, 0);

    // Test the WS_setRange
    WS_setRange(1, 1, 3, 0xFF, 0x80, 0);

    // 0x80
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 0] == 0b11010010);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 1] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 3] == 0b11011011);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 4] == 0b01101101);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 5] == 0b10110110);

    // 0
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 6] == 0b10010010);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 7] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[2 * 9 + WS_ZOFFSET + 8] == 0b00100100);

    // 0x80
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 0] == 0b11010010);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 1] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 3] == 0b11011011);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 4] == 0b01101101);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 5] == 0b10110110);

    // 0
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 6] == 0b10010010);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 7] == 0b01001001);
    CY_ASSERT(WS1_frameBuffer[3 * 9 + WS_ZOFFSET + 8] == 0b00100100);

    for(uint8_t i = 0; i < (ws2812_NUM_PIXELS_WS1 * WS_BYTES_PER_PIXEL + WS_ZOFFSET); i++)
    {
        printf("%02X ", WS1_frameBuffer[i]);
    }
    printf("\r\n");
}

/*******************************************************************************
* Function Name: WS_initMixColorRGB
********************************************************************************
* Summary:
*  Function This function  Initializes the RGB frame buffer to RGBRGBRGB...RGB
*
* Parameters:
*  stringNumbner: The string number to address
*
* Return:
*  void
*
*******************************************************************************/
static void WS_initMixColorRGB(uint8_t stringNumber)
{
	uint32_t loopCounter;
	switch(stringNumber)
	{
		case 1:
		{
			loopCounter = ws2812_NUM_PIXELS_WS1;
			break;
		}
		case 2:
		{
			loopCounter = ws2812_NUM_PIXELS_WS2;
			break;
		}
		case 3:
		{
			loopCounter = ws2812_NUM_PIXELS_WS3;
			break;
		}
		case 4:
		{
			loopCounter = ws2812_NUM_PIXELS_WS4;
			break;
		}
		case 5:
		{
			loopCounter = ws2812_NUM_PIXELS_WS5;
			break;
		}
		default:
			loopCounter = ws2812_NUM_PIXELS_WS1;
			break;
	}

    for(uint32_t i = 0; i < loopCounter; i++)
    {
        switch(i%3)
        {
            case 0:
                WS_setRGB(stringNumber, i, 0x80, 0x00, 0x00); // red
                break;
            case 1:
                WS_setRGB(stringNumber, i, 0x00, 0x80, 0x00); // green
                break;
            case 2:
                WS_setRGB(stringNumber, i, 0x00, 0x00, 0x80); // blue
                break;
        }
    }
}

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
	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Declare all the SPI channel context */
	cy_stc_scb_spi_context_t WS1_SPI_context;
	cy_stc_scb_spi_context_t WS2_SPI_context;
	cy_stc_scb_spi_context_t WS3_SPI_context;
	cy_stc_scb_spi_context_t WS4_SPI_context;
	cy_stc_scb_spi_context_t WS5_SPI_context;

	/* Delay 100ms */
	vTaskDelay(pdMS_TO_TICKS(100));

	printf("Starting WS2812 task\r\n");

	//WS1_runTest();

	/* Initialize the FrameBuffers */
    WS1_frameBuffer[0] = 0x00;
    WS2_frameBuffer[0] = 0x00;
    WS3_frameBuffer[0] = 0x00;
    WS4_frameBuffer[0] = 0x00;
    WS5_frameBuffer[0] = 0x00;

    /* Initialize all LEDs Off and update the DMA trigger to load */
    WS_setRange(1, 0, ws2812_NUM_PIXELS_WS1 - 1, 0, 0, 0);
    WS_DMATrigger(1);
    Cy_SCB_SPI_Init(WS1_SPI_HW, &WS1_SPI_config, &WS1_SPI_context);
    Cy_SCB_SPI_Enable(WS1_SPI_HW);

    WS1_DMAConfigure();
    WS_setRange(2, 0, ws2812_NUM_PIXELS_WS2 - 1, 0, 0, 0);
    WS_DMATrigger(2);
    Cy_SCB_SPI_Init(WS2_SPI_HW, &WS2_SPI_config, &WS2_SPI_context);
    Cy_SCB_SPI_Enable(WS2_SPI_HW);
    WS2_DMAConfigure();

    WS_setRange(3, 0, ws2812_NUM_PIXELS_WS3 - 1, 0, 0, 0);
    WS_DMATrigger(3);
    Cy_SCB_SPI_Init(WS3_SPI_HW, &WS3_SPI_config, &WS3_SPI_context);
    Cy_SCB_SPI_Enable(WS3_SPI_HW);
    WS3_DMAConfigure();

    WS_setRange(4, 0, ws2812_NUM_PIXELS_WS4 - 1, 0, 0, 0);
    WS_DMATrigger(4);
    Cy_SCB_SPI_Init(WS4_SPI_HW, &WS4_SPI_config, &WS4_SPI_context);
    Cy_SCB_SPI_Enable(WS4_SPI_HW);
    WS4_DMAConfigure();

    WS_setRange(5, 0, ws2812_NUM_PIXELS_WS5 - 1, 0, 0, 0);
    WS_DMATrigger(5);
    Cy_SCB_SPI_Init(WS5_SPI_HW, &WS5_SPI_config, &WS5_SPI_context);
    Cy_SCB_SPI_Enable(WS5_SPI_HW);
    WS5_DMAConfigure();

    /* This queue handles messages sent to the queue to execute */
    ws2812QueueHandle = xQueueCreate(WS2812_QUEUE_SIZE, sizeof(ws2812_msg_t));

    /* This timer calls the update function every 30ms if it is turned on. */
    ws2812TimerHandle = xTimerCreate(	/* A text name, purely to help debugging. */
    									"ws2812 timer",
    									/* The timer period, in ms */
										WS2812_AUTOUPDATE_TIMER_MS,
										/* This is a periodic timer, so
										   xAutoReload is set to pdTRUE. */
										pdTRUE,
										/* The ID is not used, so can be set
										   to anything. */
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
					WS_DMATrigger(1);
					WS_DMATrigger(2);
					WS_DMATrigger(3);
					WS_DMATrigger(4);
					WS_DMATrigger(5);
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
