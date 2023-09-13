/*
 * i2.c
 *

 */

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include "powercontrol.h"
#include "clock.h"
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Tx and Rx Buffer, write and read lengths are used in I2C Master mode */
volatile uint8_t I2CMasterTxBuffer[70], I2CMasterRxBuffer[70];
volatile uint8_t I2CMasterReadLength, I2CMasterWriteLength;
volatile uint8_t I2CMasterWriteIndex=0,I2CMasterReadIndex=0;
volatile uint8_t I2CMasterState = I2C_IDLE;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
  * @brief     Setting power ON/OFF for I2C Peripheral
  * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
  * @param[in] mode value can be either POWERON/ POWEROFF
  * @return    No return value
  **/
void vI2CPowerControl(uint8_t ucI2CNum,uint8_t mode)
{
	switch(ucI2CNum)
	{
	case  I2C_0 :
		if (mode == POWERON)
			LPC_SC->PCONP |= 1 << BIT_PCONP_PCI2C0;
		else if (mode == POWEROFF)
			LPC_SC->PCONP &= ~(1 << BIT_PCONP_PCI2C0);
		break;
	case  I2C_1:
		if (mode == POWERON)
			LPC_SC->PCONP |= 1 << BIT_PCONP_PCI2C1;
		else if (mode == POWEROFF)
			LPC_SC->PCONP &= ~(1 << BIT_PCONP_PCI2C1);
		break;
	case  I2C_2:
		if (mode == POWERON)
			LPC_SC->PCONP |= 1 << BIT_PCONP_PCI2C2;
		else if (mode == POWEROFF)
			LPC_SC->PCONP &= ~(1 << BIT_PCONP_PCI2C2);
		break;
		default :
		/* DO NOTHING */
		break;
	}
}


/**
 * @brief     Setting clock for I2C Peripheral
 * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
 * @param[in] clockmode value can be ONEFOURTH,SAME,HALF,ONEEIGTH
 * @return    No return value
 **/
void vI2CClockControl(uint8_t ucI2CNum,uint8_t clockmode)
{
	switch(ucI2CNum)
	{
	case I2C_0:
		LPC_SC->PCLKSEL0 &= ~(3 << BIT_PCLKSEL0_PCLK_I2C0 );
		LPC_SC->PCLKSEL0 |= clockmode << BIT_PCLKSEL0_PCLK_I2C0 ;
		break;
	case I2C_1:
		LPC_SC->PCLKSEL1 &= ~(3 << BIT_PCLKSEL1_PCLK_I2C1);
		LPC_SC->PCLKSEL1 |= clockmode << BIT_PCLKSEL1_PCLK_I2C1;
		break;
	case I2C_2:
		LPC_SC->PCLKSEL1 &= ~(3 << BIT_PCLKSEL1_PCLK_I2C2 );
		LPC_SC->PCLKSEL1 |= clockmode << BIT_PCLKSEL1_PCLK_I2C2 ;
		break;
		default :
		/* DO NOTHING */
		break;
	}
}

/**
 * @brief     Setting clock frequency (baud rate) for I2C SCL
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @param[in] sclclk value can be I2C_100KHz,I2C_400KHz
 * @return    No return value
 **/

void vI2CSerialClkFreq(LPC_I2C_TypeDef *I2Cx, uint8_t sclclk)
{
	switch (sclclk) {
	case I2C_100KHz:
		I2Cx->I2SCLL = 125;
		I2Cx->I2SCLH = 125;
		break;
	case I2C_400KHz:
		I2Cx->I2SCLL = 31;
		I2Cx->I2SCLH = 31;
		break;
	}
}

/**
 * @brief     Enabling the selected I2C Peripheral
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * */

void vI2CEnable(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR=1<<BIT_I2CONCLR_AAC | 1<<BIT_I2CONCLR_SIC | 1<<BIT_I2CONCLR_STAC;
	I2Cx->I2CONSET=1<<BIT_I2CONSET_I2EN;
}

