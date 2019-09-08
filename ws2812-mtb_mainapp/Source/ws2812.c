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

#if (ws2812_LED_LAYOUT_WS1 == ws2812_LED_LAYOUT_GRID16X16\
		|| ws2812_LED_LAYOUT_WS2 == ws2812_LED_LAYOUT_GRID16X16\
		|| ws2812_LED_LAYOUT_WS3 == ws2812_LED_LAYOUT_GRID16X16\
		|| ws2812_LED_LAYOUT_WS4 == ws2812_LED_LAYOUT_GRID16X16\
		|| ws2812_LED_LAYOUT_WS5 == ws2812_LED_LAYOUT_GRID16X16)
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
#endif


#if((ws2812_CHIP == ws2812_CHIP_WS2812) || (ws2812_CHIP == ws2812_CHIP_SF2812RGBW))
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
#else  //xxBBGGRR (WS2811)
const uint32 ws2812_CLUT[ ] = {
//     (24)   [ Index = 0 ]
0x0000FFFF,  // 0 Yellow
0x0000FFCC,
0x0000FF99,
0x0000FF33,
0x0000FF00,  // 5  Green
0x0066B300,
0x00999900,
0x00B36600,
0x00CC3300,  // 9 Blue
0x00B31919,
0x00990033,
0x00990040,
0x00990066,
0x00990099,
0x009900CC,
0x006600E6,
0x000000FF,
0x000033FF,
0x000066FF,
0x000080FF,
0x000080FF,  // 20 Orange
0x0000B2FF,
0x0000CCFF,
0x0000E5FF,
// #24

//xxBBGGRR  (64)  [ Index = 24 ]
0x00000000, 0x00550000, 0x00AA0000, 0x00FF0000,  // 0, Black,  LtBlue, MBlue, Blue
0x00005500, 0x00555500, 0x00AA5500, 0x00FF5500,  // 4, LtGreen
0x0000AA00, 0x0055AA00, 0x00AAAA00, 0x00FFAA00,  // 8, MGreen
0x0000FF00, 0x0055FF00, 0x00AAFF00, 0x00FFFF00,  // 12 Green

0x00000055, 0x00550055, 0x00AA0055, 0x00FF0055,  // 16, LtRed
0x00005555, 0x00555555, 0x00AA5555, 0x00FF5555,  // 20, LtYellow
0x0000AA55, 0x0055AA55, 0x00AAAA55, 0x00FFAA55,  // 24,
0x0000FF55, 0x0055FF55, 0x00AAFF55, 0x00FFFF55,  // 28,

0x000000AA, 0x005500AA, 0x00AA00AA, 0x00FF00AA,  // 32, MRed
0x000055AA, 0x005555AA, 0x00AA55AA, 0x00FF55AA,  // 36,
0x0000AAAA, 0x0055AAAA, 0x00AAAAAA, 0x00FFAAAA,  // 55,
0x0000FFAA, 0x0055FFAA, 0x00AAFFAA, 0x00FFFFAA,  // 44,

0x000000FF, 0x005500FF, 0x00AA00FF, 0x00F00FFF,  // 48, Red, ??, ??, Magenta
0x000055FF, 0x005555FF, 0x00AA55FF, 0x00FF55FF,  // 52,
0x0000AAFF, 0x0055AAFF, 0x00AAAAFF, 0x00FFAAFF,  // 56,
0x0000FFFF, 0x0055FFFF, 0x00AAFFFF, 0x00FFFFFF,  // 60, Yellow,??, ??, White

// Misc ( 16 )  [ Index = 88 ]   //xxBBGGRR
0x0000FF80,  // SPRING_GREEN
0x0080FF00,  // TURQUOSE
0x00FFFF00,  // CYAN
0x00FF8000,  // OCEAN
0x00FF0080,  // VIOLET
0x008000FF,  // RASPBERRY
0x0000FF00,  // GREEN
0x00202020,  // DIM WHITE
0x00200000,  // DIM BLUE
0x10000000,  // INVISIBLE
0x000000FF,  // Fire_Dark
0x000030FF,  // Fire_Light  // 0080FF
0xFFFFFFFF,  // Full White
0xFF000000,  // White_LED
0xFF808080,  // WHITE_RGB50
0xFF404040,  // WHITE_RGB25

// Temperture Color Blue to Red (16) [ Index = 104 ]
0x00FF0000, 0x00F00010, 0x00E00020, 0x00D00030,
0x00C00040, 0x00B00050, 0x00A00060, 0x00900070,
0x00800080, 0x00700090, 0x006000A0, 0x005000B0,
0x004000C0, 0x003000D0, 0x002000E0, 0x000000FF
};
#endif

