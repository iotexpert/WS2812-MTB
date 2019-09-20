#ifndef WS2812GRAPHICS_H
#define WS2812GRAPHICS_H

#include "ws2812Hal.h"

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

/* Define the enum for the theme and LED light show control */
typedef enum {
	themeChristmas,
	themeFourthJuly,
	themeNewYear,
	themeHalloween,
}themeLight_t;

/* Define the enum for what type of animation to use */
typedef enum {
	animationChaseSingle,
	animationChaseTail,
	animationOnOff,
	animationRainbow,
	animationAlternate,
	animationBlend,
	animationRoundRobin,
}animation_t;

/* Color Union for better access to color elements */
typedef union {
    uint8_t byteColor[4];
    uint32_t RGB;
} colorUnion_t;

/* Define the struct for the Theme being loaded */
typedef struct {
	themeLight_t themeLight;
	uint32_t themeColor[4];
	animation_t themeAnimation;
	uint8_t themeChannelOrder[MAX_LED_STRINGS];
	uint32_t themeDelay;
}themeParameters_t;

extern void WS_initMixColorRGB(uint32_t ledString);


#endif
