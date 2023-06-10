#include "motor.h"
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

    //setup OC3
    OC3CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC3CON bits are defaults
    OC3CONbits.OCTSEL = 1;   // Use timer3
    OC3RS = 0;               // duty cycle = OC1RS/(PR2+1) = 0 = 0%
    OC3R = 0;                // initialize before turning OC2 on; afterward it is read-only

    //todo map OC1, OC2 to pins
    //OC2 -> B11
    RPB11Rbits.RPB11R = 0b0101;     //set B11 to OC2
    
    //OC3 -> B14
    RPB14Rbits.RPB14R = 0b0101;     //set B14 to OC3
    
    //setup dir pins: B13, B15
    TRISBbits.TRISB13 = 0;  //set B13 to output
    TRISBbits.TRISB15 = 0;  //set B15 to output
    
    
    //turn on timer2, OC2, OC3
    T3CONbits.ON = 1;        // turn on Timer2
    OC2CONbits.ON = 1;       // turn on OC2
    OC3CONbits.ON = 1;       // turn on OC3
    

}

//set the motor1 PWM
//A motor: OC2 -> B11 -> AEN, DIR -> B13 -> APH
//PWM is between -1 and 1
void motorASet(float PWM) {
    if (PWM > 0) {
        LATBbits.LATB13 = 1; //set DIR to forward
        OC2RS = (int) (PWM * 2400); //set PWM
    } else {
        LATBbits.LATB13 = 0; //set DIR to reverse
        OC2RS = (int) (-PWM * 2400); //set PWM
    }
}

//set the motor2 PWM
//B motor: OC3 -> B14 -> BEN, DIR -> B15 -> BPH
//PWM is between -1 and 1
void motorBSet(float PWM) { 
    if (PWM > 0) {
        LATBbits.LATB15 = 1; //set DIR to forward
        OC3RS = (int) (PWM * 2400); //set PWM
    } else {
        LATBbits.LATB15 = 0; //set DIR to reverse
        OC3RS = (int) (-PWM * 2400); //set PWM
    }
}
