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
#include "i2c.h"
#include "Shared.h"
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Exported defines ----------------------------------------------------------*/

#define EEPROM_PAGE_SIZE   16 /**< Size of the page in bytes on the EEPROM */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief I2C_Device_t
 * I2C Devices available on the I2CBus1 (I2C1) bus.
 */
typedef enum I2CBus1_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C */
   SN_ROM,
   EUI_ROM,
   /* Insert more I2CBus1 device enumerations here... */
   MAX_I2C1_DEV     /**< Maximum number of available I2C devices on I2CBus1 */
} I2CBus1_Dev_t;

/**
 * @brief I2CBus1_Device_t
 * Settings for the various I2C devices that are attached to the I2CBus1 bus.
 */
typedef struct I2CBus1_DeviceSettings
{
   /* "External" device settings */
   const I2CBus1_Dev_t     i2c_dev;          /**< System I2C device specifier */
   const uint16_t          i2c_dev_addr_size;  /**< Size of the I2C device address */
   const uint16_t          i2c_dev_addr;  /**< I2C Device address of the device */

   /* Internal device settings */
   const uint8_t           i2c_mem_addr_size; /**< Size of mem addr on device */
   uint16_t                i2c_mem_addr;   /**< I2C last accessed mem address */
   const uint16_t          i2c_mem_min_addr;/**< The first address that can be accessed */
   const uint16_t          i2c_mem_max_addr;/**< The last address that can be accessed */
   const uint8_t           i2c_mem_page_size; /**< Size of a page of memory */

} I2CBus1_DevSettings_t;

/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C1 device is defined in the system
 * @param [in] DEV:  I2CBus1_Dev_t type I2C device specifier.
 * @retval
 *    1: Device exists and is valid
 *    0: Device doesn't exist or isn't defined
 */
#define IS_I2C1_DEVICE( DEV )                                                 \
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
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t device address size of the requested device.
 */
uint8_t I2C_getI2C1DevAddrSize( I2CBus1_Dev_t iDev );

/**
 * @brief   Get the device address for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t device address on the I2C1 Bus.
 */
uint16_t I2C_getI2C1DevAddr( I2CBus1_Dev_t iDev );

/**
 * @brief   Get the internal memory address size for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t internal memory address size of the requested device.
 */
uint8_t I2C_getI2C1MemAddrSize( I2CBus1_Dev_t iDev );

/**
 * @brief   Get the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory address on the I2C1 Device.
 */
uint16_t I2C_getI2C1MemAddr( I2CBus1_Dev_t iDev );


/**
 * @brief   Set the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * This is to keep track of where the last access left off so continuous reads
 * don't have to keep telling the device where to read from.  Saves 2 I2C bus
 * operations per access.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @param [in]  addr: uint16_t last accessed memory address.
 * @return: None
 */
void I2C_setI2C1MemAddr( I2CBus1_Dev_t iDev, uint16_t addr );

/**
 * @brief   Get the first accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t first memory address on the I2C1 device.
 */
uint16_t I2C_getI2C1MinMemAddr( I2CBus1_Dev_t iDev );

/**
 * @brief   Get the last accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t last memory address on the I2C1 device.
 */
uint16_t I2C_getI2C1MaxMemAddr( I2CBus1_Dev_t iDev );

/**
 * @brief   Get the page size for memory for the specified I2CBus1 device.
 *
 * @param [in]  iDev: I2CBus1_Dev_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory page size on the I2C1 device.
 */
uint8_t I2C_getI2C1PageSize( I2CBus1_Dev_t iDev );

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                          /* I2C_DEV_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