/**
 * @brief     Setting clock frequency (baud rate) for I2C SCL
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
 * @param[in] sclclk value can be I2C_100KHz,I2C_400KHz
 * @return    No return value
 **/
void vI2CMasterInitialize(LPC_I2C_TypeDef *I2Cx,uint8_t i2cnum,uint8_t sclclk )
{
	vI2CPowerControl(i2cnum, POWERON);
	vI2CClockControl(i2cnum, ONEFOURTH);
	vI2CSerialClkFreq(I2Cx,sclclk);
	vI2CEnable(I2Cx);
	switch (i2cnum) {
	case I2C_0:
		NVIC_EnableIRQ(I2C0_IRQn);
		break;
	case I2C_1:
		NVIC_EnableIRQ(I2C1_IRQn);
		break;
	case I2C_2:
		NVIC_EnableIRQ(I2C2_IRQn);
		break;
	}
}
/**
 * @brief     Call back function when selected  I2C acts as Master
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
void vI2CMasterStateMachine(LPC_I2C_TypeDef *I2Cx){

	volatile uint8_t I2Cstatus = 0;

	/* Reading the status from the I2C status register to drive the state machine */
	I2Cstatus = I2Cx->I2STAT;
	LPC_UART0->THR = I2Cstatus;

	switch(I2Cstatus)
	{

		case 0x08:
		/* A START condition has been transmitted. The Slave Address + R/W bit will now be transmitted. */

		/*1. Write Slave Address with R/W bit to I2DAT. */
		I2Cx->I2DAT = I2CMasterTxBuffer[0];
		/*2. Write 0x04 to I2CONSET to set the AA bit.  */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;
		/*3. Write 0x08 to I2CONCLR to clear the SI and also clear STA flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC | 1<<BIT_I2CONCLR_STAC;
		/*4. Set up Master Transmit mode data buffer. */
		/* DO NOTHING */
		/* 5. Set up Master Receive mode data buffer. */
		/* DO NOTHING */
		/*6. Initialize Master data counter. */
		I2CMasterWriteIndex = 1;
		I2CMasterReadIndex = 0;


		I2CMasterState= I2C_STARTED;
		break;

		case 0x10:
		/* A repeated START condition has been transmitted. The Slave Address + R/W bit will now be transmitted. */

		/* 1. Write Slave Address with R/W bit to I2DAT. */
		I2Cx->I2DAT = I2CMasterTxBuffer[I2CMasterWriteIndex];
		/*2. Write 0x04 to I2CONSET to set the AA bit.  */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;
		/*3. Write 0x08 to I2CONCLR to clear the SI flag and also clear STA flag.. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC | 1<<BIT_I2CONCLR_STAC;
		/* 4. Set up Master Transmit mode data buffer. */
		/* DO NOTHING */
		/* 5. Set up Master Receive mode data buffer. */
		/* DO NOTHING */
		/* 6. Initialize Master data counter. */
		I2CMasterReadIndex = 0;

		I2CMasterState= I2C_REPEATED_START;
		break;

		/*********************************I2C MASTER TRANSMIT MODE STATES*************************************/
		case 0x18:
		/*Previous state was State 0x08 or State 0x10, Slave Address + Write has been
		 transmitted, ACK has been received. The first data byte will be transmitted. */

		/*1. Load I2DAT with first data byte from Master Transmit buffer. */
		I2Cx->I2DAT = I2CMasterTxBuffer[I2CMasterWriteIndex];
		/*2. Write 0x04 to I2CONSET to set the AA bit. */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;
		/*3. Write 0x08 to I2CONCLR to clear the SI flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;
		/*4. Increment Master Transmit buffer pointer.*/
		I2CMasterWriteIndex++;

		I2CMasterState= DATA_ACK;
		break;

		case 0x20:
		/*Slave Address + Write has been transmitted, NOT ACK has been received.
		 * A STOP condition will be transmitted. */

		/* 1. Write 0x14 to I2CONSET to set the STO and AA bits. */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO | 1<<BIT_I2CONSET_AA;
		/* 2. Write 0x08 to I2CONCLR to clear the SI flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;

		I2CMasterState= DATA_NACK;

		break;

		case 0x28:
		/* Data has been transmitted, ACK has been received. If the transmitted data was the last
		 data byte then transmit a STOP condition, otherwise transmit the next data byte. */

		/*1. Decrement the Master data counter, skip to step 5 if not the last data byte.*/
		if (I2CMasterWriteIndex < I2CMasterWriteLength) {
			/*5. Load I2DAT with next data byte from Master Transmit buffer.*/
			I2Cx->I2DAT = I2CMasterTxBuffer[I2CMasterWriteIndex];
			/*6. Write 0x04 to I2CONSET to set the AA bit.*/
			I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;
			/*7. Write 0x08 to I2CONCLR to clear the SI flag. */
			I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;
			/*8. Increment Master Transmit buffer pointer */
			I2CMasterWriteIndex++;

			I2CMasterState= DATA_ACK;
		} else {
			/* Sending the repeated start condition as we going ahead with a read operation */
			if (I2CMasterReadLength != 0)
				I2Cx->I2CONSET = 1<<BIT_I2CONSET_STA;
			else {
				/*2. Write 0x14 to I2CONSET to set the STO and AA bits.*/
				I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO | 1<<BIT_I2CONSET_AA;
				I2CMasterState= DATA_NACK;
			}
			/*3. Write 0x08 to I2CONCLR to clear the SI flag.*/
				I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;
		}
		break;
		case 0x30:
		/* Data has been transmitted, NOT ACK received. A STOP condition will be transmitted */

		/*1. Write 0x14 to I2CONSET to set the STO and AA bits. */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO | 1<<BIT_I2CONSET_AA;
		/*2. Write 0x08 to I2CONCLR to clear the SI flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;

		I2CMasterState= DATA_NACK;
		break;

		case 0x38:
		/* Arbitration has been lost during Slave Address + Write or data. The bus has been
		 released and not addressed Slave mode is entered. A new START condition will be transmitted
		 when the bus is free again. */

		/*1. Write 0x24 to I2CONSET to set the STA and AA bits. */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_STA | 1<<BIT_I2CONSET_AA;
		/*2. Write 0x08 to I2CONCLR to clear the SI flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;

		I2CMasterState= DATA_NACK;
		break;

		/*********************************I2C MASTER RECEIVE MODE STATES**************************************/
		case 0x40:/*Previous state was State 08 or State 10. Slave Address + Read has been transmitted,
		 	 	  ACK has been received. Data will be received and ACK returned.*/

		/* Master is sending no acknowledge as it interested to read only one byte from slave */
		if (I2CMasterReadLength == 1)
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_AAC;
		else
		/* Master is sending  acknowledge as it interested to read more bytes from slave */
		/*1. Write 0x04 to I2CONSET to set the AA bit. */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;

		/*2. Write 0x08 to I2CONCLR to clear the SI flag. */
		I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;
		break;

		case 0x48:	/*Slave Address + Read has been transmitted, NOT ACK has been received.
					  A STOP condition will be transmitted.*/

		/*1. Write 0x14 to I2CONSET to set the STO and AA bits. */
			I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO | 1<<BIT_I2CONSET_AA;
		/*2. Write 0x08 to I2CONCLR to clear the SI flag. */
			I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;

			I2CMasterState = I2C_SLA_NACK;
		break;

		case 0x50:	/*Data has been received, ACK has been returned. Data will be read from I2DAT. Additional
		 	 	 data will be received. If this is the last data byte then NOT ACK will be returned, otherwise
		 	 	 ACK will be returned.*/

		/*1. Read data byte from I2DAT into Master Receive buffer. */
			I2CMasterRxBuffer[I2CMasterReadIndex]=I2Cx->I2DAT;

		/*2. Decrement the Master data counter, skip to step 5 if not the last data byte.*/
			I2CMasterReadIndex++;
			if(I2CMasterReadIndex<I2CMasterReadLength-1)
			{
				/*5. Write 0x04 to I2CONSET to set the AA bit.*/
							I2Cx->I2CONSET = 1<<BIT_I2CONSET_AA;
			/*6. Write 0x08 to I2CONCLR to clear the SI flag.*/
						I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;
					/*7. Increment Master Receive buffer pointer*/
						I2CMasterState = DATA_ACK;
			}
			else
			{
		/*3. Write 0x0C to I2CONCLR to clear the SI flag and the AA bit.*/
			I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC | 1<<BIT_I2CONCLR_AAC;
						I2CMasterState = DATA_NACK;
			}

		break;

		case 0x58:
		/* Data has been received, NOT ACK has been returned. Data will be read from I2DAT. A STOP condition will be
		 transmitted. */
		/*1. Read data byte from I2DAT into Master Receive buffer. */
			I2CMasterRxBuffer[I2CMasterReadIndex]=I2Cx->I2DAT;
		/*2. Write 0x14 to I2CONSET to set the STO and AA bits. */
			I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO | 1<<BIT_I2CONSET_AA;
		/*3. Write 0x08 to I2CONCLR to clear the SI flag. */
			I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC;

			I2CMasterState = DATA_NACK;
		 break;

		 }
}

