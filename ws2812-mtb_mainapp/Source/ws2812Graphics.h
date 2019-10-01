#ifndef WS2812GRAPHICS_H
#define WS2812GRAPHICS_H

#include "ws2812Hal.h"

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

#define ws2812_CWHEEL_SIZE  24

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

extern void ws2812LightShowTask(void *arg);

#endif
