#ifndef SPI__H__
#define SPI__H__

void initSPI();
unsigned char spi_io(unsigned char o);

#define CHIP_SELECT LATBbits.LATB9
#define NU32DIP_USER PORTAbits.RA4

#endif // SPI__H__