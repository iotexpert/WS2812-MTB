/** __Code for the WS2812 Example Application_
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
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>
#include <stdlib.h>
#include "ws2812.h"
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
#define ws2812_RED_MASK   0x0000FF00
#define ws2812_GREEN_MASK 0x000000FF
#define ws2812_BLUE_MASK  0x00FF0000

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

//#define ws2812_WHITE   ws2812_getColor((63 + ws2812_CWHEEL_SIZE))


/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */
/* Global variables definitions go here */
uint32_t  ws2812_ledIndex = 0;
uint32_t  ws2812_row = 0;
uint32_t  ws2812_refreshComplete;

uint32_t  ws2812_DimMask;
uint32_t  ws2812_DimShift;

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


/* Declare the array to hold the number of LEDs per string */
uint8_t numOfLeds[MAX_LED_STRINGS] = {0};

/* ==================================================================== */
/* ============================== data ================================ */
/* ==================================================================== */
/* Definition of datatypes go here */


/* ==================================================================== */
/* ==================== function prototypes =========================== */
/* ==================================================================== */
/* Function prototypes for public (external) functions go here */
uint32_t ws2812_ColorInc(uint32_t incValue);
void ws2812_FadeInFadeOutColor(uint8_t stringNumber, uint32_t color, bool fadeIn, bool fadeOut);
void ws2812_FadeInFadeOutRGB(uint8_t stringNumber, uint8_t red, uint8_t blue, uint8_t green, bool fadeIn, bool fadeOut);
void ws2812_MixColorRGB(uint32_t stringNumber);
void ws2812_StrobeRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
void ws2812_StrobeColor(uint8_t stringNumber, uint32_t colorRequested, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause);
void ws2812_NewKITT(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_CenterToOutside(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_OutsideToCenter(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_LeftToRight(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_RightToLeft(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay);
void ws2812_HalloweenEyesRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeWidth, uint32_t EyeSpace,
		bool Fade, uint32_t Steps, uint32_t FadeDelay, uint32_t EndPause);
void ws2812_TwinkleRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne);

void ws2812_DisplayClear(uint8_t stringNumber, uint32_t color);
uint32_t ws2812_getColor( uint32_t color);
uint32_t ws2812_RgbBlend(uint32_t fromColor, uint32_t toColor, uint32_t pct);



/* ==================================================================== */
/* ============================ functions ============================= */
/* ==================================================================== */
/*******************************************************************************
* Function Name: void ws2812LightShowTask (void *pvParameters)
********************************************************************************
* Summary:
*  Task that processes the main application for the light show
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  void
*
*******************************************************************************/
void ws2812LightShowTask(void *arg)
{
	(void)arg;
	printf("Starting LightShow task\r\n");
	vTaskDelay(pdMS_TO_TICKS(1000));

	/* If channel is enabled, get the number of LEDs for the channel */
	for(uint8_t i = 0; i < MAX_LED_STRINGS; i++)
	{
		if(channelStatus[i] == 1)
		{
			numOfLeds[i] = ws2812HAL_getNumLeds(i);
		}
	}
	/* Test the Display Clear */
	ws2812_DisplayClear(0, ws2812_BLUE);
	vTaskDelay(pdMS_TO_TICKS(500));
	/* Test the RGB Mix */
//	ws2812_MixColorRGB(0);
//	vTaskDelay(pdMS_TO_TICKS(500));
	/* Test the FadeInFadeOutRGB */
	ws2812_FadeInFadeOutRGB(0, 0xFF, 0xF0, 0xF0, 1, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the FadeInFadeOutColor */
//	ws2812_FadeInFadeOutColor(0, ws2812_GREEN, 1, 1);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_StrobeRGB */
//	ws2812_StrobeRGB(0, 0xFF, 0xFF, 0xFF, 20, 50, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_StrobeColor */
//	ws2812_StrobeColor(0, ws2812_FIRE_LIGHT, 20, 50, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_NewKITT */
//	ws2812_NewKITT(0, 0xFF, 0x00, 0x00, 4, 100, 100);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_HalloweenEyesRGB */
//	ws2812_HalloweenEyesRGB(0, 0xFF, 0x00, 0x00, 1, 3, true, 50, 60, 1000);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	/* Test the ws2812_TwinkleRGB */
//	ws2812_TwinkleRGB(0, 0xFF, 0xFF, 0xFF, 10, 100, true);
//	vTaskDelay(pdMS_TO_TICKS(1000));

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


/*******************************************************************************
* Function Name: ws2812_MixColorRGB
********************************************************************************
* Summary:
*  This function changes the LEDs to RGBRGBRGB....RGB
*
* Parameters:
*  ledString: The number of the string to be changed
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_MixColorRGB(uint32_t stringNumber)
{
	/* Cycle through all LEDs and set the RGB pattern to all LEDs in the string passed */
    for(uint32_t i = 0; i < numOfLeds[stringNumber]; i++)
    {
        switch(i%3)
        {
            case 0:
            	ws2812_setPixelColor(stringNumber, i, ws2812_RED); // red
                break;
            case 1:
            	ws2812_setPixelColor(stringNumber, i, ws2812_GREEN); // green
                break;
            case 2:
            	ws2812_setPixelColor(stringNumber, i, ws2812_BLUE); // blue
                break;
        }
    }
    ws2812_update(stringNumber);
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
	colorIndex = colorIndex % ws2812_CWHEEL_SIZE;

	#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
	color = ws2812_CLUT[ colorIndex ];
	#else
	color = colorIndex;
	#endif

    return(color);
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
	/* Clear the LEDs by setting the color selected. Black is for off */
	ws2812_setAllColor(stringNumber, color);
	/* Update the LEDs in the string */
    ws2812_update(stringNumber);
}

/*******************************************************************************
* Function Name: ws2812_FadeInFadeOutColor
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint32 color: Color to clear display.
*  bool fadeIn: request fade in
*  bool fadeOut: request fade out
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_FadeInFadeOutColor(uint8_t stringNumber, uint32_t colorRequested, bool fadeIn, bool fadeOut)
{
	typedef union {
			uint8_t uint8_ts[4];
			uint32_t word;
		} WS_colorUnion;

	WS_colorUnion color;
	color.word = colorRequested;
	/* Parse the color code into the three components */
	uint8_t red = color.uint8_ts[1];
	uint8_t blue = color.uint8_ts[2];
	uint8_t green = color.uint8_ts[0];

	/* Pass the parameters to the RGB equivalent function */
	ws2812_FadeInFadeOutRGB(stringNumber, red, green, blue, fadeIn, fadeOut);
}

/*******************************************************************************
* Function Name: ws2812_FadeInFadeOutRGB
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  bool fadeIn: request fade in
*  bool fadeOut: request fade out
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_FadeInFadeOutRGB(uint8_t stringNumber, uint8_t red, uint8_t blue, uint8_t green, bool fadeIn, bool fadeOut)
{
	float r, g, b;

	/* If flag to Fade In is set, execute the FadeIn sequence */
	if(fadeIn)
	{
		for(uint32_t k = 0; k < 256; k=k+1)
		{
			r = (k/256.0)*red;
			g = (k/256.0)*green;
			b = (k/256.0)*blue;
			/* Update the LEDs in the string to the new value */
			ws2812_setAllRGB(stringNumber, r, g, b);
			/* Update the LEDs in the string */
			ws2812_update(stringNumber);
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
	/* If flag to Fade Out is set, execute the FadeIn sequence */
	if(fadeOut)
	{
		for(uint32_t k = 255; k >= 0; k=k-2)
		{
			r = (k/256.0)*red;
			g = (k/256.0)*green;
			b = (k/256.0)*blue;
			/* Update the LEDs in the string to the new value */
			ws2812_setAllRGB(stringNumber, r, g, b);
			/* Update the LEDs in the string */
			ws2812_update(stringNumber);
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
}

/*******************************************************************************
* Function Name: ws2812_StrobeRGB
********************************************************************************
* Summary:
*   Strobes the string with a defined RGB value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  uint32_t StrobeCount: how many flashes to perform
*  uint32_t FlashDelay: the delay between flashes
*  uint32_t EndPause: the end of the cycle delay
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_StrobeRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause)
{
  for(uint32_t j = 0; j < StrobeCount; j++)
  {
	  /* Set the RGB color and update the string */
	  ws2812_setAllRGB(stringNumber, red, green, blue);
	  ws2812_update(stringNumber);
	  /* Delay for a flash before turning off */
	  vTaskDelay(pdMS_TO_TICKS(FlashDelay));
	  /* Turn off the string and update */
	  ws2812_setAllColor(stringNumber, ws2812_BLACK);
	  ws2812_update(stringNumber);
	  /* Delay for a flash before turning back on */
	  vTaskDelay(pdMS_TO_TICKS(FlashDelay));
  }
  /* Delay before exiting */
  vTaskDelay(pdMS_TO_TICKS(EndPause));
}

/*******************************************************************************
* Function Name: ws2812_StrobeColor
********************************************************************************
* Summary:
*   Fades in and out with a given value
*
* Parameters:
*  uint8_t stringNumber: string to control
*  uint8_t red: red component display.
*  uint8_t blue: blue component to display
*  uint8_t green: green component to display
*  uint32_t StrobeCount: how many flashes to perform
*  uint32_t FlashDelay: the delay between flashes
*  uint32_t EndPause: the end of the cycle delay
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_StrobeColor(uint8_t stringNumber, uint32_t colorRequested, uint32_t StrobeCount, uint32_t FlashDelay, uint32_t EndPause)
{
	typedef union {
		uint8_t uint8_ts[4];
		uint32_t word;
	} WS_colorUnion;

	WS_colorUnion color;
	color.word = colorRequested;
	/* Parse the color code into the three components */
	uint8_t red = color.uint8_ts[1];
	uint8_t blue = color.uint8_ts[2];
	uint8_t green = color.uint8_ts[0];
	/* Pass the parameters to the RGB equivalent function */
	ws2812_StrobeRGB(stringNumber, red, green, blue, StrobeCount, FlashDelay, EndPause);
}


void ws2812_NewKITT(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	ws2812_RightToLeft(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_LeftToRight(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_OutsideToCenter(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_CenterToOutside(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_LeftToRight(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_RightToLeft(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_OutsideToCenter(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	ws2812_CenterToOutside(stringNumber, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void ws2812_CenterToOutside(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i =((numOfLeds[stringNumber] - EyeSize)/2); i >= 0; i--)
	{
		/* Turn off the string and update */
		ws2812_setAllColor(stringNumber, ws2812_BLACK);
		ws2812_update(stringNumber);

		ws2812_setPixelRGB(stringNumber, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, i+j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, i+EyeSize+1, red/10, green/10, blue/10);

		ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i-j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i-EyeSize-1, red/10, green/10, blue/10);

		ws2812_update(stringNumber);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_OutsideToCenter(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = 0; i<=((numOfLeds[stringNumber]-EyeSize)/2); i++)
	{
		/* Turn off the string and update */
		ws2812_setAllColor(stringNumber, ws2812_BLACK);
		ws2812_update(stringNumber);

		ws2812_setPixelRGB(stringNumber, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, i+j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, i+EyeSize+1, red/10, green/10, blue/10);

		ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i-j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, numOfLeds[stringNumber]-i-EyeSize-1, red/10, green/10, blue/10);

		ws2812_update(stringNumber);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_LeftToRight(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = 0; i < numOfLeds[stringNumber]-EyeSize-2; i++) {
		/* Turn off the string and update */
		ws2812_setAllColor(stringNumber, ws2812_BLACK);
		ws2812_update(stringNumber);
		ws2812_setPixelRGB(stringNumber, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, i+j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, i+EyeSize+1, red/10, green/10, blue/10);
		ws2812_update(stringNumber);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
}

void ws2812_RightToLeft(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeSize, uint32_t SpeedDelay, uint32_t ReturnDelay)
{
	for(uint32_t i = numOfLeds[stringNumber]-EyeSize-2; i > 0; i--)
	{
		/* Turn off the string and update */
		ws2812_setAllColor(stringNumber, ws2812_BLACK);
		ws2812_update(stringNumber);
		ws2812_setPixelRGB(stringNumber, i, red/10, green/10, blue/10);
		for(uint32_t j = 1; j <= EyeSize; j++)
		{
			ws2812_setPixelRGB(stringNumber, i+j, red, green, blue);
		}
		ws2812_setPixelRGB(stringNumber, i+EyeSize+1, red/10, green/10, blue/10);
		ws2812_update(stringNumber);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
	}
	vTaskDelay(pdMS_TO_TICKS(ReturnDelay));
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

    newColor |= (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
    newColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
    newColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;

   return(newColor);
}

/*******************************************************************************
* Function Name: ws2812_HalloweenEyesRGB()
********************************************************************************
* Summary: This function shows two red eyes at a random spot on the strip
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   EyeWidth	How many LEDs per eye
*   EyeSpace	Number of LEDs between the eyes
*   Fade	 	Fade out or not
*   Steps	 	Number of steps on fade out
*   FadeDelay	Delay between each fade out level
*   EndPause	Delay after everything is completed
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_HalloweenEyesRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t EyeWidth, uint32_t EyeSpace,
		bool Fade, uint32_t Steps, uint32_t FadeDelay, uint32_t EndPause)
{

	uint32_t i;
	//int num = (rand() % (upper - lower + 1)) + lower;
	uint32_t StartPoint  = random() % (numOfLeds[stringNumber] - (2 * EyeWidth) - EyeSpace);
	uint32_t Start2ndEye = StartPoint + EyeWidth + EyeSpace;

	/* Turn off the string and update */
	ws2812_setAllColor(stringNumber, ws2812_BLACK);
	ws2812_update(stringNumber);

	for(i = 0; i < EyeWidth; i++)
	{
		ws2812HAL_setPixelRGB(stringNumber, StartPoint + i, red, green, blue);
		ws2812HAL_setPixelRGB(stringNumber, Start2ndEye + i, red, green, blue);
	}
	/* Update the LEDs in the string */
	ws2812_update(stringNumber);

	if(Fade == true)
	{
		float r, g, b;

		for(uint32_t j = Steps; j >= 0; j--)
		{
			r = j*(red/Steps);
			g = j*(green/Steps);
			b = j*(blue/Steps);

			for(i = 0; i < EyeWidth; i++)
			{
				ws2812HAL_setPixelRGB(stringNumber, StartPoint + i, r, g, b);
				ws2812HAL_setPixelRGB(stringNumber, Start2ndEye + i, r, g, b);
			}

			/* Update the LEDs in the string */
			ws2812_update(stringNumber);
			vTaskDelay(pdMS_TO_TICKS(FadeDelay));
		}
	}

	/* Turn off the string and update */
	ws2812_setAllColor(stringNumber, ws2812_BLACK);
	ws2812_update(stringNumber);

	vTaskDelay(pdMS_TO_TICKS(EndPause));
}

/*******************************************************************************
* Function Name: ws2812_TwinkleRGB()
********************************************************************************
* Summary: This function will blink one or more LEDs in a given color
* that fade away
*
*
* Parameters:
* 	stringNumber the string to address
*   red	 		Red Color
*   green	 	Green Color
*   blue	 	Blue Color
*   Count		How many LEDs
*   SpeedDelay	Delay between each LED
*   OnlyOne		True to show only one LED at a time
*
* Return:
*  void
*
*******************************************************************************/
void ws2812_TwinkleRGB(uint8_t stringNumber, uint8_t red, uint8_t green, uint8_t blue, uint32_t Count, uint32_t SpeedDelay, bool OnlyOne)
{
	/* Turn off the string and update */
	ws2812_setAllColor(stringNumber, ws2812_BLACK);
	ws2812_update(stringNumber);
	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));

	for (uint32_t i = 0; i < Count; i++)
	{
		uint32_t randLedIndex = (random() % (numOfLeds[stringNumber] - 1));
		ws2812HAL_setPixelRGB(stringNumber, randLedIndex, red, green, blue);
		ws2812_update(stringNumber);
		vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		if(OnlyOne)
		{
			/* Turn off the string and update */
			ws2812_setAllColor(stringNumber, ws2812_BLACK);
			ws2812_update(stringNumber);
			vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
		}
	}

	vTaskDelay(pdMS_TO_TICKS(SpeedDelay));
}
