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
/* Exported types ------------------------------------------------------------*/
/**
 * \enum I2C_Device_t
 * I2C Devices available on the system.
 */
typedef enum I2C_Device {
   EEPROM  = 0,                         /**< EEPROM attached to I2C. */
   /* Insert more I2C device enumerations here... */
   MAX_I2C     /**< Maximum number of available I2C devices on the system */
} I2C_Device_t;

/**
 * \struct USART_Settings_t
 * Most of the settings that are needed to set up all the hardware for each
 * serial port.
 */
typedef struct I2C_Settings
{
    I2C_Device_t                 i2c_dev;              /**< System I2C device.*/

    /* USART settings */
    I2C_TypeDef *                i2c_bus;                        /**< I2C bus.*/
    const uint32_t               i2c_clk;                      /**< I2C clock.*/
    const uint8_t                i2c_dev_addr;    /**< I2C device bus address.*/
    IRQn_Type                    i2c_ev_irq_num;    /**< I2C Event IRQ number.*/
    ISR_Priority                 i2c_ev_irq_prio; /**< I2C Event IRQ priority.*/
    IRQn_Type                    i2c_er_irq_num;    /**< I2C Error IRQ number.*/
    ISR_Priority                 i2c_er_irq_prio; /**< I2C Error IRQ priority.*/

    GPIO_TypeDef *               scl_port;                  /**< I2C SCL port.*/
    const uint16_t               scl_pin;                    /**< I2C SCL pin.*/
    const uint16_t               scl_af_pin_source;/**< I2C SCL alt function source.*/
    const uint16_t               scl_af;            /**< I2C SCL alt function.*/

    GPIO_TypeDef *               sda_port;                  /**< I2C SDA port.*/
    const uint16_t               sda_pin;                    /**< I2C SDA pin.*/
    const uint16_t               sda_af_pin_source;/**< I2C SDA alt function source.*/
    const uint16_t               sda_af;            /**< I2C SDA alt function.*/

    /* Buffer management */
    uint8_t                      *buffer;                /**< I2C data buffer.*/
    uint16_t                     index;      /**< I2C data buffer used length.*/
} I2C_Settings_t;


/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize specified I2C device
 *
 * This function initializes GPIO, clocks (except for the GPIO clocks), and all
 * the associated hardware for a specified I2C device.
 *
 * @param [in]  device: Which I2C device to initialize
 *    @arg
 * @return: None
 */
void I2CD_Init( I2C_Device_t device );

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
