/*
 * i2c.h

 */

#ifndef I2C_H_
#define I2C_H_

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
#include "pinmux.h"
#include "configboard.h"
#include <stdarg.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
enum{I2C_0=0,I2C_1,I2C_2};
enum{MONITORMODE_DISABLE=0,MONITORMODE_ENABLE};
enum{SCL_DISABLE=0,SCL_ENABLE};
enum{INTERRUPT_MATCH=0,INTERRUPT_ANY_ADRESS};

enum{ I2C_100KHz=0,I2C_400KHz};

enum{ I2CWRITEOPERATION=0,I2CREADOPERATION};

enum{I2C_IDLE=0, I2C_STARTED, I2C_RESTARTED, I2C_REPEATED_START, DATA_ACK, DATA_NACK, I2C_WR_STARTED, I2C_RD_STARTED,I2C_SLA_NACK};

enum {FAIL=0,PASS};

#define MAXTIMEOUT 0x00FFFFFF

/* Private macro -------------------------------------------------------------*/
#define I2C0 LPC_I2C0
#define I2C1 LPC_I2C1
#define I2C2 LPC_I2C2

/* Bits in I2C Control Set register*/
#define BIT_I2CONSET_AA   2
#define BIT_I2CONSET_SI   3
#define BIT_I2CONSET_STO  4
#define BIT_I2CONSET_STA  5
#define BIT_I2CONSET_I2EN 6

/* Bits in I2C Control Clear register*/
#define BIT_I2CONCLR_AAC   2
#define BIT_I2CONCLR_SIC   3
#define BIT_I2CONCLR_STAC  5
#define BIT_I2CONCLR_I2ENC 6

/* Bits in I2C Status register*/
#define BIT_I2STAT_STATUS  3

/* Bits in I2C Data register*/
#define BIT_I2DAT_DATA  0

/* Bits in I2C Monitor mode control register*/
#define BIT_MMCTRL_MM_ENA    0
#define BIT_MMCTRL_ENA_SCL   1
#define BIT_MMCTRL_MATCH_ALL 2

/* Bits in I2C Data buffer register*/
#define BIT_I2DATA_BUFFER_Data 0

/* Bits in I2C Slave Address registers*/
#define BIT_I2ADR0_GC     0
#define BIT_I2ADR0_ADRESS 1

#define BIT_I2ADR1_GC     0
#define BIT_I2ADR1_ADRESS 1

#define BIT_I2ADR2_GC     0
#define BIT_I2ADR2_ADRESS 1

#define BIT_I2ADR3_GC     0
#define BIT_I2ADR3_ADRESS 1

/* Bits in I2C Mask registers*/
#define BIT_I2MASK0_MASK 1
#define BIT_I2MASK1_MASK 1
#define BIT_I2MASK2_MASK 1
#define BIT_I2MASK3_MASK 1

/* Bits in I2C SCL HIGH duty cycle register*/
#define BIT_I2SCLH_SCLH 0


/* Bits in I2C SCL Low duty cycle register*/
#define BIT_I2SCLL_SCLL 0

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief     Setting power ON/OFF for I2C Peripheral
  * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
  * @param[in] mode value can be either POWERON/ POWEROFF
  * @return    No return value
  **/
void vI2CPowerControl(uint8_t ucI2CNum,uint8_t mode);

/**
 * @brief     Setting clock for I2C Peripheral
 * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
 * @param[in] clockmode value can be ONEFOURTH,SAME,HALF,ONEEIGTH
 * @return    No return value
 **/
void vI2CClockControl(uint8_t ucI2CNum,uint8_t clockmode);
/**
 * @brief     Setting clock frequency (baud rate) for I2C SCL
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @param[in] sclclk value can be I2C_100KHz,I2C_400KHz
 * @return    No return value
 **/

void vI2CSerialClkFreq(LPC_I2C_TypeDef *I2Cx, uint8_t sclclk);
/**
 * @brief     Enabling the selected I2C Peripheral
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * */

void vI2CEnable(LPC_I2C_TypeDef *I2Cx);

/**
 * @brief     Setting clock frequency (baud rate) for I2C SCL
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @param[in] ucI2CNum value can be I2C_0,I2C_1,I2C_2
 * @param[in] sclclk value can be I2C_100KHz,I2C_400KHz
 * @return    No return value
 **/
void vI2CMasterInitialize(LPC_I2C_TypeDef *I2Cx,uint8_t i2cnum,uint8_t sclclk );


/**
 * @brief     For Starting the I2C Transaction
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t  ucI2CStart(LPC_I2C_TypeDef *I2Cx);

/**
 * @brief     Function which waits until a current I2C transaction is complete
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t  ucI2CEngine(LPC_I2C_TypeDef *I2Cx);

/**
 * @brief     Function called for ending the current I2C Transaction
 * @param[in] I2Cx base address of the selected I2C value can be I2C0,I2C1,I2C2
 * @return    No return value
 **/
uint8_t ucI2CStop(LPC_I2C_TypeDef *I2Cx);



#endif /* I2C_H_ */
