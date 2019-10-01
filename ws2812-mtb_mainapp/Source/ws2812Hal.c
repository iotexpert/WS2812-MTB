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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ws2812Hal.h"
#include "ws2812Graphics.h"

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
/* #define and enum statements go here */

/* WS2812 specific packets for communicating */
#define WS_ZOFFSET 				(1u)
#define WS_ONE3  				(0b110<<24)
#define WS_ZERO3 				(0b100<<24)
#define WS_SPI_BIT_PER_BIT  	(3u)
#define WS_COLOR_PER_PIXEL  	(3u)

const int WS_BYTES_PER_PIXEL = (WS_SPI_BIT_PER_BIT * WS_COLOR_PER_PIXEL);

/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */
/* struct for the hardware channel configuration */
typedef struct {
	GPIO_PRT_Type *spiPort;
	uint32_t spiPin;
	en_hsiom_sel_t hsiom;
	CySCB_Type*spiHw;
	en_clk_dst_t spiClk;
	DW_Type *dwHW;
	uint32_t channel;
	uint32_t outTrig;
} ledMapElement_t;

/* Lookup table: GPIO_PRT and Pin number as index */
const ledMapElement_t ledStringTable[]= {
		{GPIO_PRT0,2,P0_2_SCB0_SPI_MOSI,SCB0,PCLK_SCB0_CLOCK,DW0,16,TRIG_OUT_1TO1_0_SCB0_TX_TO_PDMA0_TR_IN16},

		{GPIO_PRT2,0,P2_0_SCB1_SPI_MOSI,SCB1,PCLK_SCB1_CLOCK,DW0,18,TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18},
		{GPIO_PRT10,0,P10_0_SCB1_SPI_MOSI,SCB1,PCLK_SCB1_CLOCK,DW0,18,TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18},

		{GPIO_PRT3,0,P3_0_SCB2_SPI_MOSI,SCB2,PCLK_SCB2_CLOCK,DW0,20,TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20},
		{GPIO_PRT9,0,P9_0_SCB2_SPI_MOSI,SCB2,PCLK_SCB2_CLOCK,DW0,20,TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20},

		{GPIO_PRT6,0,P6_0_SCB3_SPI_MOSI,SCB3,PCLK_SCB3_CLOCK,DW0,22,TRIG_OUT_1TO1_0_SCB3_TX_TO_PDMA0_TR_IN22},

		{GPIO_PRT7,0,P7_0_SCB4_SPI_MOSI,SCB4,PCLK_SCB4_CLOCK,DW0,24,TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24},
		{GPIO_PRT8,0,P8_0_SCB4_SPI_MOSI,SCB4,PCLK_SCB4_CLOCK,DW0,24,TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24},

		{GPIO_PRT11,0,P11_0_SCB5_SPI_MOSI,SCB5,PCLK_SCB5_CLOCK,DW0,26,TRIG_OUT_1TO1_0_SCB5_TX_TO_PDMA0_TR_IN26},
		{GPIO_PRT5,0,P5_0_SCB5_SPI_MOSI,SCB5,PCLK_SCB5_CLOCK,DW0,26,TRIG_OUT_1TO1_0_SCB5_TX_TO_PDMA0_TR_IN26},

		{GPIO_PRT12,0,P12_0_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},
		{GPIO_PRT13,0,P13_0_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},
		{GPIO_PRT6,4,P6_4_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},

		{GPIO_PRT1,0,P1_0_SCB7_SPI_MOSI,SCB7,PCLK_SCB7_CLOCK,DW1,10,TRIG_OUT_1TO1_1_SCB7_TX_TO_PDMA1_TR_IN10},
		{GPIO_PRT4,0,P4_0_SCB7_SPI_MOSI,SCB7,PCLK_SCB7_CLOCK,DW1,10,TRIG_OUT_1TO1_1_SCB7_TX_TO_PDMA1_TR_IN10},

		{GPIO_PRT6,0,P6_0_SCB8_SPI_MOSI,SCB8,PCLK_SCB8_CLOCK,DW1,12,TRIG_OUT_1TO1_1_SCB8_TX_TO_PDMA1_TR_IN12},
		{GPIO_PRT6,4,P6_4_SCB8_SPI_MOSI,SCB8,PCLK_SCB8_CLOCK,DW1,12,TRIG_OUT_1TO1_1_SCB8_TX_TO_PDMA1_TR_IN12},
};

