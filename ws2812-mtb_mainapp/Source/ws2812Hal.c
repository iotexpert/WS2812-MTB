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

		{GPIO_PRT1,0,P1_0_SCB7_SPI_MOSI,SCB7,PCLK_SCB7_CLOCK,DW1,10,TRIG_OUT_1TO1_1_SCB7_TX_TO_PDMA1_TR_IN10},


		{GPIO_PRT2,0,P2_0_SCB1_SPI_MOSI,SCB1,PCLK_SCB1_CLOCK,DW0,18,TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18},
		{GPIO_PRT3,0,P3_0_SCB2_SPI_MOSI,SCB2,PCLK_SCB2_CLOCK,DW0,20,TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20},
		{GPIO_PRT4,0,P4_0_SCB7_SPI_MOSI,SCB7,PCLK_SCB7_CLOCK,DW1,10,TRIG_OUT_1TO1_1_SCB7_TX_TO_PDMA1_TR_IN10},
		{GPIO_PRT5,0,P5_0_SCB5_SPI_MOSI,SCB5,PCLK_SCB5_CLOCK,DW0,26,TRIG_OUT_1TO1_0_SCB5_TX_TO_PDMA0_TR_IN26},

		{GPIO_PRT6,0,P6_0_SCB3_SPI_MOSI,SCB3,PCLK_SCB3_CLOCK,DW0,22,TRIG_OUT_1TO1_0_SCB3_TX_TO_PDMA0_TR_IN22},

		{GPIO_PRT6,4,P6_4_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},

		{GPIO_PRT7,0,P7_0_SCB4_SPI_MOSI,SCB4,PCLK_SCB4_CLOCK,DW0,24,TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24},
		{GPIO_PRT8,0,P8_0_SCB4_SPI_MOSI,SCB4,PCLK_SCB4_CLOCK,DW0,24,TRIG_OUT_1TO1_0_SCB4_TX_TO_PDMA0_TR_IN24},

		{GPIO_PRT9,0,P9_0_SCB2_SPI_MOSI,SCB2,PCLK_SCB2_CLOCK,DW0,20,TRIG_OUT_1TO1_0_SCB2_TX_TO_PDMA0_TR_IN20},
		{GPIO_PRT10,0,P10_0_SCB1_SPI_MOSI,SCB1,PCLK_SCB1_CLOCK,DW0,18,TRIG_OUT_1TO1_0_SCB1_TX_TO_PDMA0_TR_IN18},

		{GPIO_PRT11,0,P11_0_SCB5_SPI_MOSI,SCB5,PCLK_SCB5_CLOCK,DW0,26,TRIG_OUT_1TO1_0_SCB5_TX_TO_PDMA0_TR_IN26},

		{GPIO_PRT12,0,P12_0_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},
		{GPIO_PRT13,0,P13_0_SCB6_SPI_MOSI,SCB6,PCLK_SCB6_CLOCK,DW1,8,TRIG_OUT_1TO1_1_SCB6_TX_TO_PDMA1_TR_IN8},
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
	uint8_t coordWarp;
	uint32_t columns;
	uint32_t rows;
	uint32_t arrayColumns;
	uint32_t arrayRows;
	uint32_t minX;
	uint32_t maxX;
	uint32_t minY;
	uint32_t maxY;
	uint32_t  **ledArray;
} ledString_t;

#if( ws2812_GAMMA_CORRECTION == ws2812_GAMMA_ON )
const uint8_t ws2812_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
#endif

