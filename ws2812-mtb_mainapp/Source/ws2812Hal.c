#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>
#include <stdint.h>
#include "ws2812Hal.h"
#include <stdlib.h>

#define WS_ZOFFSET (1)
#define WS_ONE3  (0b110<<24)
#define WS_ZERO3 (0b100<<24)
#define WS_SPI_BIT_PER_BIT  (3)
#define WS_COLOR_PER_PIXEL  (3)
const int WS_BYTES_PER_PIXEL = (WS_SPI_BIT_PER_BIT * WS_COLOR_PER_PIXEL);

typedef struct {
	GPIO_PRT_Type *spiPort;
	uint32_t spiPin;
	CySCB_Type*spiHw;
	DW_Type *dwHW;
	uint32_t channel;

} ledMapElement_t;

const ledMapElement_t ledStringTable[]= {
		{GPIO_PRT0,2,SCB0,DW0,16},
};

const uint32_t numPossibleElements = sizeof(ledStringTable)/sizeof(ledMapElement_t);

typedef struct {
	uint32_t numLeds;
	ledMapElement_t *map;
	cy_stc_scb_spi_context_t *spiContext;
	uint8_t *frameBuffer;
	uint32_t frameBufferSize;
	uint32_t numDescriptors;
	cy_stc_dma_descriptor_t *dmaDescr;
} ledString_t;

#define MAX_LED_STRINGS 5
ledString_t ledStrings[MAX_LED_STRINGS];

uint32_t numLedStrings=0;

// Static Function Prototypes
static void WS_DMAConfigure(uint32_t string);
static uint32_t WS_convert3Code(uint8_t input);


uint32_t WS_getNumLeds(uint32_t string)
{

	return ledStrings[string].numLeds;
}


void WS_Start()
{

}

int WS_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin,int numLeds)
{

	int rval=-1;

	for(int i=0;i<numPossibleElements;i++)
	{
		if(ledStringTable[i].spiPort == spiPrt && ledStringTable[i].spiPin == spiPin)
		{
			rval = i;
			ledStrings[numLedStrings].map = &ledStringTable[i];
			break;
		}
	}
	if(rval == -1)
		return rval;


	ledStrings[numLedStrings].numLeds = numLeds;
	ledStrings[numLedStrings].spiContext = malloc(sizeof(cy_stc_scb_spi_context_t));
	ledStrings[numLedStrings].frameBufferSize =  numLeds*WS_BYTES_PER_PIXEL+WS_ZOFFSET;
	ledStrings[numLedStrings].frameBuffer = malloc(ledStrings[numLedStrings].frameBufferSize);
	ledStrings[numLedStrings].numDescriptors = ((numLeds*WS_BYTES_PER_PIXEL+WS_ZOFFSET)) / 256 + 1;
	ledStrings[numLedStrings].dmaDescr = malloc(sizeof(cy_stc_dma_descriptor_t) * ledStrings[numLedStrings].numDescriptors);

	ledStrings[numLedStrings].frameBuffer[0] = 0x00;
    WS_setRange(0,0,numLeds-1,0,0,0); // Initialize everything OFF
    Cy_SCB_SPI_Init(WS_SPI_HW, &WS_SPI_config, ledStrings[numLedStrings].spiContext);
    Cy_SCB_SPI_Enable(WS_SPI_HW);
    WS_DMAConfigure(numLedStrings);
    Cy_DMA_Enable(DW0); // enable the datawire 0 hardware block

    numLedStrings += 1;

    return numLedStrings - 1; // return the string #
}

// Function WS_DMAConfiguration
// This function sets up the DMA and the descriptors

static void WS_DMAConfigure(uint32_t string)
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

    for(unsigned int i=0;i<ledStrings[string].numDescriptors;i++)
    {
        Cy_DMA_Descriptor_Init(&ledStrings[string].dmaDescr[i], &WS_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&ledStrings[string].dmaDescr[i], (uint8_t *)&ledStrings[string].frameBuffer[i*256]);
        Cy_DMA_Descriptor_SetDstAddress(&ledStrings[string].dmaDescr[i], (void *)&WS_SPI_HW->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&ledStrings[string].dmaDescr[i],256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&ledStrings[string].dmaDescr[i],&ledStrings[string].dmaDescr[i+1]);
    }


    // The last one needs a bit of change
    Cy_DMA_Descriptor_SetXloopDataCount(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1],ledStrings[string].frameBufferSize-256*(ledStrings[string].numDescriptors-1)); // the last
    Cy_DMA_Descriptor_SetNextDescriptor(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1],0);
    Cy_DMA_Descriptor_SetChannelState(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1],CY_DMA_CHANNEL_DISABLED);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function: WS_DMATrigger
// This function sets up the channel... then enables it to dump the frameBuffer to pixels
void WS_updateString(uint32_t string)
{
	//if((Cy_DMA_Channel_GetStatus(WS_DMA_HW,WS_DMA_CHANNEL) & CY_DMA_INTR_CAUSE_COMPLETION))
	if((Cy_DMA_Channel_GetStatus(ledStrings[string].map->dwHW,ledStrings[string].map->channel) & CY_DMA_INTR_CAUSE_COMPLETION))
	{
		cy_stc_dma_channel_config_t channelConfig;
		channelConfig.descriptor  = &ledStrings[string].dmaDescr[0];
		channelConfig.preemptable = false;
		channelConfig.priority    = 3;
		channelConfig.enable      = false;
		Cy_DMA_Channel_Init(ledStrings[string].map->dwHW,ledStrings[string].map->channel, &channelConfig);
		Cy_DMA_Channel_Enable(ledStrings[string].map->dwHW,ledStrings[string].map->channel);
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
void WS_setRGB(int string,int led,uint8_t red, uint8_t green, uint8_t blue)
{

    typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    WS_colorUnion color;
    color.word = WS_convert3Code(green);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+1+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+2+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(red);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+3+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+4+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+5+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(blue);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+6+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+7+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+8+WS_ZOFFSET] = color.bytes[0];
}

// Function WS_setRange
// Sets all of the pixels from start to end with the red,green,blue value
void WS_setRange(int string,int start, int end, uint8_t red,uint8_t green ,uint8_t blue)
{
    CY_ASSERT(start >= 0);
    CY_ASSERT(start < end);
    CY_ASSERT(end <= ledStrings[string].numLeds-1);

    WS_setRGB(0,start,red,green,blue);
    for(int i=1;i<=end-start;i++)
    {
        memcpy(&ledStrings[string].frameBuffer[start*WS_BYTES_PER_PIXEL+i*WS_BYTES_PER_PIXEL+WS_ZOFFSET],
        &ledStrings[string].frameBuffer[start*WS_BYTES_PER_PIXEL+WS_ZOFFSET],WS_BYTES_PER_PIXEL);
    }
}

#if 0

// Function: WS_runTest
// This function just runs test-asserts against the pixel calculation functions
void WS_runTest()
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
#endif