/* Number of possible HW/Channel possibilities. Used in looping through the lookup table */
const uint32_t numPossibleElements = sizeof(ledStringTable)/sizeof(ledMapElement_t);

/* String specific struct holding all String configuration */
typedef struct {
	const ledMapElement_t *map;
	cy_stc_scb_spi_context_t *spiContext;
	uint8_t *frameBuffer;
	uint32_t frameBufferSize;
	uint32_t numDescriptors;
	cy_stc_dma_descriptor_t *dmaDescr;
	uint32_t numLeds;
	uint8_t ledLayout;
	uint32_t columns;
	uint32_t rows;
	uint32_t arrayColumns;
	uint32_t arrayRows;
	uint32_t minX;
	uint32_t maxX;
	uint32_t minY;
	uint32_t maxY;
	#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
	uint32_t  **ledArray;
	#else
	uint8_t   **ledArray;
	#endif
} ledString_t;

/* Define the variable with the max number of strings possible */
ledString_t ledStrings[MAX_LED_STRINGS];

/* Initialize the numLedStrings to 0 until one is created */
uint8_t numLedStrings = 0;

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */

// Static Function Prototypes
static void ws2812HAL_DMAConfigure(uint8_t string);
static uint32_t ws2812HAL_convert3Code(uint8_t input);

