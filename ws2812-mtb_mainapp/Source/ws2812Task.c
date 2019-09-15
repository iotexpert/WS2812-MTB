/*
 * ws2812.c
 *
 *  Created on: Jun 15, 2019
 *      Author: arh
 */

#include "cy_pdl.h"
#include "cycfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ws2812Task.h"
#include "ws2812Graphics.h"
#include "ws2812Hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"


QueueHandle_t ws2812QueueHandle;
TimerHandle_t ws2812TimerHandle;

bool wsAutoUpdateState = false;

// These functions are helpers to create the message to send to the ws2812 task.

void ws2812_update(void)
{
	ws2812_msg_t msg;
	msg.cmd = ws2812_cmd_update;
	xQueueSend(ws2812QueueHandle,&msg,0);
}

void ws2812_autoUpdate(bool option)
{
	ws2812_msg_t msg;
	msg.cmd = ws2812_cmd_autoUpdate;
	msg.data = option;
	xQueueSend(ws2812QueueHandle,&msg,0);
}
void ws2812_setRGB(int led,uint8_t red, uint8_t green, uint8_t blue)
{
	ws2812_msg_t msg;
	msg.cmd = ws2812_cmd_setRGB;
	msg.red = red;
	msg.blue = blue;
	msg.green = green;
	msg.data = led;
	xQueueSend(ws2812QueueHandle,&msg,0);

}
void ws2812_setRange(int start, int end, uint8_t red,uint8_t green ,uint8_t blue)
{

	ws2812_msg_t msg;
	msg.cmd = ws2812_cmd_setRange;
	msg.red = red;
	msg.blue = blue;
	msg.green = green;
	msg.data = start << 16 | end;
	xQueueSend(ws2812QueueHandle,&msg,0);

}
void ws2812_initMixColorRGB(void)
{
	ws2812_msg_t msg;
	msg.cmd = ws2812_cmd_initMixColorRGB;
	xQueueSend(ws2812QueueHandle,&msg,0);
}


// This function is called by the software timer which is used to autoupdate the LEDs
// It checks to make sure that the DMA is done... if not it doesnt do anything
void ws2812CallbackFunction( TimerHandle_t xTimer )
{
	WS_updateString(0); // ARH... temporary
}


void ws2812Task(void *arg)
{
	ws2812_msg_t msg;

	vTaskDelay(100);

	WS_CreateString(GPIO_PRT0,2,5);

	// This queue handles messages from the keyboard
    ws2812QueueHandle = xQueueCreate( 10,sizeof(ws2812_msg_t));
    // This timer calls the update function every 30ms if it is turned on.
    ws2812TimerHandle = xTimerCreate("ws2812 timer",pdMS_TO_TICKS(30),pdTRUE,0,ws2812CallbackFunction );

    while(1)
    {
    		xQueueReceive(ws2812QueueHandle,&msg,0xFFFFFFFF);
    		switch(msg.cmd)
    		{
    		case ws2812_cmd_update:
    			if(!wsAutoUpdateState)
    			{
    				WS_updateString(0);
    			}
    			break;
    		case ws2812_cmd_autoUpdate:
    			if(wsAutoUpdateState && msg.data == false)
    			{
    				xTimerStop(ws2812TimerHandle,0);
    			}
    			else if(!wsAutoUpdateState && msg.data == true)
    			{
    				xTimerStart(ws2812TimerHandle,0);
    			}
    			wsAutoUpdateState = msg.data;

    			break;
    		case ws2812_cmd_setRGB:
    			WS_setRGB(0, msg.data,msg.red,msg.green ,msg.blue);
    			break;
    		case ws2812_cmd_setRange:
    			WS_setRange(0,msg.data>>16 & 0xFFFF, msg.data&0xFFFF, msg.red,msg.green ,msg.blue);
    			break;
    		case ws2812_cmd_initMixColorRGB:
    			WS_initMixColorRGB(0);
    			break;
    		}
    }
}
