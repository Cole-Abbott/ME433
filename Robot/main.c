#include "nu32dip.h"
#include "motor.h"
#include "ws2812b.h"
#include "servo.h"
#include "uart2.h"


int main(void) {
    NU32DIP_Startup();
    motorInit();
    ws2812b_setup();
    ws2812b_setupISR();
    servoInit();
    UART2_Startup();

    
    servoSet(45);

    motorASet(0.8);
    motorBSet(0.8);

	char message[100];
    
    while(1) {
		int com;
		com = get_COM();
		sprintf(message, "COM: %d\r\n", com);
		NU32DIP_WriteUART1(message);
    }
    return 0;
}