uint32_t ws2812HAL_getNumLeds(uint8_t string);
void ws2812HAL_Start(void);
int8_t ws2812HAL_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds, uint32_t numRows, uint32_t numColumns, uint8_t ledLayout);
void ws2812HAL_updateString(uint8_t string);
void ws2812HAL_setPixelRGB(uint8_t string, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812HAL_setAllRGB(uint8_t string, uint8_t red, uint8_t green, uint8_t blue);

void ws2812_PixelColor(uint8_t stringNumber, int32_t x, int32_t y, uint32_t color);
uint32_t ws2812_GetPixelColor(uint8_t stringNumber, int32_t x, int32_t y);

/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: ws2812HAL_getNumLeds
********************************************************************************
* Summary:
*  Function This function returns the number of LEDs in the specified channel
*
* Parameters:
*  string: The string number to address
*
* Return:
*  number of LEDs in the string
*
*******************************************************************************/
uint32_t ws2812HAL_getNumLeds(uint8_t string)
{
	return ledStrings[string].numLeds;
}

/*******************************************************************************
* Function Name: WS_Start
********************************************************************************
* Summary:
*  Function This function is reserved for future use
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_Start(void)
{

}

/*******************************************************************************
* Function Name: WS_CreateString
********************************************************************************
* Summary:
*  Function This function is for creating a new String object
*
* Parameters:
*  GPIO Port, The pin for the MOSI signal and the number of LEDs in the string
*
* Return:
*  Index for the created string
*
*******************************************************************************/
int8_t ws2812HAL_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds, uint32_t numRows, uint32_t numColumns, uint8_t ledLayout)
{
	/* Declare the return value nd set to -1; Error*/
	uint8_t rval =- 1;

	/* Cycle through the lookup table to retrieve the HW settings for the port defined */
	for(uint8_t i = 0; i < numPossibleElements; i++)
	{
		if(ledStringTable[i].spiPort == spiPrt && ledStringTable[i].spiPin == spiPin)
		{
			/* If found in the lookup table */
			rval = i;
			ledStrings[numLedStrings].map = &ledStringTable[i];
			printf("HW channel %d found\r\n", i);
			break;
		}
	}
	/* Check if channel was successfully found */
	if(rval == -1)
	{
		/* If no channel found, return error */
		printf("No HW channel found\r\n");
		return rval;
	}
	else
	{
		/* Load passed parameters */
		ledStrings[numLedStrings].columns = numColumns;
		ledStrings[numLedStrings].rows = numRows;
		ledStrings[numLedStrings].numLeds = numLeds;
		ledStrings[numLedStrings].ledLayout = ledLayout;
		if(ledStrings[numLedStrings].ledLayout == ws2812_LED_LAYOUT_STANDARD)
		{
			ledStrings[numLedStrings].numLeds = ledStrings[numLedStrings].columns * ledStrings[numLedStrings].rows;
			ledStrings[numLedStrings].arrayColumns = ledStrings[numLedStrings].columns;
			ledStrings[numLedStrings].arrayRows = ledStrings[numLedStrings].rows;
			ledStrings[numLedStrings].minX = 0;
			ledStrings[numLedStrings].maxX = ledStrings[numLedStrings].columns - 1;
			ledStrings[numLedStrings].minY = 0;
			ledStrings[numLedStrings].maxY = ledStrings[numLedStrings].rows - 1;
		}
		else if(ledStrings[numLedStrings].ledLayout == ws2812_LED_LAYOUT_SPIRAL)
		{
			ledStrings[numLedStrings].rows = ((ledStrings[numLedStrings].numLeds + (ledStrings[numLedStrings].columns - 1))/(ledStrings[numLedStrings].columns));
			ledStrings[numLedStrings].arrayColumns = ledStrings[numLedStrings].columns + ledStrings[numLedStrings].rows;
			ledStrings[numLedStrings].arrayRows = 1;
			ledStrings[numLedStrings].minX = 0;
			ledStrings[numLedStrings].maxX = ledStrings[numLedStrings].columns - 1;
			ledStrings[numLedStrings].minY = 0;
			ledStrings[numLedStrings].maxY = ledStrings[numLedStrings].rows - 1;
		}
		else if(ledStrings[numLedStrings].ledLayout == ws2812_LED_LAYOUT_GRID16X16)
		{
			ledStrings[numLedStrings].arrayColumns = ledStrings[numLedStrings].columns * 256;
			ledStrings[numLedStrings].arrayRows = ledStrings[numLedStrings].rows;
			ledStrings[numLedStrings].columns = ledStrings[numLedStrings].columns * 16;
			ledStrings[numLedStrings].rows = ledStrings[numLedStrings].rows * 16;
			ledStrings[numLedStrings].numLeds = ledStrings[numLedStrings].columns * ledStrings[numLedStrings].rows;
			ledStrings[numLedStrings].minX = 0;
			ledStrings[numLedStrings].maxX = ledStrings[numLedStrings].columns - 1;
			ledStrings[numLedStrings].minY = 0;
			ledStrings[numLedStrings].maxY = ledStrings[numLedStrings].rows - 1;
		}
		/* If channel found, configure SPI and DMA accordingly */
		ledStrings[numLedStrings].spiContext = malloc(sizeof(cy_stc_scb_spi_context_t));
		if (ledStrings[numLedStrings].spiContext == NULL)
		{
			printf("Error creating SPI context %d\r\n", rval);
		}
		else
		{
			printf("SPI context %d created\r\n", rval);
		}
		ledStrings[numLedStrings].frameBufferSize = numLeds * WS_BYTES_PER_PIXEL + WS_ZOFFSET;
		ledStrings[numLedStrings].frameBuffer = malloc(ledStrings[numLedStrings].frameBufferSize);
		/* Check if frameBuffer created */
		if (ledStrings[numLedStrings].frameBuffer == NULL)
		{
			printf("Error creating frame buffer %d\r\n", rval);
		}
		else
		{
			/* Initialize the frameBuffer to all 0s*/
			memset(ledStrings[numLedStrings].frameBuffer, 0, ledStrings[numLedStrings].frameBufferSize);
			printf("frameBufferMemory %d Length is %lu\r\n", rval, ledStrings[numLedStrings].frameBufferSize);
		}
		/* Allocate the size of x, y array for the rows and columns of the string used */
		#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
		ledStrings[numLedStrings].ledArray = malloc(ledStrings[numLedStrings].rows * sizeof(uint32_t));
		if (ledStrings[numLedStrings].ledArray == NULL)
		{
			printf("Error creating ledArray buffer %d\r\n", numLedStrings);
		}
		else
		{
			memset(ledStrings[numLedStrings].ledArray, 0, (ledStrings[numLedStrings].rows * sizeof(uint32_t)));
			printf("ledArray Length is %lu\r\n", (ledStrings[numLedStrings].rows * sizeof(uint32_t)));
		}
		#else
		ledStrings[numLedStrings].ledArray = malloc(ledStrings[numLedStrings].rows * sizeof(uint8_t));
		memset(ledStrings[numLedStrings].ledArray, 0, (ledStrings[numLedStrings].rows * sizeof(uint8_t)));
		#endif
		for (uint32_t r = 0; r < ledStrings[numLedStrings].arrayRows; r++)
		{
			#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
			ledStrings[numLedStrings].ledArray[r] = malloc(ledStrings[numLedStrings].arrayColumns * sizeof(uint32_t));
			memset(ledStrings[numLedStrings].ledArray[r], 0, (ledStrings[numLedStrings].arrayColumns * sizeof(uint32_t)));
			#else
			ledStrings[numLedStrings].ledArray[r] = malloc(ledStrings[numLedStrings].arrayColumns * sizeof(uint8_t));
			memset(ledStrings[numLedStrings].ledArray[r], 0, (ledStrings[numLedStrings].arrayColumns * sizeof(uint8_t)));
			#endif
		}
		if (!ledStrings[numLedStrings].ledArray) printf("Error creating LED Array %d\r\n", numLedStrings);
		/* Declare and setup the DMA descriptors */
		ledStrings[numLedStrings].numDescriptors = ((numLeds * WS_BYTES_PER_PIXEL + WS_ZOFFSET)) / 256 + 1;
		ledStrings[numLedStrings].dmaDescr = malloc(sizeof(cy_stc_dma_descriptor_t) * ledStrings[numLedStrings].numDescriptors);
		/* Check if dmaDescr was created */
		if (ledStrings[numLedStrings].dmaDescr == NULL)
		{
			printf("Error creating DMA descriptors %d\r\n", rval);
		}
		else
		{
			/* Initialize the frameBuffer to all 0s*/
			memset(ledStrings[numLedStrings].dmaDescr, 0, sizeof(cy_stc_dma_descriptor_t) * ledStrings[numLedStrings].numDescriptors);
			printf("DMA descriptor %d Length is %lu\r\n", rval, sizeof(cy_stc_dma_descriptor_t) * ledStrings[numLedStrings].numDescriptors);
		}

		/* Configure SPI */
		const cy_stc_scb_spi_config_t spiConfig =
		{
			.spiMode = CY_SCB_SPI_MASTER,
			.subMode = CY_SCB_SPI_MOTOROLA,
			.sclkMode = CY_SCB_SPI_CPHA1_CPOL1,
			.oversample = 4,
			.rxDataWidth = 8UL,
			.txDataWidth = 8UL,
			.enableMsbFirst = true,
			.enableInputFilter = false,
			.enableFreeRunSclk = false,
			.enableMisoLateSample = true,
			.enableTransferSeperation = false,
			.ssPolarity = ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) | \
												 (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) | \
												 (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) | \
												 (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3)),
			.enableWakeFromSleep = false,
			.rxFifoTriggerLevel = 63UL,
			.rxFifoIntEnableMask = 0UL,
			.txFifoTriggerLevel = 63UL,
			.txFifoIntEnableMask = 0UL,
			.masterSlaveIntEnableMask = 0UL,
		};
		/* Initialize SPI channel */
		Cy_SCB_SPI_Init(ledStrings[numLedStrings].map->spiHw, &spiConfig, ledStrings[numLedStrings].spiContext);
		/* setup the SPI Clock */
		Cy_SysClk_PeriphAssignDivider(ledStrings[numLedStrings].map->spiClk, CY_SYSCLK_DIV_8_BIT, 1U); // ARH this needs a better clock assignment than a hard code
		/* Enable SPI Channel */
		Cy_SCB_SPI_Enable(ledStrings[numLedStrings].map->spiHw);

		/* Trigger Mux */
		Cy_TrigMux_Select(ledStrings[numLedStrings].map->outTrig, false, TRIGGER_TYPE_LEVEL);

		/* Configure SPI MOSI Pin */
		cy_stc_gpio_pin_config_t pinConfig =
		{
				.outVal = 1,
				.driveMode = CY_GPIO_DM_STRONG_IN_OFF,
				.hsiom = 0,
				.intEdge = CY_GPIO_INTR_DISABLE,
				.intMask = 0UL,
				.vtrip = CY_GPIO_VTRIP_CMOS,
				.slewRate = CY_GPIO_SLEW_FAST,
				.driveSel = CY_GPIO_DRIVE_FULL,
				.vregEn = 0UL,
				.ibufMode = 0UL,
				.vtripSel = 0UL,
				.vrefSel = 0UL,
				.vohSel = 0UL,
		};
		pinConfig.hsiom = ledStrings[numLedStrings].map->hsiom;
		/* Initialize Pin */
		Cy_GPIO_Pin_Init(ledStrings[numLedStrings].map->spiPort, ledStrings[numLedStrings].map->spiPin, &pinConfig);

		/* Configure and enable the DMA */
		ws2812HAL_DMAConfigure(numLedStrings);
		Cy_DMA_Enable(ledStrings[numLedStrings].map->dwHW);

		/* Increment the stringNumber for the next loop */
		numLedStrings += 1;

		/* Return the string number */
		return (numLedStrings - 1);
	}
}

