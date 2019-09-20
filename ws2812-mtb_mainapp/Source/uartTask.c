/** @source __uartTask.c__
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

#include <stdio.h>
#include "ws2812.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cy_device_headers.h"
#include "cycfg.h"
#include "cy_pdl.h"

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



/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: void Task_Display (void *pvParameters)
********************************************************************************
* Summary:
*  Task that processes the touch command received and the updates the display
*  with the corresponding menu / text page
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  void
*
*******************************************************************************/

cy_stc_scb_uart_context_t UART_STDIO_context;
SemaphoreHandle_t UART_STDIO_SemaphoreHandle;

void UART_Isr(void)
{

	// Disable & clear the interrupt
	Cy_SCB_SetRxInterruptMask(UART_STDIO_HW,0);
	Cy_SCB_ClearRxInterrupt(UART_STDIO_HW, CY_SCB_RX_INTR_NOT_EMPTY);

	static BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( UART_STDIO_SemaphoreHandle, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uartTask(void *arg)
{
	(void)arg;

	UART_STDIO_SemaphoreHandle = xSemaphoreCreateCounting( 0xFFFF,0); // Semaphore counts unprocessed key presses

	Cy_SCB_UART_Init(UART_STDIO_HW,&UART_STDIO_config,&UART_STDIO_context);
	cy_stc_sysint_t uartIntrConfig =
	{
			.intrSrc      = UART_STDIO_IRQ,
			.intrPriority = 7,
	};

	(void) Cy_SysInt_Init(&uartIntrConfig, &UART_Isr);
	NVIC_EnableIRQ(UART_STDIO_IRQ);
    Cy_SCB_SetRxInterruptMask(UART_STDIO_HW,CY_SCB_RX_INTR_NOT_EMPTY);
    setvbuf( stdin, NULL, _IONBF, 0 ); // Turn off Input buffering on STDIO
	Cy_SCB_UART_Enable(UART_STDIO_HW);
	/* Clear Screen */
	printf("\033[2J");
	/* Send Cursor Home - Top Left */
	printf("\033[0;0H\n");
	printf("Starting UART Task\r\n");

	for(;;)
	{
		/* Wait for a semaphore */
		xSemaphoreTake( UART_STDIO_SemaphoreHandle, 0xFFFFFFFF);

		while(Cy_SCB_UART_GetNumInRxFifo(UART_STDIO_HW))
		{
			char c=getchar();
			switch(c)
			{
			case 'u':
				printf("Enable auto DMA updating\r\n");
				ws2812_autoUpdate(true);
				break;
			case 'U':
				printf("Disable auto DMA updating\r\n");
				ws2812_autoUpdate(false);
				break;
			case 't':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_update(i);
					}
				}
				printf("Update LEDs\r\n");
				break;
			case 'r':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRGB(i, 0, 0xFF, 0, 0);
					}
				}
				printf("Set LED0 Red\r\n");
				break;
			case 'g':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRGB(i, 0, 0, 0xFF, 0);
					}
				}
				printf("Set LED0 Green\r\n");
				break;
			case 'O':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRange(i, 0, WS_getNumLeds(i) - 1, 0, 0, 0);
					}
				}
				printf("Turn off all LEDs\r\n");
				break;
			case 'o':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRange(i, 0,WS_getNumLeds(i) - 1, 0xFF, 0xFF, 0xFF);
					}
				}
				printf("Turn on all LEDs\r\n");
				break;
			case 'b':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRGB(i, 0, 0, 0, 0xFF);
					}
				}
				printf("Set LED0 Blue\r\n");
				break;
			case 'R':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRange(i, 0, WS_getNumLeds(i) - 1, 0x80, 0, 0);
					}
				}
				printf("Turn on all LEDs RED\r\n");
				break;
			case 'G':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRange(i, 0, WS_getNumLeds(i) - 1, 0, 0x80, 0);
					}
				}
				printf("Turn on all LEDs Green\r\n");
				break;
			case 'B':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_setRange(i, 0, WS_getNumLeds(i) - 1, 0, 0, 0x80);
					}
				}
				printf("Turn on all LEDs Blue\r\n");
				break;
			case 'a':
				for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
				{
					/* If channel is enabled, trigger the DMA */
					if(channelStatus[i] == 1)
					{
						ws2812_initMixColorRGB(i);
					}
				}
				printf("Turn on all LEDs RGB Pattern\r\n");
				break;
			case '!':
//				for(uint32_t j = 0; j < channelConfig[4].frameBufferLength; j++)
//				{
//					printf("%#02x ", channelConfig[4].frameBuffer[j]);
//					if(j%9 == 0)
//					{
//						printf("\r\n LED %lu: ", j/9);
//					}
//				}
//				printf("\r\n");
//				for(uint32_t r = 0; r < channelConfig[4].rows; r++)
//				{
//					printf("Row %lu:", r);
//					for(uint32_t c = 0; c < channelConfig[4].columns; c++)
//					{
//						printf("%#04x ", (unsigned int)channelConfig[4].ledArray[r][c]);
//					}
//					printf("\r\n");
//				}
				break;
			case '?':
				printf("u\tEnable Auto Update of LEDs\r\n");
				printf("U\tDisable Auto Update of LEDs\r\n");
				printf("t\tTrigger the DMA\r\n");
				printf("r\tSet the first pixel Red\r\n");
				printf("g\tSet the first pixel Green\r\n");
				printf("b\tSet the first pixel Blue\r\n");
				printf("O\tTurn off all of the pixels\r\n");
				printf("o\tSet the pixels to white full on\r\n");
				printf("R\tSet all of the pixels to Red\r\n");
				printf("G\tSet all of the pixels to Green\r\n");
				printf("B\tSet all of the pixels to Blue\r\n");
				printf("a\tSet pixels to repeating RGBRGB\r\n");
				printf("!\tPrint Frame buffers \r\n");
				printf("?\tHelp\r\n");
				break;
			}
		}
		// turn the rx fifo interrupt back on
        Cy_SCB_SetRxInterruptMask(UART_STDIO_HW,CY_SCB_RX_INTR_NOT_EMPTY); // Turn on interrupts for Rx buffer
	}
}