#if(ws2812_CHIP == ws2812_CHIP_WS2812)
#define ws2812_RED_MASK   0x0000FF00
#define ws2812_GREEN_MASK 0x000000FF
#define ws2812_BLUE_MASK  0x00FF0000
#else
#define ws2812_RED_MASK   0x000000FF
#define ws2812_GREEN_MASK 0x0000FF00
#define ws2812_BLUE_MASK  0x00FF0000
#endif

#define ws2812_YELLOW      ws2812_getColor(1)
#define ws2812_GREEN       ws2812_getColor((70 + ws2812_CWHEEL_SIZE))
#define ws2812_ORANGE      ws2812_getColor(20)
#define ws2812_BLACK       ws2812_getColor((0 + ws2812_CWHEEL_SIZE))
#define ws2812_OFF         ws2812_getColor((0 + ws2812_CWHEEL_SIZE))
#define ws2812_LTBLUE      ws2812_getColor((1 + ws2812_CWHEEL_SIZE))
#define ws2812_MBLUE       ws2812_getColor((2 + ws2812_CWHEEL_SIZE))
#define ws2812_BLUE        ws2812_getColor((3 + ws2812_CWHEEL_SIZE))
#define ws2812_LTGREEN     ws2812_getColor((4 + ws2812_CWHEEL_SIZE))
#define ws2812_MGREEN      ws2812_getColor((8 + ws2812_CWHEEL_SIZE))
//#define ws2812_GREEN       (12 + ws2812_CWHEEL_SIZE)
#define ws2812_LTRED       ws2812_getColor((16 + ws2812_CWHEEL_SIZE))
#define ws2812_LTYELLOW    ws2812_getColor((20 + ws2812_CWHEEL_SIZE))
#define ws2812_MGRED       ws2812_getColor((32 + ws2812_CWHEEL_SIZE))
#define ws2812_RED         ws2812_getColor((48 + ws2812_CWHEEL_SIZE))
#define ws2812_MAGENTA     ws2812_getColor((51 + ws2812_CWHEEL_SIZE))
#define ws2812_RGB_WHITE   ws2812_getColor((63 + ws2812_CWHEEL_SIZE))

#define ws2812_SPRING_GREEN ws2812_getColor((64 + ws2812_CWHEEL_SIZE))
#define ws2812_TURQUOSE    ws2812_getColor((65 + ws2812_CWHEEL_SIZE))
#define ws2812_CYAN        ws2812_getColor((66 + ws2812_CWHEEL_SIZE))
#define ws2812_OCEAN       ws2812_getColor((67 + ws2812_CWHEEL_SIZE))
#define ws2812_VIOLET      ws2812_getColor((68 + ws2812_CWHEEL_SIZE))
#define ws2812_RASPBERRY   ws2812_getColor((69 + ws2812_CWHEEL_SIZE))
#define ws2812_DIM_WHITE   ws2812_getColor((71 + ws2812_CWHEEL_SIZE))
#define ws2812_DIM_BLUE    ws2812_getColor((72 + ws2812_CWHEEL_SIZE))
#define ws2812_INVISIBLE   ws2812_getColor((73 + ws2812_CWHEEL_SIZE))

#define ws2812_COLD_TEMP   ws2812_getColor((80 + ws2812_CWHEEL_SIZE))
#define ws2812_HOT_TEMP    ws2812_getColor((95 + ws2812_CWHEEL_SIZE))

#define ws2812_FIRE_DARK   ws2812_getColor((74 + ws2812_CWHEEL_SIZE))
#define ws2812_FIRE_LIGHT  ws2812_getColor((75 + ws2812_CWHEEL_SIZE))

#define ws2812_FULL_WHITE  ws2812_getColor((76 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_LED   ws2812_getColor((77 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_RGB50 ws2812_getColor((78 + ws2812_CWHEEL_SIZE))
#define ws2812_WHITE_RGB25 ws2812_getColor((79 + ws2812_CWHEEL_SIZE))

