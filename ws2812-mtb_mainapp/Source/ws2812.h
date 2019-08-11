/*
 * ws2812.h
 *
 *  Created on: Jun 15, 2019
 *      Author: arh
 */

#ifndef WS2812_H_
#define WS2812_H_
/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
/* Inclusion of system and local header files goes here */
/* Header file includes */
#include "cy_pdl.h"
#include "cycfg.h"
#include "stdbool.h"

#include "FreeRTOS.h"
#include "queue.h"

/* Number of independent hardware channels in the system */
#define ws2812_NUMBER_OF_CHANNELS	(5U)

/* Define the Dimming Level */
#define ws2812_DimLevel_0   0
#define ws2812_DimLevel_1   1
#define ws2812_DimLevel_2   2
#define ws2812_DimLevel_3   3
#define ws2812_DimLevel_4   4

/* Define the system Memory Type to be used. Same for all channels */
#define ws2812_MEMORY_TYPE 			ws2812_MEMORY_RGB
#define ws2812_MEMORY_RGB   0
#define ws2812_MEMORY_LUT   1

/* Define the system Gamma correction on or off to be used. Same for all channels */
#define ws2812_GAMMA_CORRECTION   	ws2812_GAMMA_ON
#define ws2812_GAMMA_ON            	1
#define ws2812_GAMMA_OFF           	0

/* Define the Coordinate warp to be used. Same for all channels */
#define ws2812_COORD_WRAP        ws2812_COORD_NONE
#define ws2812_COORD_NONE        (0u)
#define ws2812_COORD_XAXIS       (1u)
#define ws2812_COORD_YAXIS       (2u)
#define ws2812_COORD_XYAXIS      (3u)

/* Define the LED Layout used. Each channel has its own layout */
#define ws2812_LED_LAYOUT_STANDARD  (0u)
#define ws2812_LED_LAYOUT_SPIRAL    (1u)
#define ws2812_LED_LAYOUT_GRID16X16 (2u)

#define ws2812_FLIP_X_COORD      0
#define ws2812_FLIP_Y_COORD      0

#define ws2812_FLIP_COORD         (1u)
#define ws2812_NOT_FLIP_COORD     (0u)

#define ws2812_SWAP_XY_COORD      0
#define ws2812_XY_SWAPED          (1u)

#define ws2812_GRIDMODE_NORMAL  1
#define ws2812_GRIDMODE_WRAP    0

#define ws2812_CHIP        ws2812_CHIP_WS2812
#define ws2812_CHIP_WS2811     1
#define ws2812_CHIP_WS2812     2
#define ws2812_CHIP_SF2812RGBW 3

#define ws2812_COLOR_WHEEL_SIZE  24

#if(ws2812_CHIP == ws2812_CHIP_WS2812)
#define ws2812_RED_MASK   0x0000FF00
#define ws2812_GREEN_MASK 0x000000FF
#define ws2812_BLUE_MASK  0x00FF0000
#else
#define ws2812_RED_MASK   0x000000FF
#define ws2812_GREEN_MASK 0x0000FF00
#define ws2812_BLUE_MASK  0x00FF0000
#endif

#define ws2812_CWHEEL_SIZE 24
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


#define INCLUDE_WS1				(0UL)
#define INCLUDE_WS2				(0UL)
#define INCLUDE_WS3				(0UL)
#define INCLUDE_WS4				(0UL)
#define INCLUDE_WS5				(1UL)

typedef struct {
	bool enable;
	uint8_t ledLayout;
	uint32_t columns;
	uint32_t rows;
	uint32_t totalLeds;
	uint32_t arrayColumns;
	uint32_t arrayRows;
	uint32_t minX;
	uint32_t maxX;
	uint32_t minY;
	uint32_t maxY;
	uint8_t *frameBuffer;
	uint32_t frameBufferLength;
	#if(ws2812_MEMORY_TYPE == ws2812_MEMORY_RGB)
	uint32_t  **ledArray;
	#else
	uint8_t   **ledArray;
	#endif
	cy_stc_dma_descriptor_t *dmaDescriptors;
	uint32_t numberDmaDescriptors;
	DW_Type *dmaHW;
	uint8_t dmaChannel;
	CySCB_Type *scbHW;
}channelConfig_t;

/* Declare the channel configuration array */
extern channelConfig_t channelConfig[ws2812_NUMBER_OF_CHANNELS];

extern QueueHandle_t ws2812QueueHandle;


typedef enum {
	ws2812_cmd_update,            /* no arguments */
	ws2812_cmd_autoUpdate,        /* data is a binary true for autoupdate false for no update  */
	ws2812_cmd_setRGB,            /* data is pixel number + rgb                                */
	ws2812_cmd_setRange,          /* data is 0xFF00 bits for start and 0x00FF bits for y + rgb */
	ws2812_cmd_initMixColorRGB,   /* no arguments, turns string to rgbrgbrgb...                */
}ws2812_cmd_t;

typedef struct {
	ws2812_cmd_t cmd;				/* Command from ws2812_cmd_t enum */
	uint8_t stringNumber;			/* String Number = 1, 2, 3, 4, or 5 */
	uint32_t data;					/**/
	uint8_t red;					/**/
	uint8_t green;					/**/
	uint8_t blue;					/**/
} ws2812_msg_t;



extern void ws2812_update(uint8_t stringNumber);
extern void ws2812_autoUpdate(bool option);
extern void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
extern void ws2812_initMixColorRGB(uint8_t stringNumber);

extern void ws2812Task(void *arg);

#endif /* WS2812_H_ */
