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
#ifndef I2C_H_
#define I2C_H_

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
/**
 * @brief   Macro to determine if an I2C device is defined in the system
 * @param [in] DEV:  I2C_Device_t type I2C device specifier.
 * @retval
 *    1: Device exists and is valid
 *    0: Device doesn't exist or isn't defined
 */
#define IS_I2C_DEVICE(DEV) ((DEV) == EEPROM || (DEV) == SN_ROM || (DEV) == UIE_ROM)

/* Exported types ------------------------------------------------------------*/

/**
 * \enum I2C_Device_t
 * I2C Devices available on the system.
 */
typedef enum I2C_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C */
   SN_ROM,
   UIE_ROM,
   /* Insert more I2C device enumerations here... */
   MAX_I2C_DEV     /**< Maximum number of available I2C devices on the system */
} I2C_Device_t;

/**
 * \struct I2C_BusDevice_t
 * Settings for the various I2C devices that are attached to any of the I2C busses.
 */
typedef struct I2C_BusDevices
{
   /* "External" device settings */
   const I2C_Device_t      i2c_dev;          /**< System I2C device specifier */
   const I2C_TypeDef *     i2c_bus;        /**< I2C bus device is attached to */
   const uint16_t          i2c_dev_addr;  /**< I2C Device address of the device */

   /* Internal device settings */
   const uint8_t           i2c_mem_addr_size; /**< How many bytes are used to specify internal mem addr of the device */
   uint16_t                i2c_mem_addr;  /**< I2C last accessed mem address  */

} I2C_BusDevice_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
