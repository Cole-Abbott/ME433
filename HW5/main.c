#include "nu32dip.h"
#include "spi.h"
#include <math.h>

int main(void) {
    NU32DIP_Startup();
    //init spi
    initSPI();

    //can pre calculate sine wave points here and store in array so you dont
    //have to calculate on the fly, saves speed but costs memory
    
    unsigned short Va; //voltage a
    unsigned short Vb; //voltage b
    unsigned short t = 0; //message bits

    int i = 0;

    while(1) {
        int time = _CP0_GET_COUNT();

        //figure out what voltage for sine wave, triangle wave
        Va = (sin((M_PI * i * 2) / 500 ) + 1) * 511.5; // sin wave, 2Hz, 1023 amplitude
        Vb = 1023/500 * abs(i - 500); // triange wave, 1hz, 1023 amplitude

        i = (i + 1) % 1000; //reset i every second

        //format data a
        t = 0b0111 << 12; //= 0111 0000 0000 0000
        t = t | (Va << 2); 
        //send data a
        //CHIP_SELECT high
        CHIP_SELECT = 0;
        spi_io(t >> 8); //first 8 bits
        spi_io(t); //last 8 bits
        //CHIP_SELECT low
        CHIP_SELECT = 1;


        //format data b
        t = 0b1111 << 12; //= 0111 0000 0000 0000
        t = t | (Vb << 2); 
        //send data b
        //CHIP_SELECT high
        CHIP_SELECT = 0;
        spi_io(t >> 8); //first 8 bits
        spi_io(t); //last 8 bits
        //CHIP_SELECT low
        CHIP_SELECT = 1;
        
        //delay 1 ms
        while(_CP0_GET_COUNT() < time + 24000) {;}
    }

}


