/*
===============================================================================
 Name        : 35_Day81_17022022_I2C001.c

 Version     :
 Copyright   : $(copyright)
 Description :

 Layered Architecture for Coding
 ************************************
 Application layer-35_Day81_17022022_I2C001.c
 ************************************
 Board layer - pinmux.c/.h, eeprom.c/.h
 ************************************
 Low level drivers or chip level -i2c.c/.h, uart.c/.h
 ************************************
 Hardware
 ************************************

===============================================================================
*/

/* Header includes */
#include "pinmux.h"
#include "uart.h"
#include "i2c.h"
#include "eeprom.h"

/* Hardware initialization used in the application */
void vAppHardwareInit(void) {

	vPinmuxInitialize();
	vUARTInitialize(UART0,UART_0,BAUDRATE_9600);
	/* Attaching the vUARTCallbackHandler function when interrupt occurs in UART0 */
	vUARTIntAttachCallback(UART_0,vUARTCallbackHandler);
	vUARTInterruptDisable(UART0,THRE_INTERRUPT);
	vUARTInterruptDisable(UART0, RBR_INTERRUPT);
	vI2CMasterInitialize(I2C0, I2C_0, I2C_100KHz);
}

/* Crude delay implementation for generating a delay */
void vAppDelay(uint32_t count)
{
	int i,j;
	for(i=0;i<count;i++)
		for(j=0;j<0xA00;j++);

}

/* Application Entry Point */
int main(void) {
	/* Variable declarations and initializations */

	uint8_t buffer[]="ready";
	uint8_t readbyte=0;
    uint8_t rxbuffer[10];
	/* Hardware Initialization */
	vAppHardwareInit();

	vUARTPrintfBlocking(UART0,"i2C EEPROM TESTING\r\n");
    vEEPROMByteWrite(0x0050,'H');
	/* Delay required for EEPROM to do the internal write operations */
	vAppDelay(0xA00);
	vEEPROMCurrentAddressRead(&readbyte);
	vUARTPrintfBlocking(UART0,"%c",readbyte);
    vEEPROMPageWrite(0x00A0,buffer,6);
	/* Delay required for EEPROM to do the internal write operations */
	vAppDelay(0xA00);
	vAppDelay(0xA00);
	vEEPROMRandomAddressRead(0x00A0,rxbuffer,6);
	vUARTPrintfBlocking(UART0,"%s",rxbuffer);

	vEEPROMSequentialRead(rxbuffer,4);
	vUARTPrintfBlocking(UART0,"%c",rxbuffer[0]);
	vUARTPrintfBlocking(UART0,"%c",rxbuffer[1]);
	vUARTPrintfBlocking(UART0,"%c",rxbuffer[2]);
	vUARTPrintfBlocking(UART0,"%c",rxbuffer[3]);

	vUARTPrintfBlocking(UART0,"i2C EEPROM TEST FINISHED\r\n");

	/* Infinite loop */
	while (1) {


	}
	/* Code will never reach here */
}
