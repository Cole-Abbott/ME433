#include "nu32dip.h" // constants, functions for startup and UART


int main(void) {
    char message[200];
    int time;

    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    while (1) {
        while(NU32DIP_USER == 1) {}
            for (int i = 0; i < 100; i++){
                //write i to uart
                sprintf(message, "%d\n\r", i);
                NU32DIP_WriteUART1(message);

                //wait 0.01 seconds
                time = _CP0_GET_COUNT();
                while(_CP0_GET_COUNT() - time < 240000) {}
            }
    
    }
}


