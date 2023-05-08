#include "nu32dip.h"
#include "ws2812b.h"

#define NUMLEDS 8

int main(void) {
    ws2812b_setup();
    wsColor c[NUMLEDS];

    //turn off leds
    wsColor off = {0, 0, 0};
    for (int i = 0; i < NUMLEDS; i++) {
        c[i] = off;
    }
   
    ws2812b_setColor(c, NUMLEDS);

    //make led rainbow pattern
    int angle = 0;
    while(1) {
        //set each led to be equaly spaced around the color wheel
        for(int i = 0; i < NUMLEDS; i++) {
            c[i] = HSBtoRGB((angle + i * 360 / NUMLEDS) % 360 , 1, 0.1);
        }
        ws2812b_setColor(c, NUMLEDS);
        angle = (angle + 2) % 360;
        //delay 10ms of a second
        int time = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() < time + 24000000 / 100) {
            ;
        }
    }

}