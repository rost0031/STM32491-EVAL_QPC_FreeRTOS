/**
 * @file    i2c_dev.h
 * @brief   Data for devices on the I2C Busses.
 *
 * @date    10/28/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_DEV_H_
#define I2C_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp_defs.h"
#include "i2c_defs.h"
#include "Shared.h"
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C device is defined in the system
 * @param [in] DEV:  I2C_Dev_t type I2C device specifier.
 * @retval
 *    1: Device exists and is valid
 *    0: Device doesn't exist or isn't defined
 */
#define IS_I2C_DEVICE( DEV )                                                  \
(                                                                             \
   (DEV) == EEPROM ||                                                         \
   (DEV) == SN_ROM ||                                                         \
   (DEV) == EUI_ROM                                                           \
)

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Get the device address size for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t device address size of the requested device.
 */
uint8_t I2C_getDevAddrSize( I2C_Dev_t iDev );

/**
 * @brief   Get the device address for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t device address on the I2C Bus.
 */
uint16_t I2C_getDevAddr( I2C_Dev_t iDev );

/**
 * @brief   Get the internal memory address size for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t internal memory address size of the requested device.
 */
uint8_t I2C_getMemAddrSize( I2C_Dev_t iDev );

/**
 * @brief   Get the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory address on the I2C Device.
 */
uint16_t I2C_getMemAddr( I2C_Dev_t iDev );


/**
 * @brief   Set the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * This is to keep track of where the last access left off so continuous reads
 * don't have to keep telling the device where to read from.  Saves 2 I2C bus
 * operations per access.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @param [in]  addr: uint16_t last accessed memory address.
 * @return: None
 */
void I2C_setI2CMemAddr( I2C_Dev_t iDev, uint16_t addr );

/**
 * @brief   Get the first accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t first memory address on the I2C device.
 */
uint16_t I2C_getMinMemAddr( I2C_Dev_t iDev );

/**
 * @brief   Get the last accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t last memory address on the I2C device.
 */
uint16_t I2C_getMaxMemAddr( I2C_Dev_t iDev );

/**
 * @brief   Get the page size for memory for the specified I2C device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory page size on the I2C device.
 */
uint8_t I2C_getPageSize( I2C_Dev_t iDev );

/**
 * @brief   Get the I2C bus of specified I2C device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory page size on the I2C device.
 */
I2C_Bus_t I2C_getBus( I2C_Dev_t iDev );

/**
 * @brief   Get the string representation of the I2C device.
 *
 * @param [in]  iDev: I2C_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: string representation of the I2C device.
 */
char* I2C_devToStr( I2C_Dev_t iDev );

/**
 * @brief   Calculate how to properly write large data over page boundaries in EEPROM.
 *
 * @param [out] *writeSizeFirstPage: uint8_t pointer to how many bytes to write
 *              on the first page of the EEPROM.
 * @param [out] *writeSizeLastPage: uint8_t pointer to how many bytes to write
 *              on the last page of the EEPROM.
 * @param [out] *writeTotalPages: uint8_t pointer to how many total pages will
 *              be written to the EEPROM.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 * @param [in] pageSize: uint16_t specifying the size of the page of the target
 * device.
 * @return CBErrorCode: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_calcPageWriteSizes(
      uint8_t *writeSizeFirstPage,
      uint8_t *writeSizeLastPage,
      uint8_t *writeTotalPages,
      uint16_t i2cMemAddr,
      uint16_t bytesToWrite,
      uint16_t pageSize
);

/******************************************************************************/
/* Blocking Functions - Don't call unless before threads/AOs started or after */
/* they crashed                                                               */
/******************************************************************************/

/**
 * @brief   Blocking function to read data from UI64 RO section of the EEPROM.
 *
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in]  offset: offset from beginning of EEPROM memory to read from.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 *
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readUi64RomBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
);

/**
 * @brief   Blocking function to read data from UI64 RO section of the EEPROM.
 *
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in]  offset: offset from beginning of EEPROM memory to read from.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 *
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readSnRomBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
);

/**
 * @brief   Blocking function to read data from EEPROM.
 *
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in]  offset: offset from beginning of EEPROM memory to read from.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 *
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readEepromBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
);

/**
 * @brief   Blocking function to write data to EEPROM.
 *
 * @param [out] *pBuffer: uint8_t pointer to the buffer of data to write.
 * @param [in]  offset: offset from beginning of EEPROM memory to write from.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 *
 * @return CBErrorCode: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeEepromBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToWrite
);

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                          /* I2C_DEV_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
