#include "servo.h"
#include "nu32dip.h"


int main(void) {
    servoInit(); //initialize the servo PWM
    while(1) {
        //move the servo from 0 to 180 degrees every second
        for(int i = 45; i < 135; i += 1) {
            servoSet(i);
            int time = _CP0_GET_COUNT();
            while(_CP0_GET_COUNT() - time < 24000000/90 * 4) {
                ;
            }
        }
        //move the servo from 180 to 0 degrees every second
        for(int i = 135; i > 45; i -= 1) {
            servoSet(i);
            int time = _CP0_GET_COUNT();
            while(_CP0_GET_COUNT() - time < 24000000/90 * 4) {
                ;
            }
        }
    }
}