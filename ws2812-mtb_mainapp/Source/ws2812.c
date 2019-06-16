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
#include "ws2812.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

QueueHandle_t ws2812QueueHandle;
TimerHandle_t ws2812TimerHandle;

bool wsAutoUpdateState = false;


#define WS_ZOFFSET (1)
#define WS_ONE3  (0b110<<24)
#define WS_ZERO3 (0b100<<24)
#define WS_SPI_BIT_PER_BIT (3)
#define WS_COLOR_PER_PIXEL (3)
#define WS_BYTES_PER_PIXEL (WS_SPI_BIT_PER_BIT * WS_COLOR_PER_PIXEL)

static uint8_t WS_frameBuffer[ws2812_NUM_PIXELS*WS_BYTES_PER_PIXEL+WS_ZOFFSET];


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



// Function WS_DMAConfiguration
// This function sets up the DMA and the descriptors

#define WS_NUM_DESCRIPTORS (sizeof(WS_frameBuffer) / 256 + 1)
static cy_stc_dma_descriptor_t WSDescriptors[WS_NUM_DESCRIPTORS];
static void WS_DMAConfigure(void)
{
    // I copies this structure from the PSoC Creator Component configuration
    // in generated source
    const cy_stc_dma_descriptor_config_t WS_DMA_Descriptors_config =
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

    for(unsigned int i=0;i<WS_NUM_DESCRIPTORS;i++)
    {
        Cy_DMA_Descriptor_Init(&WSDescriptors[i], &WS_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&WSDescriptors[i], (uint8_t *)&WS_frameBuffer[i*256]);
        Cy_DMA_Descriptor_SetDstAddress(&WSDescriptors[i], (void *)&WS_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&WSDescriptors[i],256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&WSDescriptors[i],&WSDescriptors[i+1]);
    }

    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&WSDescriptors[WS_NUM_DESCRIPTORS-1],sizeof(WS_frameBuffer)-256*(WS_NUM_DESCRIPTORS-1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&WSDescriptors[WS_NUM_DESCRIPTORS-1],0);
    Cy_DMA_Descriptor_SetChannelState(&WSDescriptors[WS_NUM_DESCRIPTORS-1],CY_DMA_CHANNEL_DISABLED);

    Cy_DMA_Enable(WS_DMA_HW);
}

// Function: WS_DMATrigger
// This function sets up the channel... then enables it to dump the frameBuffer to pixels
void WS_DMATrigger()
{

    cy_stc_dma_channel_config_t channelConfig;
    channelConfig.descriptor  = &WSDescriptors[0];
    channelConfig.preemptable = false;
    channelConfig.priority    = 3;
    channelConfig.enable      = false;
    Cy_DMA_Channel_Init(WS_DMA_HW, WS_DMA_CHANNEL, &channelConfig);
    Cy_DMA_Channel_Enable(WS_DMA_HW,WS_DMA_CHANNEL);
}


// This function is called by the software timer which is used to autoupdate the LEDs
// It checks to make sure that the DMA is done... if not it doesnt do anything
void ws2812CallbackFunction( TimerHandle_t xTimer )
{
    if((Cy_DMA_Channel_GetStatus(WS_DMA_HW,WS_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
    {
        WS_DMATrigger();
    }
}

// Function: convert3Code
// This function takes an 8-bit value representing a color
// and turns it into a WS2812 bit code... where 1=110 and 0=011
// 1 input byte turns into three output bytes of a uint32_t
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

// Function: WS_setRGB
// Takes a position and a three byte rgb value and updates the WS_frameBuffer with the correct 9-bytes
static void WS_setRGB(int led,uint8_t red, uint8_t green, uint8_t blue)
{

    typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    WS_colorUnion color;
    color.word = WS_convert3Code(green);
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+WS_ZOFFSET] = color.bytes[2];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+1+WS_ZOFFSET] = color.bytes[1];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+2+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(red);
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+3+WS_ZOFFSET] = color.bytes[2];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+4+WS_ZOFFSET] = color.bytes[1];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+5+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(blue);
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+6+WS_ZOFFSET] = color.bytes[2];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+7+WS_ZOFFSET] = color.bytes[1];
    WS_frameBuffer[led*WS_BYTES_PER_PIXEL+8+WS_ZOFFSET] = color.bytes[0];
}