/*******************************************************************************
* Function Name: ws2812HAL_DMAConfigure
********************************************************************************
* Summary:
*  Function This function configures the DMA
*
* Parameters:
*  String number
*
* Return:
*  Index for the created string
*
*******************************************************************************/

static void ws2812HAL_DMAConfigure(uint8_t string)
{
    /* I copied this structure from the PSoC Creator Component configuration
    	in generated source */
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

    for(uint8_t i = 0; i < ledStrings[string].numDescriptors; i++)
    {
        Cy_DMA_Descriptor_Init(&ledStrings[string].dmaDescr[i], &WS_DMA_Descriptors_config);
        Cy_DMA_Descriptor_SetSrcAddress(&ledStrings[string].dmaDescr[i], (uint8_t *)&ledStrings[string].frameBuffer[i * 256]);
        Cy_DMA_Descriptor_SetDstAddress(&ledStrings[string].dmaDescr[i], (void *)&ledStrings[string].map->spiHw->TX_FIFO_WR);
        Cy_DMA_Descriptor_SetXloopDataCount(&ledStrings[string].dmaDescr[i], 256); // the last
        Cy_DMA_Descriptor_SetNextDescriptor(&ledStrings[string].dmaDescr[i], &ledStrings[string].dmaDescr[i+1]);
    }

    /* The last one needs a bit of change */
    Cy_DMA_Descriptor_SetXloopDataCount(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1], ledStrings[string].frameBufferSize - 256 * (ledStrings[string].numDescriptors - 1));
    Cy_DMA_Descriptor_SetNextDescriptor(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1], 0);
    Cy_DMA_Descriptor_SetChannelState(&ledStrings[string].dmaDescr[ledStrings[string].numDescriptors-1], CY_DMA_CHANNEL_DISABLED);

}

