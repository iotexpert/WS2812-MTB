/*
 * uartTask.c
 *
 *  Created on: Jun 16, 2019
 *      Author: arh
 */

#include <stdio.h>
#include "ws2812.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cy_device_headers.h"
#include "cycfg.h"
#include "cy_pdl.h"

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

	printf("Starting UART Task\n");

	for(;;)
	{
		xSemaphoreTake( UART_STDIO_SemaphoreHandle, 0xFFFFFFFF); // Wait for a semaphore

		while(Cy_SCB_UART_GetNumInRxFifo(UART_STDIO_HW))
		{
			char c=getchar();
			switch(c)
			{
			case 'u':
				printf("Enable auto DMA updating\n");
				ws2812_autoUpdate(true);
				break;
			case 'U':
				printf("Disable auto DMA updating\n");

				ws2812_autoUpdate(false);
				break;
			case 't':
				printf("Update LEDs\n");
				ws2812_update();
				break;
			case 'r':
				ws2812_setRGB(0,0xFF,0,0);
				printf("Set LED0 Red\n");
				break;
			case 'g':
				ws2812_setRGB(0,0,0xFF,0);
				printf("Set LED0 Green\n");
				break;
			case 'O':
				ws2812_setRange(0,ws2812_NUM_PIXELS-1,0,0,0);
				printf("Turn off all LEDs\n");
				break;
			case 'o':
				ws2812_setRange(0,ws2812_NUM_PIXELS-1,0xFF,0xFF,0xFF);
				printf("Turn on all LEDs\n");
				break;
			case 'b':
				ws2812_setRGB(0,0,0,0xFF);
				printf("Set LED0 Blue\n");
				break;
			case 'R':
				ws2812_setRange(0,ws2812_NUM_PIXELS-1,0x80,0,0);
				printf("Turn on all LEDs RED\n");
				break;
			case 'G':
				ws2812_setRange(0,ws2812_NUM_PIXELS-1,0,0x80,0);
				printf("Turn on all LEDs Green\n");
				break;
			case 'B':
				ws2812_setRange(0,ws2812_NUM_PIXELS-1,0,0,0x80);
				printf("Turn on all LEDs Blue\n");
				break;
			case 'a':
				ws2812_initMixColorRGB();
				printf("Turn on all LEDs RGB Pattern\n");
				break;
			case '?':
				printf("u\tEnable Auto Update of LEDs\n");
				printf("U\tDisable Auto Update of LEDs\n");
				printf("t\tTrigger the DMA\n");
				printf("r\tSet the first pixel Red\n");
				printf("g\tSet the first pixel Green\n");
				printf("b\tSet the first pixel Blue\n");
				printf("O\tTurn off all of the pixels\n");
				printf("o\tSet the pixels to white full on\n");
				printf("R\tSet all of the pixels to Red\n");
				printf("G\tSet all of the pixels to Green\n");
				printf("B\tSet all of the pixels to Blue\n");
				printf("a\tSet pixels to repeating RGBRGB\n");
				printf("?\tHelp\n");
				break;
			}
		}
		// turn the rx fifo interrupt back on
        Cy_SCB_SetRxInterruptMask(UART_STDIO_HW,CY_SCB_RX_INTR_NOT_EMPTY); // Turn on interrupts for Rx buffer
	}
}
