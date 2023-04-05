#include "nu32dip.h" // constants, functions for startup and UART
#include <math.h>

int main(void) {
    char message[200];
    int time;

    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    while (1) {
        while(NU32DIP_USER == 1) {}
            for (int i = 0; i < 100; i++){
                //write i to uart
                //sprintf(message, "%d\n\r", i);
                //NU32DIP_WriteUART1(message);

                //write sine wave with 100 samples to UART
                sprintf(message, "%f\n\r", (3.3* sin(2*3.14159*i/100)));
                NU32DIP_WriteUART1(message);

                //wait 0.01 seconds
                time = _CP0_GET_COUNT();
                while(_CP0_GET_COUNT() - time < 240000) {}
            }
    }
}