#define ws2812_CLUT_SIZE  (96 + ws2812_CWHEEL_SIZE)

#if(ws2812_CHIP == ws2812_CHIP_SF2812RGBW)
    #define ws2812_WHITE   ws2812_getColor((77 + ws2812_CWHEEL_SIZE))
#else
    #define ws2812_WHITE   ws2812_getColor((63 + ws2812_CWHEEL_SIZE))
#endif

/* Declare queue for the WS2812 Task */
QueueHandle_t ws2812QueueHandle = NULL;

/* Declare the LED refresh timer handle */
TimerHandle_t ws2812TimerHandle = NULL;

/* Declare the LED Auto-Update flag and set the default to true */
bool wsAutoUpdateState = false;

uint32_t  ws2812_ledIndex = 0;
uint32_t  ws2812_row = 0;
uint32_t  ws2812_refreshComplete;

uint32_t  ws2812_DimMask;
uint32_t  ws2812_DimShift;

/* Declare the channel configuration array */
channelConfig_t channelConfig[ws2812_NUMBER_OF_CHANNELS];

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
void configure_LedChannels(void);
void WS_DMATrigger(uint8_t stringNumber);
void ws2812_update(uint8_t stringNumber);
void ws2812_autoUpdate(bool option);

void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_initMixColorRGB(uint8_t stringNumber);
uint32_t ws2812_ColorInc(uint32_t incValue);
void ws2812_MemClear(uint8_t stringNumber, uint32_t color);
void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color);
void ws2812_Pixel(uint8_t stringNumber, int32_t x, int32_t y, uint32_t color);
uint32_t ws2812_GetPixel(uint8_t stringNumber, int32_t x, int32_t y);
uint32_t ws2812_getColor( uint32_t color);
void ws2812_Dim(uint32 dimLevel);
uint32_t ws2812_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct);
uint32_t WS_convert3Code(uint8_t input);
void WS_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void WS_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
void WS_initMixColorRGB(uint8_t stringNumber);

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
void configure_LedChannels(void)
{
	/* I copies this structure from the PSoC Creator Component configuration
		    	in generated source */
	    const cy_stc_dma_descriptor_config_t DMA_Descriptors_config =
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


	/* User: Configure basic element of Channel 0 */
	channelConfig[0].enable = 0;
	channelConfig[0].ledLayout = ws2812_LED_LAYOUT_STANDARD;
	channelConfig[0].totalLeds = 24;
	channelConfig[0].columns = 24;
	channelConfig[0].rows = 1;
	channelConfig[0].scbHW = WS0_SPI_HW;
	channelConfig[0].dmaHW = WS0_DMA_HW;
	channelConfig[0].dmaChannel = WS0_DMA_CHANNEL;
	/* User: Configure basic element of Channel 1 */
	channelConfig[1].enable = 0;
	channelConfig[1].ledLayout = ws2812_LED_LAYOUT_STANDARD;
	channelConfig[1].totalLeds = 24;
	channelConfig[1].columns = 24;
	channelConfig[1].rows = 1;
	channelConfig[1].scbHW = WS1_SPI_HW;
	channelConfig[1].dmaHW = WS1_DMA_HW;
	channelConfig[1].dmaChannel = WS1_DMA_CHANNEL;
	/* User: Configure basic element of Channel 2 */
	channelConfig[2].enable = 0;
	channelConfig[2].ledLayout = ws2812_LED_LAYOUT_STANDARD;
	channelConfig[2].totalLeds = 24;
	channelConfig[2].columns = 24;
	channelConfig[2].rows = 1;
	channelConfig[2].scbHW = WS2_SPI_HW;
	channelConfig[2].dmaHW = WS2_DMA_HW;
	channelConfig[2].dmaChannel = WS2_DMA_CHANNEL;
	/* User: Configure basic element of Channel 3 */
	channelConfig[3].enable = 0;
	channelConfig[3].ledLayout = ws2812_LED_LAYOUT_STANDARD;
	channelConfig[3].totalLeds = 24;
	channelConfig[3].columns = 24;
	channelConfig[3].rows = 1;
	channelConfig[3].scbHW = WS3_SPI_HW;
	channelConfig[3].dmaHW = WS3_DMA_HW;
	channelConfig[3].dmaChannel = WS3_DMA_CHANNEL;
	/* User: Configure basic element of Channel 4 */
	channelConfig[4].enable = 1;
	channelConfig[4].ledLayout = ws2812_LED_LAYOUT_STANDARD;
	channelConfig[4].totalLeds = 24;
	channelConfig[4].columns = 24;
	channelConfig[4].rows = 1;
	channelConfig[4].scbHW = WS4_SPI_HW;
	channelConfig[4].dmaHW = WS4_DMA_HW;
	channelConfig[4].dmaChannel = WS4_DMA_CHANNEL;
	/* Configure the rest of the struct for each channel. Only if the channel is Enabled */
	for(uint8_t i = 0; i < ws2812_NUMBER_OF_CHANNELS; i++)
	{
		if(channelConfig[i].enable == 1)
		{
			if(channelConfig[i].ledLayout == ws2812_LED_LAYOUT_STANDARD)
			{
				channelConfig[i].totalLeds = channelConfig[i].columns * channelConfig[i].rows;
				channelConfig[i].arrayColumns = channelConfig[i].columns;
				channelConfig[i].arrayRows = channelConfig[i].rows;
				channelConfig[i].minX = 0;
				channelConfig[i].maxX = channelConfig[i].columns - 1;
				channelConfig[i].minY = 0;
				channelConfig[i].maxY = channelConfig[i].rows - 1;
			}
			else if(channelConfig[i].ledLayout == ws2812_LED_LAYOUT_SPIRAL)
			{
				channelConfig[i].rows = ((channelConfig[i].totalLeds + (channelConfig[i].columns - 1))/(channelConfig[i].columns));
				channelConfig[i].arrayColumns = channelConfig[i].columns + channelConfig[i].rows;
				channelConfig[i].arrayRows = 1;
				channelConfig[i].minX = 0;
				channelConfig[i].maxX = channelConfig[i].columns - 1;
				channelConfig[i].minY = 0;
				channelConfig[i].maxY = channelConfig[i].rows - 1;
			}
			else if(channelConfig[i].ledLayout == ws2812_LED_LAYOUT_GRID16X16)
			{
				channelConfig[i].arrayColumns = channelConfig[i].columns * 256;
				channelConfig[i].arrayRows = channelConfig[i].rows;
				channelConfig[i].columns = channelConfig[i].columns * 16;
				channelConfig[i].rows = channelConfig[i].rows * 16;
				channelConfig[i].totalLeds = channelConfig[i].columns * channelConfig[i].rows;
				channelConfig[i].minX = 0;
				channelConfig[i].maxX = channelConfig[i].columns - 1;
				channelConfig[i].minY = 0;
				channelConfig[i].maxY = channelConfig[i].rows - 1;
			}
			/* Define the buffer length to hold the data for the string of LEDs */
			channelConfig[i].frameBufferLength = (channelConfig[i].totalLeds * WS_BYTES_PER_PIXEL + WS_ZOFFSET);
			/* Allocate the data to the frame buffer then set the values to 0 */
			channelConfig[i].frameBuffer = malloc(channelConfig[i].frameBufferLength * sizeof(uint8_t));
			if (!channelConfig[i].frameBuffer) printf("Error creating frame buffer %d\r\n", i);
			memset(channelConfig[i].frameBuffer, 0, (channelConfig[i].frameBufferLength * sizeof(uint8_t)));
			/* Calculate how many descriptors we need based on the number of LEDs per string */
			channelConfig[i].numberDmaDescriptors = (sizeof(channelConfig[i].frameBuffer) / 256 + 1);
			/* Allocate the size to the DMA descriptor array */
			channelConfig[i].dmaDescriptors = malloc((channelConfig[i].numberDmaDescriptors) * sizeof(cy_stc_dma_descriptor_t));
			/* Allocte the size of x, y array for the rows and columns of the string used */
			#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
			channelConfig[i].ledArray = malloc(channelConfig[i].rows * sizeof(uint32_t));
			memset(channelConfig[i].ledArray, 0, (channelConfig[i].rows * sizeof(uint32_t)));
			#else
			channelConfig[i].ledArray = malloc(channelConfig[i].rows * sizeof(uint8_t));
			memset(channelConfig[i].ledArray, 0, (channelConfig[i].rows * sizeof(uint8_t)));
			#endif
			for (uint32_t r = 0; r < channelConfig[i].arrayRows; r++)
			{
				#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
				channelConfig[i].ledArray[r] = malloc(channelConfig[i].arrayColumns * sizeof(uint32_t));
				memset(channelConfig[i].ledArray[r], 0, (channelConfig[i].arrayColumns * sizeof(uint32_t)));
				#else
				channelConfig[i].ledArray[r] = malloc(channelConfig[i].arrayColumns * sizeof(uint8_t));
				memset(channelConfig[i].ledArray[r], 0, (channelConfig[i].arrayColumns * sizeof(uint8_t)));
				#endif
			}
			if (!channelConfig[i].ledArray) printf("Error creating LED Array %d\r\n", i);
			/* Configure the DMA for each channel */
			for(uint32_t j = 0; j < channelConfig[i].numberDmaDescriptors; j++)
			{
				Cy_DMA_Descriptor_Init(&channelConfig[i].dmaDescriptors[j], &DMA_Descriptors_config);
				Cy_DMA_Descriptor_SetSrcAddress(&channelConfig[i].dmaDescriptors[j], (uint8_t *)&channelConfig[i].frameBuffer[j * 256]);
				Cy_DMA_Descriptor_SetDstAddress(&channelConfig[i].dmaDescriptors[j], (void *)&channelConfig[i].scbHW->TX_FIFO_WR);
				Cy_DMA_Descriptor_SetXloopDataCount(&channelConfig[i].dmaDescriptors[j], 256); // the last
				Cy_DMA_Descriptor_SetNextDescriptor(&channelConfig[i].dmaDescriptors[j], &channelConfig[i].dmaDescriptors[j + 1]);
			}
			/* The last one needs a bit of change */
			Cy_DMA_Descriptor_SetXloopDataCount(&channelConfig[i].dmaDescriptors[channelConfig[i].numberDmaDescriptors - 1], sizeof(channelConfig[i].frameBuffer) - 256 * (channelConfig[i].numberDmaDescriptors - 1)); // the last
			Cy_DMA_Descriptor_SetNextDescriptor(&channelConfig[i].dmaDescriptors[channelConfig[i].numberDmaDescriptors - 1], 0);
			Cy_DMA_Descriptor_SetChannelState(&channelConfig[i].dmaDescriptors[channelConfig[i].numberDmaDescriptors - 1], CY_DMA_CHANNEL_DISABLED);
			/* Enable the DMA channel */
			Cy_DMA_Enable(channelConfig[i].dmaHW);
		}
	}
}

