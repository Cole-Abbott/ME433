#include "nu32dip.h"
#include "motor.h"
#include "ws2812b.h"
#include "servo.h"
#include "uart2.h"

#define MAXPWM 0.5
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

		sprintf(message, "COM: %d\r\n", com);
		NU32DIP_WriteUART1(message);

		//if com [25-35], go straight
		if (com > 25 && com < 35) {
			motorASet(MAXPWM);
			motorBSet(MAXPWM);
		}
		//if com [35-60], turn right
		else if (com > 35) {
			motorASet(MAXPWM);
			motorBSet(MAXPWM * (1 - Kp * (com - 35)));
		}
		//if com [0-25], turn left
		else if (com < 25) {
			motorASet(MAXPWM * (1 - Kp * (25 - com)));
			motorBSet(MAXPWM);
		}


    }
    return 0;
}