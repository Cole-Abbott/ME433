#include "nu32dip.h"
#include "motor.h"
#include "ws2812b.h"


int main(void) {
    NU32DIP_Startup();
    motorInit();
    ws2812b_setup();
    ws2812b_setupISR();


    motorASet(0.5);
    motorBSet(0.5);

    
    while(1) {
      ;  
    }
    return 0;
}