/**
 * @brief     For Starting the I2C Transaction
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t  ucI2CStart(LPC_I2C_TypeDef *I2Cx)
{

		uint32_t timeout=0;
		/* Starting the I2C Transaction */
		I2Cx->I2CONSET = 1<<BIT_I2CONSET_STA;

		while(1)
		{
			timeout++;
			/* Waiting for the I2CMasterState to I2C_STARTED state for a timeout ,
			 * if it is not changed to I2C_STARTED state means within timeout ,
			 * I2C transaction is a failure */
			if(I2CMasterState == I2C_STARTED)
			{
					return PASS;
			}

			if(timeout> MAXTIMEOUT)
				return FAIL;
		}

}

/**
 * @brief     Function which waits until a current I2C transaction is complete
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t  ucI2CEngine(LPC_I2C_TypeDef *I2Cx)
{
				/* Making initial I2CMasterState as I2C_IDLE */
				I2CMasterState=I2C_IDLE;
				/* Clearing I2CMasterWriteIndex & I2CMasterReadIndex */
				I2CMasterWriteIndex=0;
				I2CMasterReadIndex=0;
				/* If ucI2CStart is false , end the transaction by ucI2CStop */
				if(ucI2CStart(I2Cx)==FAIL)
				{
					ucI2CStop(I2Cx);
					return FAIL;
				}
				while(1)
				{
				   if (I2CMasterState == DATA_NACK)
				   {
						ucI2CStop(I2Cx);
						break;
				   }
				}
				return PASS;
}

/**
 * @brief     Function called for ending the current I2C Transaction
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t ucI2CStop(LPC_I2C_TypeDef *I2Cx)
{

		/* Stop bit is set for the I2C transaction and indicates the end of I2C transaction */
			I2Cx->I2CONSET = 1<<BIT_I2CONSET_STO;
			I2Cx->I2CONCLR = 1<<BIT_I2CONCLR_SIC; /* Clear SI flag */
				/*--- Wait for STOP detected ---*/
				while (I2Cx->I2CONSET & 1<<BIT_I2CONSET_STO);
				return PASS;
}



void I2C0_IRQHandler(void)
{
	vI2CMasterStateMachine(I2C0);
}
void I2C1_IRQHandler(void)
{
	/* DO NOTHING */
}
void I2C2_IRQHandler(void)
{
	/* DO NOTHING */
}








