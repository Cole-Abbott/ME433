#include "nu32dip.h"
#include "motor.h"
#include "ws2812b.h"
#include "servo.h"
#include "uart2.h"

#define MAXPWM .9
#define Kp 0.02


int main(void) {
    NU32DIP_Startup();
    motorInit();
    ws2812b_setup();
    ws2812b_setupISR();
    servoInit();
    UART2_Startup();

    servoSet(45);


	char message[100];
    
    while(1) {
		int com;
		com = get_COM();

		sprintf(message, "DIR: %d\r\n", com);
		NU32DIP_WriteUART1(message);
		//1 = forward
		//2 = backward
		//3 = stop
		//4 = left
		//5 = right
		switch (com)
		{
		case 1:
			motorASet(MAXPWM);
			motorBSet(MAXPWM);
			break;

		case 2:
			motorASet(-MAXPWM * 0.75);
			motorBSet(-MAXPWM * 0.75);
			break;

		case 3:
			motorASet(0);
			motorBSet(0);
			break;

		case 4:
			motorASet(MAXPWM * 0.7);
			motorBSet(0);
			break;

		case 5:
			motorASet(0);
			motorBSet(MAXPWM * 0.7);
			break;
			
		break;
		

		}



    }
    return 0;
}