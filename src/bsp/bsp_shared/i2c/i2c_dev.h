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

/**
 * @brief   A blocking function to read I2C data that should be called from
 * FreeRTOS threads.
 *
 * This function sends and event to the I2CxDevMgr AO to read data from an I2C
 * device and then suspends the calling task.  Once the AO is finished processing
 * the request, it will put an event with the data (or error) into the raw queue
 * and wake the task.  This function then resumes and retrieves the data out of
 * the queue, parses it, fills in the appropriate buffers and status and returns.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] offset: uint8_t offset from beginning of the device where to read
 *             the data from.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [out] *pBytesRead: number of bytes read as returned from the
 *             I2CxDevMgr AO that performs the actual read operation.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesRead,
      uint16_t nBytesToRead
);

/**
 * @brief   A blocking function to write I2C data that should be called from
 * FreeRTOS threads.
 *
 * This function sends and event to the I2CxDevMgr AO to write data to an I2C
 * device and then suspends the calling task.  Once the AO is finished processing
 * the request, it will put an event with the data (or error) into the raw queue
 * and wake the task.  This function then resumes and retrieves the data out of
 * the queue, parses it, fills in the appropriate buffers and status and returns.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 * @param [in] offset: uint8_t offset from beginning of the device where to write
 *             the data to.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [in] *pBuffer: uint8_t pointer to the buffer containing data to write.
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [out] *pBytesWrite: number of bytes written as returned from the
 *             I2CxDevMgr AO that performs the actual write operation.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 * @return CBErrorCode: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesWritten,
      uint16_t nBytesToWrite
);

/**
 * @brief  Posts an event read a block of data from a memory device on any I2C bus.
 *
 * @note:  This function should only be called from RTOS controlled thread/AO.
 * It is non-blocking and instantly returns.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] addr: internal memory address of the device on the I2C bus.
 * @param [in] bytesToRead : uint8_t variable specifying how many bytes to read.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @param [in] *callingAO: QActive pointer to the AO that called this function.
 *                         If called by a FreeRTOS thread, this should be NULL.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readDevMemEVT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToRead,
      AccessType_t accType,
      QActive* callingAO
);

/**
 * @brief  Posts an event write a block of data to a memory device on any I2C bus.
 *
 * @note:  This function should only be called from RTOS controlled thread/AO.
 * It is non-blocking and instantly returns.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 * @param [in] offset: uint16_t offset from beginning of device memory to write
 * to.
 * @param [in] bytesToWrite : uint16_t variable specifying how many bytes to write.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @param [in] *callingAO: QActive pointer to the AO that called this function.
 *                         If called by a FreeRTOS thread, this should be NULL.
 * @param [in] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeDevMemEVT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToWrite,
      AccessType_t accType,
      QActive* callingAO,
      uint8_t *pBuffer
);

/******************************************************************************/
/* Blocking Functions - Don't call unless before threads/AOs started or after */
/* they crashed                                                               */
/******************************************************************************/

/**
 * @brief  Blocking function to read a block of data from a memory device on
 * any I2C bus.
 *
 * @note:  This is a slow function that should not be called by any threads or
 * objects. It's for use in case of crashes and before the entire system has
 * come up.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] offset: uint16_t offset from beginning of device memory to read
 * from.
 * @param [in] bytesToRead : uint16_t variable specifying how many bytes to read.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in] bufSize: uint8_t maximum size of storage pointed to by *pBuffer
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readDevMemBLK(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToRead,
      AccessType_t accType,
      uint8_t* pBuffer,
      uint8_t  bufSize
);

/**
 * @brief  Blocking function to write a block of data to a memory device on
 * any I2C bus.
 *
 * @note:  This is a slow function that should not be called by any threads or
 * objects. It's for use in case of crashes and before the entire system has
 * come up.
 *
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] offset: uint16_t offset from beginning of device memory to write
 * to.
 * @param [in] bytesToWrite : uint16_t variable specifying how many bytes to write.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @param [out] *pBuffer: uint8_t pointer to the buffer to write.
 * @param [in] bufSize: uint8_t maximum size of storage pointed to by *pBuffer
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeDevMemBLK(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToWrite,
      AccessType_t accType,
      uint8_t* pBuffer,
      uint8_t  bufSize
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