const uint32 ws2812_CLUT[ ] = {
//xxBBRRGG (WS2812)
//     (24)   [ Index = 0 ]
0x0000FFFF,  // 0 Yellow
0x0000CCFF,
0x000099FF,
0x000033FF,
0x000000FF,  // 5  Green
0x006600B3,
0x00990099,
0x00B30066,
0x00CC0033,  // 9 Blue
0x00B31919,
0x00993300,
0x00994000,
0x00996600,
0x00999900,
0x0099CC00,
0x0066E600,
0x0000FF00, // Red
0x0000FF33,
0x0000FF66,
0x0000FF80,
0x0000FF99,  // 20 Orange
0x0000FFB2,
0x0000FFCC,
0x0000FFE5,
// #24

//xxBBRRGG  (64)  [ Index = 24 ]
0x00000000, 0x00550000, 0x00AA0000, 0x00FF0000,  // 0, Black,  LtBlue, MBlue, Blue
0x00000055, 0x00550055, 0x00AA0055, 0x00FF0055,  // 4, LtGreen
0x000000AA, 0x005500AA, 0x00AA00AA, 0x00FF00AA,  // 8, MGreen
0x000000FF, 0x005500FF, 0x00AA00FF, 0x00FF00FF,  // 12 Green

0x00005500, 0x00555500, 0x00AA5500, 0x00FF5500,  // 16, LtRed
0x00005555, 0x00555555, 0x00AA5555, 0x00FF5555,  // 20, LtYellow
0x000055AA, 0x005555AA, 0x00AA55AA, 0x00FF55AA,  // 24,
0x000055FF, 0x005555FF, 0x00AA55FF, 0x00FF55FF,  // 28,

0x0000AA00, 0x0055AA00, 0x00AAAA00, 0x00FFAA00,  // 32, MRed
0x0000AA55, 0x0055AA55, 0x00AAAA55, 0x00FFAA55,  // 36,
0x0000AAAA, 0x0055AAAA, 0x00AAAAAA, 0x00FFAAAA,  // 55,
0x0000AAFF, 0x0055AAFF, 0x00AAAAFF, 0x00FFAAFF,  // 44,

0x0000FF00, 0x0055FF00, 0x00AAFF00, 0x00FFFF00,  // 48, Red, ??, ??, Magenta
0x0000FF55, 0x0055FF55, 0x00AAFF55, 0x00FFFF55,  // 52,
0x0000FFAA, 0x0055FFAA, 0x00AAFFAA, 0x00FFFFAA,  // 56,
0x0000FFFF, 0x0055FFFF, 0x00AAFFFF, 0x00FFFFFF,  // 60, Yellow,??, ??, White

// Misc ( 16 )  [ Index = 88 ]
0x000080FF,  // SPRING_GREEN
0x008000FF,  // TURQUOSE
0x00FF00FF,  // CYAN
0x00FF0080,  // OCEAN
0x00FF8000,  // VIOLET
0x0080FF00,  // RASPBERRY
0x000000FF,  // GREEN
0x00202020,  // DIM WHITE
0x00200000,  // DIM BLUE
0x10000000,  // INVISIBLE
0x0000FF00,  // Fire_Dark
0x0000FF30,  // Fire_Light  // 00FF80
0xFFFFFFFF,  // Full White
0xFF000000,  // LED_WHITE
0xFF808080,  // WHITE_RGB50
0xFF404040,  // WHITE_RGB25

// Temperture Color Blue to Red (16) [ Index = 104 ]
0x00FF0000, 0x00F01000, 0x00E02000, 0x00D03000,
0x00C04000, 0x00B05000, 0x00A06000, 0x00907000,
0x00808000, 0x00709000, 0x0060A000, 0x0050B000,
0x0040C000, 0x0030D000, 0x0020E000, 0x0000FF00
};

