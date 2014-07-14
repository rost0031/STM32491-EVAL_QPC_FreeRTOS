/**
 * @file 	i2c.h
 * @brief   Basic driver for I2C bus.
 *
 * @date   	06/30/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm32f2xx_i2c.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_gpio.h"
#include "bsp.h"

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C device is defined in the system
 * @param [in] DEV:  I2C_Device_t type I2C device specifier.
 * @retval
 *    1: Device exists and is valid
 *    0: Device doesn't exist or isn't defined
 */
#define IS_I2C_DEVICE(DEV) ((DEV) == EEPROM)

/**
 * @brief   Macro to determine if an I2C bus is defined in the system
 * @param [in] BUS:  I2C_Bus_t type I2C bus specifier.
 * @retval
 *    1: Bus exists and is valid
 *    0: Bus doesn't exist or isn't defined
 */
#define IS_I2C_BUS(BUS) ((BUS) == I2CBus1)

/* Exported types ------------------------------------------------------------*/

/**
 * \enum I2C_Device_t
 * I2C Devices available on the system.
 */
typedef enum I2C_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C. */
   /* Insert more I2C device enumerations here... */
   MAX_I2C_DEV     /**< Maximum number of available I2C devices on the system */
} I2C_Device_t;

/**
 * \enum I2C_Bus_t
 * I2C Busses available on the system.
 */
typedef enum I2C_Busses {
   I2CBus1  = 0,                                       /**< I2C Bus 1 (I2C1). */
   /* Insert more I2C device enumerations here... */
   MAX_I2C_BUS      /**< Maximum number of available I2C Busses on the system */
} I2C_Bus_t;

/**
 * \struct I2C_BusDevice_t
 * Settings for the various I2C devices that are attached to any of the I2C busses.
 */
typedef struct I2C_BusDevices
{
   I2C_Device_t            i2c_dev;          /**< System I2C device specifier.*/
   I2C_TypeDef *           i2c_bus;        /**< I2C bus device is attached to.*/

   uint8_t                 i2c_address;  /**< I2C Slave address of the device */

} I2C_BusDevice_t;

/**
 * \struct I2C_BusSettings_t
 * Most of the settings that are needed to set up all the hardware for each
 * I2C Bus.
 */
typedef struct I2C_BusSettings
{
   I2C_Bus_t               i2c_sys_bus;     /**< System specifier for I2C Bus.*/

   /* I2C Bus settings */
   I2C_TypeDef *           i2c_bus;                              /**< I2C bus.*/
   const uint32_t          i2c_bus_speed;            /**< I2C bus speed in Hz.*/
   const uint32_t          i2c_clk;                            /**< I2C clock.*/

   IRQn_Type               i2c_ev_irq_num;          /**< I2C Event IRQ number.*/
   ISR_Priority            i2c_ev_irq_prio;       /**< I2C Event IRQ priority.*/
   IRQn_Type               i2c_er_irq_num;          /**< I2C Error IRQ number.*/
   ISR_Priority            i2c_er_irq_prio;       /**< I2C Error IRQ priority.*/

   GPIO_TypeDef *          scl_port;                        /**< I2C SCL port.*/
   const uint16_t          scl_pin;                          /**< I2C SCL pin.*/
   const uint16_t          scl_af_pin_source;/**< I2C SCL alt function source.*/
   const uint16_t          scl_af;                  /**< I2C SCL alt function.*/

   GPIO_TypeDef *          sda_port;                        /**< I2C SDA port.*/
   const uint16_t          sda_pin;                          /**< I2C SDA pin.*/
   const uint16_t          sda_af_pin_source;/**< I2C SDA alt function source.*/
   const uint16_t          sda_af;                  /**< I2C SDA alt function.*/

   /* Buffer management */
   uint8_t                 *pRxBuffer;                   /**< I2C data buffer.*/
   uint16_t                nRxindex;         /**< I2C data buffer used length.*/

   /* Device management */
   I2C_Device_t            i2c_cur_dev;     /**< Current I2C device specifier.*/
   uint8_t                 i2c_cur_dev_addr;      /**< I2C device bus address.*/
   uint8_t                 bTransDirection;    /**< Transmitting or Receiving */
   uint16_t                nBytesExpected; /**< How many bytes expected to TX or RX. */
   uint16_t                nBytesCurrent; /**< How many bytes have already been TXed or RXed. */

} I2C_BusSettings_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize specified I2C bus
 *
 * This function initializes GPIO, clocks (except for the GPIO clocks), and all
 * the associated hardware for a specified I2C bus interface.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to initialize
 *    @arg
 * @return: None
 */
void I2C_BusInit( I2C_Bus_t iBus );

/**
 * @brief   Set the direction on the I2C bus.
 *
 * This function sets the direction in the I2C internal structure so the ISR will
 * know what to do when an interrupt fires.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C device
 *    @arg  EEPROM
 * @param [in]  I2C_Direction: an STM32 define that specifies the direction for
 * an I2C device
 *    @arg  I2C_Direction_Receiver
 *    @arg  I2C_Direction_Transmitter
 * @return: None
 */
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction);

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
