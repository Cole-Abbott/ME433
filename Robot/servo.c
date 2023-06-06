#include "servo.h"
#include "nu32dip.h"


//Sets up servo PWM on timer 2 OC1 pin B7
void servoInit(void) {
    //set up servo PWM on timer 2 OC1
    T2CONbits.TCKPS = 0b100;     // Timer2 prescaler N=16 (1:16)
    PR2 = 59999;              // period = (PR2+1) * N * (1/48000000) =  50Hz
    TMR2 = 0;                // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL = 0;   // Use timer2
    OC1RS = 5999;             // duty cycle = OC1RS/(PR2+1) = 2ms = 90 degrees
    OC1R = 5999;              // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer2
    OC1CONbits.ON = 1;       // turn on OC1
    RPB7Rbits.RPB7R = 0b0101;     //set B7 to OC1
}


//set the servo to the angle in degrees
void servoSet(unsigned int angle) {
    //convert angle to a duty cycle
    //0: 2500
    //180: 7500
    angle = angle % 180; //only use 0-179 degrees
    unsigned int duty = (angle / 180.0) * 6000 + 1500; 
    OC1RS = duty;

}
