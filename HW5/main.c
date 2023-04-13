#include "nu32dip.h"
#include "spi.h"

int main(void) {
    NU32DIP_Startup();
    //init spi

    //can pre calculate sine wave points here and store in array so you dont
    //have to calculate on the fly, saves speed but costs memory


    while(1) {
        //figure out what voltage for sine wave, triangle wave
        unsigned char Va = 4;


        //format message

        //config bits
        unsigned short t = 0;
        t = 0b111 << 12; //= 0111 0000 0000 0000
        t = t | (Va << 15); 


        //send data
        //cs high
        spi_io(t >> 8); //first 8 bits
        spi_io(t); //last 8 bits
        //cs low


        //delay
    }

}


