/*
 * eeprom.h
 *
 *
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "eeprom.h"
#include "i2c.h"
#include <stdint.h>

/* Connect the Ai A0 pins of EEPROM according to your choice */
#define GND_GND 0
#define GND_VCC 1
#define VCC_GND 2
#define VCC_VCC 3


#if A1_A0==GND_GND
#define AT24C256EEPROMADEVICEADDRESS     0x50
#elif A1_A0==GND_VCC
#define AT24C256EEPROMADEVICEADDRESS     0x51
#elif A1_A0==VCC_GND
#define AT24C256EEPROMADEVICEADDRESS     0x52
#elif A1_A0==VCC_VCC
#define AT24C256EEPROMADEVICEADDRESS     0x53
#endif

#define AT24C256MAXPAGESIZE              63


#define EEPROMI2C I2C0


/* Tx and Rx Buffer, write and read lengths are used in I2C Master mode */
extern volatile uint8_t I2CMasterTxBuffer[70], I2CMasterRxBuffer[70];
extern volatile uint8_t I2CMasterReadLength, I2CMasterWriteLength;

/**
  * @brief     Writing a byte to a selected device eeprom memory location
  * @param[in] bytetowrite byte value to write
  * @return    No return value
  **/
void vEEPROMByteWrite(uint16_t devicememoryaddress,uint8_t bytetowrite);

/* Function for writing bytes of data from the buffer into the selected starting memory location */
/**
  * @brief     Writing bytes of data from the buffer into the selected starting memory location, max is 63 bytes
  * @param[in] ptrtobufferofbytes pointer to the buffer containing bytes
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/
void vEEPROMPageWrite(uint16_t startingmemoryaddress,uint8_t* ptrtobufferofbytes, uint8_t bufferlength);


/**
  * @brief     Read a byte from the current location in which eeprom
  * @param[out]ptrtobuffer pointer to the buffer to which we need to store byte which is read from eeprom
  * @return    No return value
  **/

void vEEPROMCurrentAddressRead(uint8_t* ptrtobuffer);

/**
  * @brief     Reading a bytes of data to the buffer from the selected starting memory location
  * @param[out]ptrtobufferofbytes pointer to the buffer to which we need to store bytes which is read from eeprom
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/

void vEEPROMRandomAddressRead(uint16_t startingmemoryaddress,uint8_t* ptrtobufferofbytes, uint8_t bufferlength);

/**
  * @brief     Reading bytes of data to the buffer from the selected starting memory location
  * @param[out]ptrtobufferofbytes pointer to the buffer to which we need to store bytes which is read from eeprom
  * @param[in] bufferlength length of the buffer containing bytes
  * @return    No return value
  **/
void vEEPROMSequentialRead(uint8_t* ptrtobufferofbytes, uint8_t bufferlength);

#endif /* EEPROM_H_ */
