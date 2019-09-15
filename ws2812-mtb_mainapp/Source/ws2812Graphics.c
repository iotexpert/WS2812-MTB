
#include "ws2812Hal.h"
#include "ws2812Graphics.h"



// Initializes the RGB frame buffer to RGBRGBRGB...RGB
void WS_initMixColorRGB(uint32_t ledString)
{

    for(int i=0;i<WS_getNumLeds(ledString);i++)
    {
        switch(i%3)
        {
            case 0:
                WS_setRGB(ledString,i,0x80,0x00,0x00); // red
                break;
            case 1:
                WS_setRGB(ledString,i,0x00,0x80,0x00); // green
                break;
            case 2:
                WS_setRGB(ledString,i,0x00,0x00,0x80); // blue
                break;
        }
    }
}