/*******************************************************************************
* Function Name: WS_DMATrigger
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
void WS_DMATrigger(uint8_t stringNumber)
{
	if(channelConfig[stringNumber].enable == 1)
	{
		/* Declare the DMA channelConfig*/
		cy_stc_dma_channel_config_t dmaChannelConfig;
		/* Configure the DM channel settings for the passed stringNumber*/
		dmaChannelConfig.descriptor  = &channelConfig[stringNumber].dmaDescriptors[0];
		dmaChannelConfig.preemptable = false;
		dmaChannelConfig.priority    = 3;
		dmaChannelConfig.enable      = false;
		Cy_DMA_Channel_Init(channelConfig[stringNumber].dmaHW, channelConfig[stringNumber].dmaChannel, &dmaChannelConfig);
		Cy_DMA_Channel_Enable(channelConfig[stringNumber].dmaHW, channelConfig[stringNumber].dmaChannel);
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
uint32_t WS_convert3Code(uint8_t input)
{
    uint32_t rval=0;
    for(uint8_t i = 0; i < 8; i++)
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
void WS_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    WS_colorUnion color;

    color.word = WS_convert3Code(green);
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + WS_ZOFFSET] = color.bytes[2];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 1 + WS_ZOFFSET] = color.bytes[1];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 2 + WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(red);
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 3 + WS_ZOFFSET] = color.bytes[2];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 4 + WS_ZOFFSET] = color.bytes[1];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 5 + WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(blue);
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 6 + WS_ZOFFSET] = color.bytes[2];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 7 + WS_ZOFFSET] = color.bytes[1];
    channelConfig[stringNumber].frameBuffer[led * WS_BYTES_PER_PIXEL + 8 + WS_ZOFFSET] = color.bytes[0];
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
void WS_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue)
{
    CY_ASSERT(start >= 0);
    CY_ASSERT(start < end);

    CY_ASSERT(end <= channelConfig[stringNumber].totalLeds - 1);

    WS_setRGB(stringNumber, start, red, green, blue);
    for(uint32_t i = 1; i <= end - start; i++)
    {
    	memcpy(&channelConfig[stringNumber].frameBuffer[start * WS_BYTES_PER_PIXEL + i * WS_BYTES_PER_PIXEL + WS_ZOFFSET],
    			&channelConfig[stringNumber].frameBuffer[start * WS_BYTES_PER_PIXEL + WS_ZOFFSET], WS_BYTES_PER_PIXEL);
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
	(void)arg;

	/* Declare message struct. */
	ws2812_msg_t msg;

	/* Declare the SPI channel context */
	cy_stc_scb_spi_context_t SPI_context;

	const cy_stc_scb_spi_config_t SPI_config =
	{
		.spiMode = CY_SCB_SPI_MASTER,
		.subMode = CY_SCB_SPI_MOTOROLA,
		.sclkMode = CY_SCB_SPI_CPHA1_CPOL1,
		.oversample = 10,
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

	printf("Starting WS2812 task\r\n");
	/* Run the LED channel configuration */
	configure_LedChannels();
	printf("All channels configured\r\n");
	/* Initialize and start the SPI blocks for each enabled channel */
	for(uint8_t i = 0; i < ws2812_NUMBER_OF_CHANNELS; i++)
	{
		if(channelConfig[i].enable == 1)
		{
			channelConfig[i].frameBuffer[0] = 0x00;
			/* Initialize all LEDs Off and update the DMA trigger to load */
			Cy_SCB_SPI_Init(channelConfig[i].scbHW, &SPI_config, &SPI_context);
			Cy_SCB_SPI_Enable(channelConfig[i].scbHW);
			printf("Channel %d: SPI block enabled\r\n", i);
		}
	}

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
					WS_DMATrigger(msg.stringNumber);
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
void WS_initMixColorRGB(uint8_t stringNumber)
{
    for(uint32_t i = 0; i < channelConfig[stringNumber].totalLeds; i++)
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
* Function Name: ws2812_ColorInc
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
uint32_t ws2812_ColorInc(uint32_t incValue)
{
	uint32_t color;
	extern const uint32_t ws2812_CLUT[];
	static uint32_t colorIndex = 0;

	colorIndex += incValue;
	colorIndex = colorIndex % ws2812_COLOR_WHEEL_SIZE;

#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
	color = ws2812_CLUT[ colorIndex ];
#else
	color = colorIndex;
#endif

    return(color);
}

/*******************************************************************************
* Function Name: ws2812_MemClear
********************************************************************************
* Summary:
*   Clear LED memory with given color, but do not update display.
*
* Parameters:
*  uint32 color: Color to clear display.
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_MemClear(uint8_t stringNumber, uint32_t color)
{
    uint32_t  row, col;

    for(row = 0; row < channelConfig[stringNumber].arrayRows; row++)
    {
    	for(col=0; col < channelConfig[stringNumber].arrayColumns; col++)
    	{
			#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
    		channelConfig[stringNumber].ledArray[row][col] = color;
			#else  /* Else use lookup table */
    		channelConfig[stringNumber].ledArray[row][col] = (uint8_t)color;
			#endif
    	}
    }
}

/*******************************************************************************
* Function Name: ws2812_DisplayClear
********************************************************************************
* Summary:
*   Clear memory with a given value and update the display.
*
* Parameters:
*  uint32 color: Color to clear display.
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color)
{
    ws2812_MemClear(stringNumber, color);
    WS_DMATrigger(stringNumber);
}

/*******************************************************************************
* Function Name: ws2812_Pixel
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
void ws2812_Pixel(uint8_t stringNumber, int32_t x, int32_t y, uint32_t color)
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
        x = x % (ws2812_MAX_X+1);
    #endif

    // Y-Wrap
    #if (ws2812_COORD_WRAP & ws2812_COORD_YAXIS )
        y = y % (ws2812_MAX_Y+1);
    #endif

    // Flip X-Axis
    #if (ws2812_FLIP_X_COORD == ws2812_FLIP_COORD )
        x = ws2812_MAX_X - x;
    #endif

    // Flip Y-Axis
    #if (ws2812_FLIP_Y_COORD == ws2812_FLIP_COORD )
        y = ws2812_MAX_Y - y;
    #endif

    // Make sure X-Y values are in range
   	if((x >= channelConfig[stringNumber].minX) && (y >= channelConfig[stringNumber].minY) && (x <= channelConfig[stringNumber].maxX) && (y <= channelConfig[stringNumber].maxY))
    {

        #if (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_STANDARD)
             // Do nothing special
        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_SPIRAL)
            x = x + (y * ws2812_COLUMNS);
            y = 0;

        #elif (ws2812_LED_LAYOUT == ws2812_LED_LAYOUT_GRID16X16)
            	x = (((x/(int32_t)16)*(int32_t)256) + (uint32_t)ws2812_cTrans[y % 16][x % 16]);
		        y = (y/(int32_t)16);
        #endif

        #if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
		    channelConfig[stringNumber].ledArray[y][x] = color;
        #else  /* Else use lookup table */
		    channelConfig[stringNumber].ledArray[y][x] = (uint8_t)color;
        #endif
    }
}