/*******************************************************************************
* Function Name: ws2812HAL_updateString
********************************************************************************
* Summary:
*  Function This function sets up the DMA channel...
*  then enables it to dump the frameBuffer to pixels
*
* Parameters:
*  stringNumbner: The string number to address
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_updateString(uint8_t string)
{
	if((Cy_DMA_Channel_GetStatus(ledStrings[string].map->dwHW, ledStrings[string].map->channel) & CY_DMA_INTR_CAUSE_COMPLETION))
	{
		cy_stc_dma_channel_config_t channelConfig;
		channelConfig.descriptor  = &ledStrings[string].dmaDescr[0];
		channelConfig.preemptable = false;
		channelConfig.priority    = 3;
		channelConfig.enable      = false;
		Cy_DMA_Channel_Init(ledStrings[string].map->dwHW, ledStrings[string].map->channel, &channelConfig);
		Cy_DMA_Channel_Enable(ledStrings[string].map->dwHW, ledStrings[string].map->channel);
	}
}

/*******************************************************************************
* Function Name: ws2812HAL_convert3Code
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
static uint32_t ws2812HAL_convert3Code(uint8_t input)
{
    uint32_t rval = 0;
    for(uint32_t i = 0; i < 8; i++)
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
* Function Name: ws2812HAL_setPixelRGB
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
void ws2812HAL_setPixelRGB(uint8_t string, uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    WS_colorUnion color;
    color.word = ws2812HAL_convert3Code(green);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+1+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+2+WS_ZOFFSET] = color.bytes[0];

    color.word = ws2812HAL_convert3Code(red);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+3+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+4+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+5+WS_ZOFFSET] = color.bytes[0];

    color.word = ws2812HAL_convert3Code(blue);
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+6+WS_ZOFFSET] = color.bytes[2];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+7+WS_ZOFFSET] = color.bytes[1];
    ledStrings[string].frameBuffer[led*WS_BYTES_PER_PIXEL+8+WS_ZOFFSET] = color.bytes[0];
}

/*******************************************************************************
* Function Name: ws2812HAL_setAllRGB
********************************************************************************
* Summary:
*  Function This function sets the string to a three byte rgb value and
*  updates the WS_frameBuffer with the correct 9-bytes
*
* Parameters:
*  stringNumbner: The string number to address
*  red: Hex code for RED
*  green: Hex code for GREEN
*  blue: HEX code for BLUE
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_setAllRGB(uint8_t string, uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint32_t i = 0; i < ledStrings[string].numLeds; i++)
	{
		ws2812HAL_setPixelRGB(string, i, red, green, blue);
	}
}

/*******************************************************************************
* Function Name: ws2812_PixelColor
********************************************************************************
*
* Summary:
*  Draw Pixel
*
* Parameters:
*  x,y:    Location to draw the pixel
*  color:  Color of the pixel
*
* Return:
*  None
*******************************************************************************/
void ws2812_PixelColor(uint8_t stringNumber, int32_t x, int32_t y, uint32_t color)
{
    // Swap X-Y Coordinates
    #if(ws2812_SWAP_XY_COORD == ws2812_XY_SWAPED)
        uint32_t xyTmp;
        xyTmp = x;
        x = y;
        y = xyTmp;
    #endif

    // X-Wrap
    #if (ws2812_COORD_WRAP & ws2812_COORD_XAXIS )
        x = x % (ledStrings[stringNumber].maxX + 1);
    #endif

    // Y-Wrap
    #if (ws2812_COORD_WRAP & ws2812_COORD_YAXIS )
        y = y % (ledStrings[stringNumber].maxY + 1);
    #endif

    // Flip X-Axis
    #if (ws2812_FLIP_X_COORD == ws2812_FLIP_COORD )
        x = ledStrings[stringNumber].maxX - x;
    #endif

    // Flip Y-Axis
    #if (ws2812_FLIP_Y_COORD == ws2812_FLIP_COORD )
        y = ledStrings[stringNumber].maxY - y;
    #endif

    // Make sure X-Y values are in range
   	if((x >= ledStrings[stringNumber].minX) && (y >= ledStrings[stringNumber].minY) && (x <= ledStrings[stringNumber].maxX) && (y <= ledStrings[stringNumber].maxY))
    {

        #if (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_STANDARD)
             // Do nothing special
        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_SPIRAL)
            x = x + (y * ledStrings[stringNumber].Columns);
            y = 0;

        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_GRID16X16)
            	x = (((x/(int32_t)16)*(int32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
		        y = (y/(int32_t)16);
        #endif

        #if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
		        ledStrings[stringNumber].ledArray[y][x] = color;
        #else  /* Else use lookup table */
		        ledStrings[stringNumber].ledArray[y][x] = (uint8_t)color;
        #endif
    }
}

/*******************************************************************************
* Function Name: ws2812_GetPixelColor
********************************************************************************
*
* Summary:
*  Get Pixel Color
*
* Parameters:
*  x,y:    Location to get pixel color
*
* Return:
*  None
*******************************************************************************/
uint32_t ws2812_GetPixelColor(uint8_t stringNumber, int32_t x, int32_t y)
{
    uint32_t color = 0;
    #if (ws2812_COORD_WRAP & ws2812_COORD_XAXIS )
        x = x % (ws2812_MAX_X+1);
    #endif
    #if (ws2812_COORD_WRAP & ws2812_COORD_YAXIS )
        y = y % (ws2812_MAX_Y+1);
    #endif

    if((x>=0) && (y>=0) && (x < ledStrings[stringNumber].arrayColumns) && (y < ledStrings[stringNumber].arrayRows))
    {

        #if (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_STANDARD)

        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_SPIRAL)
            x = x + (y * ws2812_COLUMNS);
            y = 0;

        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_GRID16X16)
            	x = (((x/(int32_t)16)*(int32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
		        y = (y/(int32-t)16);
        #endif

		#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
		    color = ledStrings[stringNumber].ledArray[y][x];
        #else  /* Else use lookup table */
		    color = (uint32_t)ledStrings[stringNumber].ledArray[y][x];
        #endif

    }
    return(color);
}

