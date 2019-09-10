/*
 * ws2812.h
 *
 *  Created on: Jun 15, 2019
 *      Author: arh
 */

#ifndef WS2812_H_
#define WS2812_H_

#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"

#define ws2812_NUM_PIXELS_WS1 (3)
#define ws2812_NUM_PIXELS_WS2 (3)
#define ws2812_NUM_PIXELS_WS3 (3)
#define ws2812_NUM_PIXELS_WS4 (3)
#define ws2812_NUM_PIXELS_WS5 (24)

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

extern

void ws2812_update(uint8_t stringNumber);
void ws2812_autoUpdate(bool option);
void ws2812_setRGB(uint8_t stringNumber, uint32_t led, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setRange(uint8_t stringNumber, uint32_t start, uint32_t end, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_initMixColorRGB(uint8_t stringNumber);

void ws2812Task(void *arg);

#endif /* WS2812_H_ */