// Function WS_setRange
// Sets all of the pixels from start to end with the red,green,blue value
static void WS_setRange(int start, int end, uint8_t red,uint8_t green ,uint8_t blue)
{
    CY_ASSERT(start >= 0);
    CY_ASSERT(start < end);
    CY_ASSERT(end <= ws2812_NUM_PIXELS-1);

    WS_setRGB(start,red,green,blue);
    for(int i=1;i<=end-start;i++)
    {
        memcpy(&WS_frameBuffer[start*WS_BYTES_PER_PIXEL+i*WS_BYTES_PER_PIXEL+WS_ZOFFSET],
        &WS_frameBuffer[start*WS_BYTES_PER_PIXEL+WS_ZOFFSET],WS_BYTES_PER_PIXEL);
    }
}
// Function: WS_runTest
// This function just runs test-asserts against the pixel calculation functions
static void WS_runTest()
{
    printf("Size of WS_frameBuffer = %d\n",sizeof(WS_frameBuffer));

    // Some unit tests for the convert3Code Function
    printf("Test 0x00 = %0X\n",(unsigned int)WS_convert3Code(0));
    printf("Test 0xFF = %0X\n",(unsigned int)WS_convert3Code(0xFF));
    printf("Test 0x80 = %0X\n",(unsigned int)WS_convert3Code(0x80));

    // Make sure that WS_convert3Code does the right thing
    CY_ASSERT(WS_convert3Code(0x00) == 0b00000000100100100100100100100100);
    CY_ASSERT(WS_convert3Code(0x80) == 0b00000000110100100100100100100100);
    CY_ASSERT(WS_convert3Code(0xFF) == 0b00000000110110110110110110110110);


    CY_ASSERT(ws2812_NUM_PIXELS>=3); // we are goign to test 3 locations
    // Test the WS_setRGB
    WS_setRGB(0,0x80,0,0xFF);

    // 0
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+0] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+1] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+2] == 0b00100100);

    // 0x80
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+3] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+4] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+5] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+6] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+7] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[0*9+WS_ZOFFSET+8] == 0b10110110);

    WS_setRGB(1,0,0xFF,0x80);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+0] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+1] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+2] == 0b10110110);

    // 0
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+3] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+4] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+5] == 0b00100100);

    // 0x80
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+6] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+7] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[1*9+WS_ZOFFSET+8] == 0b00100100);

    WS_setRGB(2,0xFF,0x80,0);

    // 0x80
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+0] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+1] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+3] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+4] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+5] == 0b10110110);

    // 0
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+6] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+7] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+8] == 0b00100100);


    // change the values of the range
    WS_setRGB(2,0,0,0);
    WS_setRGB(3,0,0,0);
    WS_setRGB(4,0,0,0);

    // Test the WS_setRange
    WS_setRange(2,4,0xFF,0x80,0);

    // 0x80
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+0] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+1] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+3] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+4] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+5] == 0b10110110);

    // 0
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+6] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+7] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[2*9+WS_ZOFFSET+8] == 0b00100100);

    // 0x80
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+0] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+1] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+3] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+4] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+5] == 0b10110110);

    // 0
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+6] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+7] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[3*9+WS_ZOFFSET+8] == 0b00100100);

    // 0x80
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+0] == 0b11010010);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+1] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+2] == 0b00100100);

    // 0xFF
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+3] == 0b11011011);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+4] == 0b01101101);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+5] == 0b10110110);

    // 0
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+6] == 0b10010010);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+7] == 0b01001001);
    CY_ASSERT(WS_frameBuffer[4*9+WS_ZOFFSET+8] == 0b00100100);


    for(int i=0;i<10;i++)
    {
        printf("%02X ",WS_frameBuffer[i]);
    }
    printf("\n");

}

// Initializes the RGB frame buffer to RGBRGBRGB...RGB
static void WS_initMixColorRGB()
{
    for(int i=0;i<ws2812_NUM_PIXELS;i++)
    {
        switch(i%3)
        {
            case 0:
                WS_setRGB(i,0x80,0x00,0x00); // red
                break;
            case 1:
                WS_setRGB(i,0x00,0x80,0x00); // green
                break;
            case 2:
                WS_setRGB(i,0x00,0x00,0x80); // blue
                break;
        }
    }
}



void ws2812Task(void *arg)
{
	ws2812_msg_t msg;
	cy_stc_scb_spi_context_t WS_SPI_context;

	vTaskDelay(100);

	printf("Starting ws2812 task\n");
	WS_runTest();
    WS_frameBuffer[0] = 0x00;
    WS_setRange(0,ws2812_NUM_PIXELS-1,0,0,0); // Initialize everything OFF
    Cy_SCB_SPI_Init(WS_SPI_HW, &WS_SPI_config, &WS_SPI_context);
    Cy_SCB_SPI_Enable(WS_SPI_HW);
    WS_DMAConfigure();

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
    				WS_DMATrigger();
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
    			WS_setRGB( msg.data,msg.red,msg.green ,msg.blue);
    			break;
    		case ws2812_cmd_setRange:
    			WS_setRange(msg.data>>16 & 0xFFFF, msg.data&0xFFFF, msg.red,msg.green ,msg.blue);
    			break;
    		case ws2812_cmd_initMixColorRGB:
    			WS_initMixColorRGB();
    			break;
    		}
    }
}
