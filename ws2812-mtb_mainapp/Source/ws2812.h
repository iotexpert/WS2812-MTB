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

#define ws2812_NUM_PIXELS (144)

extern QueueHandle_t ws2812QueueHandle;


typedef enum {
	ws2812_cmd_update,            /* no arguments */
	ws2812_cmd_autoUpdate,        /* data is a binary true for autoupdate false for no update  */
	ws2812_cmd_setRGB,            /* data is pixel number + rgb                                */
	ws2812_cmd_setRange,          /* data is 0xFF00 bits for start and 0x00FF bits for y + rgb */
	ws2812_cmd_initMixColorRGB,   /* no arguments, turns string to rgbrgbrgb...                */
}ws2812_cmd_t;

typedef struct {
	ws2812_cmd_t cmd;
	uint32_t data;
	uint8_t red;
	uint8_t green;
	uint8_t blue;

} ws2812_msg_t;

extern

void ws2812_update(void);
void ws2812_autoUpdate(bool option);
void ws2812_setRGB(int led,uint8_t red, uint8_t green, uint8_t blue);
void ws2812_setRange(int start, int end, uint8_t red,uint8_t green ,uint8_t blue);
void ws2812_initMixColorRGB(void);

void ws2812Task(void *arg);

#endif /* WS2812_H_ */
