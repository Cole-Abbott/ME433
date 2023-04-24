#include "nu32dip.h"
#include "i2c_master_noint.h"

void GPIO7Set(char);
void chipInit(void);
char GPIO0Read(void);

int main(void) {
    NU32DIP_Startup();
    i2c_master_setup();

    chipInit();

    while(1) {
        NU32DIP_GREEN = !NU32DIP_GREEN; //heartbeat

        //delay 10ms
        int time = _CP0_GET_COUNT(); 
        while(_CP0_GET_COUNT() - time < 240000) { ; }

        //read GPIO0
        //if high, set GPIO7 low (led on)
        //if low, set GPIO7 high
        if(GPIO0Read()) {
            GPIO7Set(0); 
        } else {
            GPIO7Set(1);
        }
    }

}


void chipInit(void) {
    char message;
    //start bit
    i2c_master_start();

    //device address + W bit
    message = 0b01000000;
    i2c_master_send(message);

    //register address
        //IODIR -> 0x00
    message = 0x00;
    i2c_master_send(message);

    //data
        //-> IO7 output, IO0 input, everything else input
        // 1 input, 0 output
    message = 0b01111111; //all output
    i2c_master_send(message);

    //stop bit
    i2c_master_stop();
    
}

void GPIO7Set(char state) {
    char message;
    //start bit
    i2c_master_start();

    //device address + W bit
    message = 0b01000000;
    i2c_master_send(message);

    //register address
        //OLAT 0x0A
    message = 0x0A;
    i2c_master_send(message);

    //data
    message = state << 7;

    i2c_master_send(message);

    //stop bit
    i2c_master_stop();
}

char GPIO0Read(void) {
    char message;
    //start bit
    i2c_master_start();

    //device address + W bit
    message = 0b01000000;
    i2c_master_send(message);

    //register address
        //OLAT 0x09
    message = 0x09;
    i2c_master_send(message);

    //reset
    i2c_master_restart();

    //device address + R bit
    message = 0b01000001;
    i2c_master_send(message);

    //register address
        //OLAT 0x09
    //message = 0x09;
    //i2c_master_send(message);

    //read response
    message = i2c_master_recv();
    i2c_master_ack(1);

    //stop bit
    i2c_master_stop();

    //return last bit
    return (message & 0x01);
}