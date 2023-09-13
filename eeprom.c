/*
 * eeprom.c
 *

 */

#include "eeprom.h"

/**
  * @brief     Writing a byte to a selected device eeprom memory location
  * @param[in] bytetowrite byte value to write
  * @return    No return value
  **/
void vEEPROMByteWrite(uint16_t devicememoryaddress,uint8_t bytetowrite)
{
	    I2CMasterReadLength = 0; /* Nothing do with read length*/
		I2CMasterWriteLength = 4; /* 4 bytes -> SLA+MSB memory address + LSB memory address +byte  */
		/* Placing the EEPROM Slave Address in write mode into the I2C Master tx buffer */
		I2CMasterTxBuffer[0] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CWRITEOPERATION;
		/* Placing the EEPROM device MSB of the memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[1] = devicememoryaddress >> 8 & 0xFF;
		/* Placing the EEPROM device LSB of the memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[2] = devicememoryaddress & 0xFF;
		/* Placing the byte to be written to the selected memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[3] = bytetowrite;
		/* Starting the I2C Engine */
		ucI2CEngine(EEPROMI2C);

}

/* Function for writing bytes of data from the buffer into the selected starting memory location */
/**
  * @brief     Writing bytes of data from the buffer into the selected starting memory location, max is 63 bytes
  * @param[in] ptrtobufferofbytes pointer to the buffer containing bytes
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/
void vEEPROMPageWrite(uint16_t startingmemoryaddress,uint8_t* ptrtobufferofbytes, uint8_t bufferlength)
{
	int i=0;

    I2CMasterReadLength = 0; /* Nothing do with read length*/

    if(bufferlength<AT24C256MAXPAGESIZE)
    	I2CMasterWriteLength = (bufferlength-1) +3; /* SLA in write mode +MSB memory address + LSB memory address + bufferlength */

    /* Placing the EEPROM Slave Address in write mode into the I2C Master tx buffer */
	I2CMasterTxBuffer[0] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CWRITEOPERATION;
	/* Placing the EEPROM device MSB of the memory address into the I2C Master tx buffer */
	I2CMasterTxBuffer[1] = startingmemoryaddress >> 8 & 0xFF;
	/* Placing the EEPROM device LSB of the memory address into the I2C Master tx buffer */
	I2CMasterTxBuffer[2] = startingmemoryaddress & 0xFF;
	/* Placing the byte to be written to the selected memory address into the I2C Master tx buffer */

	for(i=0;i<bufferlength;i++)
	{
		/* Placing the bytes to be written to the selected memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[i+3] = ptrtobufferofbytes[i];

	}
	/* Starting the I2C Engine */
	ucI2CEngine(EEPROMI2C);
}

/**
  * @brief     Read a byte from the current location in which eeprom
  * @param[out]ptrtobuffer pointer to the buffer to which we need to store byte which is read from eeprom
  * @return    No return value
  **/

void vEEPROMCurrentAddressRead(uint8_t* ptrtobuffer)
{
	    I2CMasterReadLength  = 1; /* Reading a byte */
		I2CMasterWriteLength = 1; /* For writing the EEPROM slave address in read mode */

		/* Placing the EEPROM Slave Address in read mode into the I2C Master tx buffer */
		I2CMasterTxBuffer[0] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CREADOPERATION;

		/* Starting the I2C Engine */
		ucI2CEngine(EEPROMI2C);

		 /* Placing the received byte from I2C Master Rx buffer into our application buffer */
		*ptrtobuffer =I2CMasterRxBuffer[0];
}



/**
  * @brief     Reading a bytes of data to the buffer from the selected starting memory location
  * @param[out]ptrtobufferofbytes pointer to the buffer to which we need to store bytes which is read from eeprom
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/

void vEEPROMRandomAddressRead(uint16_t startingmemoryaddress,uint8_t* ptrtobufferofbytes, uint8_t bufferlength)
{
	int i = 0;

		if (bufferlength <= AT24C256MAXPAGESIZE)
			I2CMasterWriteLength = 3; /* 3bytes -> SLA in Write Mode +MSB memory address + LSB memory address + SLA in Read Mode*/

		/* Placing the EEPROM Slave Address in write mode into the I2C Master tx buffer */
		I2CMasterTxBuffer[0] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CWRITEOPERATION;
		/* Placing the EEPROM device MSB of the memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[1] = startingmemoryaddress >> 8 & 0xFF;
		/* Placing the EEPROM device LSB of the memory address into the I2C Master tx buffer */
		I2CMasterTxBuffer[2] = startingmemoryaddress & 0xFF;
		/* Placing the EEPROM Slave Address in read mode into the I2C Master tx buffer */
		I2CMasterTxBuffer[3] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CREADOPERATION;

		I2CMasterReadLength = bufferlength; /* Reading the number bytes mentioned in the buffer length */

	   /* Starting the I2C Engine */
		ucI2CEngine(EEPROMI2C);

		 /* Placing the received bytes from I2C Master Rx buffer into our application buffer */
		for(i=0;i<bufferlength;i++)
			ptrtobufferofbytes[i] =I2CMasterRxBuffer[i];

}



/**
  * @brief     Reading bytes of data to the buffer from the selected starting memory location
  * @param[out]ptrtobufferofbytes pointer to the buffer to which we need to store bytes which is read from eeprom
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/
void vEEPROMSequentialRead(uint8_t* ptrtobufferofbytes, uint8_t bufferlength)
{

  /* TODO*/
	int i = 0;

	if (bufferlength <= AT24C256MAXPAGESIZE)
		I2CMasterWriteLength = 1;

	I2CMasterReadLength = bufferlength; /* Nothing do with read length*/
	/* Placing the EEPROM Slave Address in read mode into the I2C Master tx buffer */
	I2CMasterTxBuffer[0] = AT24C256EEPROMADEVICEADDRESS << 1 | I2CREADOPERATION;

	/* Starting the I2C Engine */
	ucI2CEngine(EEPROMI2C);

	/* Placing the received bytes from I2C Master Rx buffer into our application buffer */
	for (i = 0; i < bufferlength; i++)
		ptrtobufferofbytes[i] = I2CMasterRxBuffer[i];

}