uint8_t const ws2812_cTrans[16][16] =  {
	/*  0  */  {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
    /*  1  */  { 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16},
	/*  2  */  { 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
	/*  3  */  { 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48},
	/*  4  */  { 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79},
	/*  5  */  { 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80},
	/*  6  */  { 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111},
	/*  7  */  {127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112},
	/*  8  */  {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143},
	/*  9  */  {159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144},
	/* 10  */  {160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175},
	/* 11  */  {191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176},
	/* 12  */  {192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207},
	/* 13  */  {223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208},
	/* 14  */  {224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239},
	/* 15  */  {255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240}};


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
int8_t ws2812HAL_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds, uint32_t numRows, uint32_t numColumns, uint8_t ledLayout, uint8_t coordWarp);
bool ws2812HAL_updateString(uint8_t string);
void ws2812HAL_DmaInterrupt(void);

void ws2812HAL_setPixelRGB(uint8_t string, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812HAL_setAllRGB(uint8_t string, uint8_t red, uint8_t green, uint8_t blue);
void ws2812HAL_setPixelColor(uint8_t string, uint32_t led, uint32_t color);
void ws2812HAL_setAllColor(uint8_t string, uint32_t color);
void ws2812HAL_setPixelArrayColor(uint8_t string, uint32_t y, uint32_t x, uint32_t color);
void ws2812HAL_setPixelArrayRGB(uint8_t string, uint32_t y, uint32_t x, uint8_t red, uint8_t green, uint8_t blue);

uint32_t ws2812_GetPixelColor(uint8_t stringNumber, int32_t x, int32_t y);

uint32_t ws2812HAL_getColor( uint32_t color);
uint32_t ws2812HAL_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct);
uint32_t ws2812HAL_ColorInc(uint32_t incValue);

/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: ws2812HAL_ColorInc
********************************************************************************
* Summary:
*  Increment color through the color lookup table.
*
* Parameters:
*  uint32 incValue: Increment through color table by incValue.
*
* Return: Color at next location.
*
*
*******************************************************************************/
uint32_t ws2812HAL_ColorInc(uint32_t incValue)
{
	/* Declare the variables used */
	uint32_t color;
	extern const uint32_t ws2812_CLUT[];
	/* Holds the index value between calls */
	static uint32_t colorIndex = 0;

	/* Increase the index value with the offset provided */
	colorIndex += incValue;
	colorIndex = colorIndex % ws2812_CWHEEL_SIZE;

	/* Retrieve and return the color code from the LUT */
	color = ws2812_CLUT[ colorIndex ];
    return(color);
}

/*******************************************************************************
* Function Name: ws2812HAL_getColor
********************************************************************************
*
* Summary:
*  Get Pixel Color from the LUT
*
* Parameters:
*  color index
*
* Return:
*  color code
*******************************************************************************/
uint32_t ws2812HAL_getColor( uint32_t color)
{
    color = ws2812_CLUT[color];
    return(color);
}

/*******************************************************************************
* Function Name: ws2812HAL_RgbBlend()
********************************************************************************
* Summary:  Blend two colors into one.
*
*    newColor = (pct * toColor)  + ((100-pct) * fromColor)
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
uint32_t ws2812HAL_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct)
{
    uint32_t newColor = 0;

    newColor |= (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
    newColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
    newColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;

   return(newColor);
}

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
int8_t ws2812HAL_CreateString(GPIO_PRT_Type *spiPrt, uint32_t spiPin, uint32_t numLeds, uint32_t numRows, uint32_t numColumns, uint8_t ledLayout, uint8_t coordWarp)
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
		for (uint32_t r = 0; r < ledStrings[numLedStrings].arrayRows; r++)
		{
			ledStrings[numLedStrings].ledArray[r] = malloc(ledStrings[numLedStrings].arrayColumns * sizeof(uint32_t));
			memset(ledStrings[numLedStrings].ledArray[r], 0, (ledStrings[numLedStrings].arrayColumns * sizeof(uint32_t)));
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

		/* Configure the DMA */
		ws2812HAL_DMAConfigure(numLedStrings);

		/* Configure the interrupt for the associated DMA channel */
		uint32_t dwintbase;

		if(ledStrings[numLedStrings].map->dwHW == DW0)
		{
			dwintbase = cpuss_interrupts_dw0_0_IRQn;
		}
		else
		{
			dwintbase = cpuss_interrupts_dw1_0_IRQn;
		}

		/* Set the DMA interrupt configuration */
		cy_stc_sysint_t dmaIntConfig = {
		.intrSrc      = dwintbase + ledStrings[numLedStrings].map->channel,
		.intrPriority = 7,
		};
		/* Initialize and enable the Interrupt */
		(void) Cy_SysInt_Init(&dmaIntConfig, &ws2812HAL_DmaInterrupt);
		NVIC_EnableIRQ(dmaIntConfig.intrSrc);
		/* Set the interrupt Mask */
		Cy_DMA_Channel_SetInterruptMask (ledStrings[numLedStrings].map->dwHW, ledStrings[numLedStrings].map->channel, CY_DMA_INTR_MASK);

		/* Enable the DMA */
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
* Function Name: ws2812HAL_DmaInterrupt
********************************************************************************
* Summary:
*  Function This function sets or clears the bit associated with the triggered
*  channel interrupt
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_DmaInterrupt(void)
{
	/* Loop through the led strings */
	for(int i = 0; i < numLedStrings; i++)
	{
		if(Cy_DMA_Channel_GetInterruptStatus(ledStrings[i].map->dwHW, ledStrings[i].map->channel))
		{
			//TODO: Set or clear the even bit
			Cy_DMA_Channel_ClearInterrupt(ledStrings[i].map->dwHW, ledStrings[i].map->channel);

		}
	}
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
bool ws2812HAL_updateString(uint8_t string)
{
	bool retValue = false;
	if((Cy_DMA_Channel_GetStatus(ledStrings[string].map->dwHW, ledStrings[string].map->channel) & CY_DMA_INTR_CAUSE_COMPLETION))
	{
		cy_stc_dma_channel_config_t channelConfig;
		channelConfig.descriptor  = &ledStrings[string].dmaDescr[0];
		channelConfig.preemptable = false;
		channelConfig.priority    = 3;
		channelConfig.enable      = false;
		Cy_DMA_Channel_Init(ledStrings[string].map->dwHW, ledStrings[string].map->channel, &channelConfig);
		Cy_DMA_Channel_Enable(ledStrings[string].map->dwHW, ledStrings[string].map->channel);
		retValue = true;
	}
	return retValue;
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
* Function Name: ws2812HAL_setPixelColor
********************************************************************************
* Summary:
*  Function This function sets the string to specified color
*
* Parameters:
*  stringNumbner: The string number to address
*  color: the requested color
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_setPixelColor(uint8_t string, uint32_t led, uint32_t color)
{
	typedef union {
		uint8_t bytes[4];
		uint32_t word;
	} WS_colorUnion;

	uint8_t red;
	uint8_t blue;
	uint8_t green;

	WS_colorUnion requestedColor;

	requestedColor.word = color;
	/* Configure command Parameters */
	red = requestedColor.bytes[1];
	blue = requestedColor.bytes[2];
	green = requestedColor.bytes[0];

	ws2812HAL_setPixelRGB(string, led, red, green, blue);
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
* Function Name: ws2812HAL_setAllColor
********************************************************************************
* Summary:
*  Function This function sets the string to specified color
*
* Parameters:
*  stringNumbner: The string number to address
*  color: the requested color
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_setAllColor(uint8_t string, uint32_t color)
{
	typedef union {
		uint8_t bytes[4];
		uint32_t word;
	} WS_colorUnion;

	uint8_t red;
	uint8_t blue;
	uint8_t green;

	WS_colorUnion requestedColor;

	requestedColor.word = color;
	/* Configure command Parameters */
	red = requestedColor.bytes[1];
	blue = requestedColor.bytes[2];
	green = requestedColor.bytes[0];

	ws2812HAL_setAllRGB(string, red, green, blue);
}

/*******************************************************************************
* Function Name: ws2812HAL_setPixelArrayColor
********************************************************************************
* Summary:
*  Function This function Takes a position and a color
*
* Parameters:
*  stringNumbner: The string number to address
*  y:	the row the LED is in
*  x: the column the LED is in
*  color: the color to be displayed
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_setPixelArrayColor(uint8_t string, uint32_t y, uint32_t x, uint32_t color)
{
	// Swap X-Y Coordinates
	if(ledStrings[string].coordWarp == ws2812_COORD_XYAXIS)
	{
		uint32_t xyTmp;
		xyTmp = x;
		x = y;
		y = xyTmp;
	}

	// X-Wrap
	if(ledStrings[string].coordWarp == ws2812_COORD_XAXIS )
	{
		x = x % (ledStrings[string].maxX + 1);
	}

	// Y-Wrap
	if(ledStrings[string].coordWarp == ws2812_COORD_YAXIS )
	{
		y = y % (ledStrings[string].maxY + 1);
	}

//	// Flip X-Axis
//	if(ledStrings[string].coordWarp == ws2812_FLIP_COORD )
//	{
//		x = ledStrings[string].maxX - x;
//	}
//
//	// Flip Y-Axis
//	if(ledStrings[string].coordWarp == ws2812_FLIP_COORD )
//	{
//		y = ledStrings[string].maxY - y;
//	}

	// Make sure X-Y values are in range
	if((x >= ledStrings[string].minX) && (y >= ledStrings[string].minY) && (x <= ledStrings[string].maxX) && (y <= ledStrings[string].maxY))
	{

		if(ledStrings[string].ledLayout == ws2812_LED_LAYOUT_STANDARD)
		{
			// Do nothing special
		}
		else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_SPIRAL)
		{
			x = x + (y * ledStrings[string].columns);
			y = 0;
		}
		else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_GRID16X16)
		{
			x = (((x/(int32_t)16)*(int32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
			y = (y/(int32_t)16);
		}

		ledStrings[string].ledArray[y][x] = color;
		ws2812HAL_setPixelColor(string, (y*x)+x, color );
	}
}

/*******************************************************************************
* Function Name: ws2812HAL_setPixelArrayRGB
********************************************************************************
* Summary:
*  Function This function Takes a position and a color
*
* Parameters:
*  stringNumbner: The string number to address
*  y:	the row the LED is in
*  x: the column the LED is in
*  red: Hex code for RED
*  green: Hex code for GREEN
*  blue: HEX code for BLUE
*
* Return:
*  void
*
*******************************************************************************/
void ws2812HAL_setPixelArrayRGB(uint8_t string, uint32_t y, uint32_t x, uint8_t red, uint8_t green, uint8_t blue)
{
	typedef union {
		uint8_t bytes[4];
		uint32_t word;
	} WS_colorUnion;

	WS_colorUnion requestedColor;

	/* Configure command Parameters */
	requestedColor.bytes[1] = red;
	requestedColor.bytes[2] = blue;
	requestedColor.bytes[0] = green;

	// Swap X-Y Coordinates
	if(ledStrings[string].coordWarp == ws2812_COORD_XYAXIS)
	{
		uint32_t xyTmp;
		xyTmp = x;
		x = y;
		y = xyTmp;
	}

	// X-Wrap
	if(ledStrings[string].coordWarp == ws2812_COORD_XAXIS )
	{
		x = x % (ledStrings[string].maxX + 1);
	}

	// Y-Wrap
	if(ledStrings[string].coordWarp == ws2812_COORD_YAXIS )
	{
		y = y % (ledStrings[string].maxY + 1);
	}

//	// Flip X-Axis
//	if(ledStrings[string].coordWarp == ws2812_FLIP_COORD )
//	{
//		x = ledStrings[string].maxX - x;
//	}
//
//	// Flip Y-Axis
//	if(ledStrings[string].coordWarp == ws2812_FLIP_COORD )
//	{
//		y = ledStrings[string].maxY - y;
//	}

	// Make sure X-Y values are in range
	if((x >= ledStrings[string].minX) && (y >= ledStrings[string].minY) && (x <= ledStrings[string].maxX) && (y <= ledStrings[string].maxY))
	{

		if(ledStrings[string].ledLayout == ws2812_LED_LAYOUT_STANDARD)
		{
			// Do nothing special
		}
		else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_SPIRAL)
		{
			x = x + (y * ledStrings[string].columns);
			y = 0;
		}
		else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_GRID16X16)
		{
			x = (((x/(int32_t)16)*(int32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
			y = (y/(int32_t)16);
		}

		ledStrings[string].ledArray[y][x] = requestedColor.word;
		ws2812HAL_setPixelRGB(string, (y*x)+x, red, green, blue );
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
uint32_t ws2812_GetPixelColor(uint8_t string, int32_t y, int32_t x)
{
    uint32_t color = 0;
    if(ledStrings[string].coordWarp == ws2812_COORD_XAXIS )
    {
        x = x % (ledStrings[string].maxX + 1);
    }
    if(ledStrings[string].coordWarp == ws2812_COORD_YAXIS )
    {
        y = y % (ledStrings[string].maxY + 1);
    }

    if((x>=0) && (y>=0) && (x < ledStrings[string].arrayColumns) && (y < ledStrings[string].arrayRows))
    {

       if(ledStrings[string].ledLayout == ws2812_LED_LAYOUT_STANDARD)
       {

       }
       else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_SPIRAL)
       {
            x = x + (y * ledStrings[string].columns);
            y = 0;
       }
       else if (ledStrings[string].ledLayout == ws2812_LED_LAYOUT_GRID16X16)
       {
            	x = (((x/(uint32_t)16)*(uint32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
		        y = (y/(int32_t)16);
       }
		    color = ledStrings[string].ledArray[y][x];
    }
    return(color);
}