/*******************************************************************************
* Function Name: ws2812_GetPixel
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
uint32_t ws2812_GetPixel(uint8_t stringNumber, int32_t x, int32_t y)
{
    uint32_t color = 0;
    #if (ws2812_COORD_WRAP & ws2812_COORD_XAXIS )
        x = x % (ws2812_MAX_X+1);
    #endif
    #if (ws2812_COORD_WRAP & ws2812_COORD_YAXIS )
        y = y % (ws2812_MAX_Y+1);
    #endif

    if((x>=0) && (y>=0) && (x < channelConfig[stringNumber].arrayColumns) && (y < channelConfig[stringNumber].arrayRows))
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
		    color = channelConfig[stringNumber].ledArray[y][x];
        #else  /* Else use lookup table */
		    color = (uint32_t)channelConfig[stringNumber].ledArray[y][x];
        #endif

    }
    return(color);
}

/*******************************************************************************
* Function Name: ws2812_getColor
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
uint32_t ws2812_getColor( uint32_t color)
{
    #if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
    color = ws2812_CLUT[color];
    #endif

    return(color);
}

/*******************************************************************************
* Function Name: ws2812_Dim
********************************************************************************
*
* Summary:
*  Dim all output by a specific level (0,1,2,3,4)
*
* Parameters:
*  dimLevel:  Dim level 1 to 4, 0 => No dimming.
*
* Return:
*  None
*
*******************************************************************************/
void ws2812_Dim(uint32 dimLevel)
{
	extern uint32_t  ws2812_DimMask;
	extern uint32_t  ws2812_DimShift;

    switch(dimLevel)
    {
       case 1:  // 1/2 bright
           ws2812_DimMask = 0x7F7F7F7F;
           ws2812_DimShift = 1;
           break;

       case 2:
           ws2812_DimMask = 0x3F3F3F3F;
           ws2812_DimShift = 2;
           break;

       case 3:
           ws2812_DimMask = 0x1F1F1F1F;
           ws2812_DimShift = 3;
           break;

       case 4:
           ws2812_DimMask = 0x0F0F0F0F;
           ws2812_DimShift = 4;
           break;

       default:
           ws2812_DimMask = 0xFFFFFFFF;
           ws2812_DimShift = 0;
           break;
    }
}

/*******************************************************************************
* Function Name: ws2812_RgbBlend()
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
uint32_t ws2812_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct)
{
    uint32_t newColor = 0;

    #if(ws2812_CHIP == ws2812_CHIP_SF2812RGBW)
        uint32_t tc, fc;

        tc = toColor >> 8;
        fc = fromColor >> 8;
        newColor = ((((pct * (tc & 0x00FF0000)) + ((100-pct) * (fc & 0x00FF0000)))/100) & 0x00FF0000)<<8;
    #endif
    newColor |= (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
    newColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
    newColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;

   return(newColor);
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
	for(uint8_t i = 0; i < ws2812_NUMBER_OF_CHANNELS; i++)
	{
		if((Cy_DMA_Channel_GetStatus(channelConfig[i].dmaHW, channelConfig[i].dmaChannel) & CY_DMA_INTR_CAUSE_COMPLETION))
		{
			WS_DMATrigger(i);
		}
	}
}
