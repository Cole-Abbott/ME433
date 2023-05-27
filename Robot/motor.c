#include "servo.h"
#include "nu32dip.h"


//Sets up motor PWM on timer 3 OC2, OC3 pin 
void motorInit(void) {
    //setup timer3
    T3CONbits.TCKPS = 0b0;     // Timer2 prescaler N=1 (1:1)
    PR3 = 2400;              // period = (PR2+1) * N * (1/48000000) =  20kHz
    TMR3 = 0;                // initial TMR2 count is 0
    
    //setup OC2
    OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC2CON bits are defaults
    OC2CONbits.OCTSEL = 1;   // Use timer3
    OC2RS = 0;               // duty cycle = OC1RS/(PR2+1) = 0 = 0%
    OC2R = 0;                // initialize before turning OC2 on; afterward it is read-only
    _CP0_GET_COUNT();
    //setup OC3
    OC3CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC3CON bits are defaults
    OC3CONbits.OCTSEL = 1;   // Use timer3
    OC3RS = 0;               // duty cycle = OC1RS/(PR2+1) = 0 = 0%
    OC3R = 0;                // initialize before turning OC2 on; afterward it is read-only

    //todo map OC1, OC2 to pins
    //setup dir pins
    //RPB7Rbits.RPB7R = 0b0101;     //set B7 to OC1
    
    
    
    //turn on timer2, OC2, OC3
    T3CONbits.ON = 1;        // turn on Timer2
    OC2CONbits.ON = 1;       // turn on OC2
    OC3CONbits.ON = 1;       // turn on OC3
    

}


//set the motor1 PWM
void motor1Set(float PWM) {

}

//set the motor2 PWM
void motor2Set(float PWM) { 

}
