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

/** @defgroup STM324x9I_EVAL_IOE_Exported_Constants
  * @{
  */

/**
  * @brief  The 7 bits IO Expanders addresses and chip IDs
  */
#define IOE16_ADDR                 0x84
#define STMPE1600_ID               0x1600

/*------------------------------------------------------------------------------
    Functional and Interrupt Management
------------------------------------------------------------------------------*/

/**
  * @brief  Interrupt enable
  */
#define IOE16_IT_EN                0x04



/*------------------------------------------------------------------------------
    STMPE1600 device register definition
------------------------------------------------------------------------------*/
/**
  * @brief  Identification registers
  */
#define IOE16_REG_CHP_ID_LSB       0x00
#define IOE16_REG_CHP_ID_MSB       0x01
#define IOE16_REG_ID_VER           0x02

/**
  * @brief  System Control Register
  */
#define IOE16_REG_SYS_CTRL         0x03

/**
  * @brief  Interrupt Control register
  */
#define IOE16_REG_IEGPIOR_LSB      0x08
#define IOE16_REG_IEGPIOR_MSB      0x09
#define IOE16_REG_ISGPIOR_LSB      0x0A
#define IOE16_REG_ISGPIOR_MSB      0x0B


/**
  * @brief  GPIO Registers
  */

#define IOE16_REG_GPMR_LSB         0x10
#define IOE16_REG_GPMR_MSB         0x11
#define IOE16_REG_GPSR_LSB         0x12
#define IOE16_REG_GPSR_MSB         0x13
#define IOE16_REG_GPDR_LSB         0x14
#define IOE16_REG_GPDR_MSB         0x15
#define IOE16_REG_GPPIR_LSB        0x16
#define IOE16_REG_GPPIR_MSB        0x17


/*------------------------------------------------------------------------------
    Functions parameters defines
------------------------------------------------------------------------------*/

/**
  * @brief JOYSTICK Pins definition
  */
#define JOY_IO16_SEL               IO16_Pin_14
#define JOY_IO16_DOWN              IO16_Pin_13
#define JOY_IO16_LEFT              IO16_Pin_12
#define JOY_IO16_RIGHT             IO16_Pin_11
#define JOY_IO16_UP                IO16_Pin_10
#define JOY_IO16_NONE              JOY_IO16_PINS
#define JOY_IO16_PINS              (IO16_Pin_10 | IO16_Pin_11 | IO16_Pin_12 | IO16_Pin_13 | IO16_Pin_14)

/**
  * @brief  IO Pins
  */
#define IO16_Pin_0                 0x0001
#define IO16_Pin_1                 0x0002
#define IO16_Pin_2                 0x0004
#define IO16_Pin_3                 0x0008
#define IO16_Pin_4                 0x0010
#define IO16_Pin_5                 0x0020
#define IO16_Pin_6                 0x0040
#define IO16_Pin_7                 0x0080

#define IO16_Pin_8                 0x0100
#define IO16_Pin_9                 0x0200
#define IO16_Pin_10                0x0400
#define IO16_Pin_11                0x0800
#define IO16_Pin_12                0x1000
#define IO16_Pin_13                0x2000
#define IO16_Pin_14                0x4000
#define IO16_Pin_15                0x8000

#define IO16_Pin_ALL_LSB           0x00FF
#define IO16_Pin_ALL_MSB           0xFF00

/**
  * @brief  IO Pin directions
  */
#define Direction_IN               0x00
#define Direction_OUT              0x01

/**
  * @brief  Interrupt Line output parameters
  */
#define IOE16_Polarity_Low         0x00
#define IOE16_Polarity_High        0x01


/**
  * @brief IO Interrupts
  */
#define IO16_IT_0                  0x0001
#define IO16_IT_1                  0x0002
#define IO16_IT_2                  0x0004
#define IO16_IT_3                  0x0008
#define IO16_IT_4                  0x0010
#define IO16_IT_5                  0x0020
#define IO16_IT_6                  0x0040
#define IO16_IT_7                  0x0080

#define IO16_IT_8                  0x0100
#define IO16_IT_9                  0x0200
#define IO16_IT_10                 0x0400
#define IO16_IT_11                 0x0800
#define IO16_IT_12                 0x1000
#define IO16_IT_13                 0x2000
#define IO16_IT_14                 0x4000
#define IO16_IT_15                 0x8000

#define ALL_IT_LSB                 0x00FF
#define ALL_IT_MSB                 0xFF00
#define IOE16_JOY_IT               (IO16_IT_10 | IO16_IT_11 | IO16_IT_12 | IO16_IT_13 | IO16_IT_14)
#define IOE16_TS_IT                IO16_IT_4

/**
  * @brief  Edge detection value
  */
#define EDGE_FALLING               0x01
#define EDGE_RISING                0x02

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief I2C_Device_t
 * I2C Devices available on the I2CBus1 (I2C1) bus.
 */
typedef enum I2CBus1_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C */
   SN_ROM,
   EUI_ROM,
   IO_EXP,
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
   (DEV) == EUI_ROM ||                                                        \
   (DEV) == IO_EXP                                                            \
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
