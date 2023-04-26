#include "nu32dip.h"
#include "ssd1306.h"
#include "i2c_master_noint.h"
#include "font.h"
#include "mpu6050.h"

void drawChar(char c, char x, char y);
void drawString(char *m, char x, char y);
void drawBall(float x, float y);

int main(void) {
    NU32DIP_Startup();
    i2c_master_setup();
    ssd1306_setup();
    init_mpu6050();

    //for IMU
    // char array for the raw data
    unsigned char d[14];
	// floats to store the data
    float ax, ay, az, gx, gy, gz, temp;

    //for display
    char m[200];
    sprintf(m, "Hello i am a\r\nPIC32MX170f265b\n\rI can write things\r\non this display");

    drawString(m, 0, 0);
    ssd1306_update();

    //delay 500ms
    int time = _CP0_GET_COUNT();
    while(_CP0_GET_COUNT() < time + 24000000/2) {
        ;
    }

    float ballx = 50, bally = 15;
    float ballvx = 0, ballvy = 0;
    float dt = .2, damping = 0.997;


    while(1) {
        static int time = 0;

        //record time to draw last frame
        time = _CP0_GET_COUNT() - time;
        sprintf(m, "%4.2ffps", 24000000.0 / time);

        //reset time for next frame
        time = _CP0_GET_COUNT();

        //clear display
        ssd1306_clear();

        //draw the fps count
        drawString(m, 70, 24);

        // read IMU
        burst_read_mpu6050(d);
		// convert data
        ax = conv_xXL(d);
        ay = conv_yXL(d);

        // print out the IMU data
        sprintf(m, "ax: %4.2f ay: %4.2f", ax, ay);
        drawString(m, 0, 0);

        // update ball position
        ballvx = ballvx - ax*dt; //calculate new velocity
        ballvy = ballvy + ay*dt;
        ballvx = ballvx*damping; //apply damping
        ballvy = ballvy*damping;
        ballx = ballx + ballvx*dt; //calculate new position
        bally = bally + ballvy*dt;
        // bounce off walls
        if(ballx > 126) {
            ballx = 126;
            ballvx = -ballvx;
        }
        if(ballx < 1) {
            ballx = 1;
            ballvx = -ballvx;
        }
        if(bally > 30) {
            bally = 30;
            ballvy = -ballvy;
        }
        if(bally < 1) {
            bally = 1;
            ballvy = -ballvy;
        }
        drawBall(ballx, bally);



        ssd1306_update();
        
    }

}


//writes a character to the display at the given x,y location
void drawChar(char c, char x, char y) {
    //loop through 5 cols
        
    for(int i = 0; i < 5; i++){
        //get col from font
        char col = ASCII[c - 0x20][i];
        //loop through 8 rows
        for(int j = 0; j < 8; j++){
            //get bit from col
            char bit = (col >> j) & 1;
            //if bit is 1, draw pixel at x+i, y+j
            ssd1306_drawPixel(x+i, y+j, bit);
        }
    }
}

//writes a string to the display at the given x,y location
void drawString(char *m, char x, char y) {
    //loop through string
        //drawChar(m[i], x+i, y
    int cursorX = x - 6, cursorY = y;
    int i = 0;
    while(m[i] != '\0') {
        if(m[i] == '\n') {
            cursorY += 8;
        } else if (m[i] == '\r') {
            cursorX = x - 6;
        } else {
            drawChar(m[i], cursorX+=6 , cursorY);
        }
        i++;
    }
}

//draws a 3x3 ball at the given x,y location
void drawBall(float ballX, float ballY) {
    int x = (int) ballX;
    int y = (int) ballY;

    ssd1306_drawPixel(x, y, 1);
    ssd1306_drawPixel(x+1, y, 1);
    ssd1306_drawPixel(x, y+1, 1);
    ssd1306_drawPixel(x+1, y+1, 1);
    ssd1306_drawPixel(x-1, y+1, 1);
    ssd1306_drawPixel(x+1, y-1, 1);
    ssd1306_drawPixel(x-1, y, 1);
    ssd1306_drawPixel(x, y-1, 1);
    ssd1306_drawPixel(x-1, y-1, 1);
}