// WS2812B library

#include "ws2812b.h"
#include "nu32dip.h"
// other includes if necessary for debugging

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.65uS


// setup Timer4 for 48MHz, and setup the output pin as pin B9
void ws2812b_setup() {
    T4CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
    PR4 = 65535; // maximum period
    TMR4 = 0; // initialize Timer2 to 0
    T4CONbits.ON = 1; // turn on Timer2

    // initialize output pin as off
    TRISBbits.TRISB9 = 0; // set pin B9 as an output
    LATBbits.LATB9 = 0; // set pin B9 low
}

// build an array of high/low times from the color input array, then output the high/low bits
void ws2812b_setColor(wsColor * c, int numLEDs) {
    int i = 0; int j = 0; // for loops
    int numBits = 2 * 3 * 8 * numLEDs; // the number of high/low bits to store, 2 per color bit
    volatile unsigned int delay_times[numBits]; // I only gave you 5 WS2812B, adjust this if you get more somewhere

    // start at time at 0
    delay_times[0] = 0;
    
    int nB = 1; // which high/low bit you are storing, 2 per color bit, 24 color bits per WS2812B
	
    // loop through each WS2812B
    for (i = 0; i < numLEDs; i++) {
        // loop through each color bit, MSB first
        for (j = 7; j >= 0; j--) {
            // if the bit is a 1 /* identify the bit in c[].r, is it 1 */
            if ((c[i].g >> j) & 1) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
        // do it again for green
        // loop through each color bit, MSB first
        for (j = 7; j >= 0; j--) {
            // if the bit is a 1 /* identify the bit in c[].r, is it 1 */
            if (c[i].r >> j & 1) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
		// do it again for blue
        // loop through each color bit, MSB first
        for (j = 7; j >= 0; j--) {
            // if the bit is a 1 /* identify the bit in c[].r, is it 1 */
            if (c[i].b >> j & 1) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
    }

    // turn on the pin for the first high/low
    LATBbits.LATB9 = 1;

    TMR4 = 0; // start the timer
    for (i = 1; i < numBits; i++) {
        while (TMR4 < delay_times[i]) {
        }
        LATBINV = 0b1000000000; // invert B9
    }
    LATBbits.LATB9 = 0;
    TMR4 = 0;
    while(TMR4 < 2400){} // wait 50uS, reset condition
}

//set Controller to Timer5 ISR
void __ISR(_TIMER_5_VECTOR, IPL6SOFT) ws2812b_controller(void);

//setup ISR for Timer5 @ 60Hz to control ws2812b in rainbow pattern
void ws2812b_setupISR() {
    	//config ISR on Timer5 @ 60Hz
	{
	__builtin_disable_interrupts();
	//config ISR
	IFS0bits.T5IF = 0;			// Timer5 flag clear
	IEC0bits.T5IE = 1;			// Timer5 enable set
	IPC5bits.T5IP = 6;			// Timer5 priority 6
	IPC5bits.T5IS = 0;			// Timer5 sub-priority 1
	
    //config Timer5
	T5CONbits.TCKPS = 0b100; 	// Timer5 prescaler = 4 (1:16)
	PR5 = 50000-1;				// period = 50000, 60Hz
	TMR5 = 0; 					// set Timer5 to 0
	T5CONbits.ON = 1;			// turn on Timer2
	__builtin_enable_interrupts();
	}

}

#define NUMLEDS 16

//ISR for Timer5 @ 60Hz to control ws2812b in rainbow pattern
void ws2812b_controller() {
    NU32DIP_GREEN = 0;

    static int angle = 0;
    static wsColor c[NUMLEDS];
    
    for (int i = 0; i < NUMLEDS / 2 ; i++) {
        c[i] = HSBtoRGB((angle + i * 360 / NUMLEDS) % 360 , 1.0, 0.1);
        c[NUMLEDS - i - 1] = HSBtoRGB((angle + i * 360 / NUMLEDS) % 360 , 1.0, 0.1);

    }

    ws2812b_setColor(c, NUMLEDS);
    
    angle = angle + 5 % 360;


    NU32DIP_GREEN = 1;
    IFS0bits.T5IF = 0;			//Tieintmer2 flag clear
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}