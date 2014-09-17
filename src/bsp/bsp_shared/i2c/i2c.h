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
#include "stm32f4xx.h"                                 /* For STM32F4 support */
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
 * \enum I2C_Operation_t
 * I2C Operations available on the system.
 */
typedef enum I2C_Operations {
   I2C_OP_NONE  = 0,                                /**< No current operation */
   I2C_OP_READ,                               /**< Reading from an I2C device */
   I2C_OP_WRITE,                                /**< Writing to an I2C device */
   /* Insert more I2C operations here... */
} I2C_Operation_t;

/**
 * \enum I2C_Device_t
 * I2C Devices available on the system.
 */
typedef enum I2C_States {
   I2C_IDLE_ST  = 0,                         /**< I2C is idle */
   I2C_GEN_START_ST,
   I2C_MASTER_TX_MODE_SELECTED_ST,
   I2C_SENT_MSB_ADDR_ST,
   I2C_SENT_LSB_ADDR_ST,
   I2C_GEN_2ND_START_ST,
   /* Insert more I2C states here... */
   I2C_MAX_ST     /**< Maximum number of available I2C states */
} I2C_State_t;

/**
 * \enum I2C_Device_t
 * I2C Devices available on the system.
 */
typedef enum I2C_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C */
   /* Insert more I2C device enumerations here... */
   MAX_I2C_DEV     /**< Maximum number of available I2C devices on the system */
} I2C_Device_t;

/**
 * \enum I2C_Bus_t
 * I2C Busses available on the system.
 */
typedef enum I2C_Busses {
   I2CBus1  = 0,                                        /**< I2C Bus 1 (I2C1) */
   /* Insert more I2C device enumerations here... */
   MAX_I2C_BUS      /**< Maximum number of available I2C Busses on the system */
} I2C_Bus_t;

/**
 * \struct I2C_BusDevice_t
 * Settings for the various I2C devices that are attached to any of the I2C busses.
 */
typedef struct I2C_BusDevices
{
   /* "External" device settings */
   const I2C_Device_t      i2c_dev;          /**< System I2C device specifier */
   const I2C_TypeDef *     i2c_bus;        /**< I2C bus device is attached to */
   const uint8_t           i2c_dev_addr;  /**< I2C Device address of the device */

   /* Internal device settings */
   const uint8_t           i2c_mem_addr_size; /**< How many bytes are used to specify internal mem addr of the device */
   uint16_t                i2c_mem_addr;  /**< I2C last accessed mem address  */

} I2C_BusDevice_t;

/**
 * \struct I2C_BusSettings_t
 * Most of the settings that are needed to set up all the hardware for each
 * I2C Bus.
 */
typedef struct I2C_BusSettings
{
   I2C_Bus_t               i2c_sys_bus;     /**< System specifier for I2C Bus */

   /* I2C Bus settings */
   I2C_TypeDef *           i2c_bus;                              /**< I2C bus */
   const uint32_t          i2c_bus_speed;            /**< I2C bus speed in Hz */
   const uint32_t          i2c_clk;                            /**< I2C clock */

   /* I2C interrupt settings */
   IRQn_Type               i2c_ev_irq_num;          /**< I2C Event IRQ number */
   ISR_Priority            i2c_ev_irq_prio;       /**< I2C Event IRQ priority */
   IRQn_Type               i2c_er_irq_num;          /**< I2C Error IRQ number */
   ISR_Priority            i2c_er_irq_prio;       /**< I2C Error IRQ priority */

   /* I2C GPIO settings for SCL */
   GPIO_TypeDef *          scl_port;                        /**< I2C SCL port */
   const uint16_t          scl_pin;                          /**< I2C SCL pin */
   const uint16_t          scl_af_pin_source;/**< I2C SCL alt function source */
   const uint16_t          scl_af;                  /**< I2C SCL alt function */
   const uint32_t          scl_clk;                   /**< I2C SCL GPIO clock */

   /* I2C GPIO settings for SDA */
   GPIO_TypeDef *          sda_port;                        /**< I2C SDA port */
   const uint16_t          sda_pin;                          /**< I2C SDA pin */
   const uint16_t          sda_af_pin_source;/**< I2C SDA alt function source */
   const uint16_t          sda_af;                  /**< I2C SDA alt function */
   const uint32_t          sda_clk;                   /**< I2C SDA GPIO clock */

   /* Common I2C DMA settings */
   DMA_TypeDef *           i2c_dma;                       /**< I2C DMA device */
   const uint32_t          i2c_dma_channel;              /**< I2C DMA channel */
   const uint32_t          i2c_dma_dr_addr;           /**< I2C DMA DR address */
   const uint32_t          i2c_dma_clk;               /**< I2C DMA Clk source */

   /* TX I2C DMA settings */
   DMA_Stream_TypeDef *    i2c_dma_tx_stream;      /**< I2C DMA stream for TX */
   IRQn_Type               i2c_dma_tx_irq_num;     /**< I2C DMA TX IRQ number */
   ISR_Priority            i2c_dma_tx_irq_prio;  /**< I2C DMA TX IRQ priority */

   /* RX I2C DMA settings */
   DMA_Stream_TypeDef *    i2c_dma_rx_stream;      /**< I2C DMA stream for RX */
   IRQn_Type               i2c_dma_rx_irq_num;     /**< I2C DMA RX IRQ number */
   ISR_Priority            i2c_dma_rx_irq_prio;  /**< I2C DMA RX IRQ priority */

   /* Buffer management */
   uint8_t                 *pRxBuffer;                   /**< I2C data buffer.*/
   uint16_t                nRxIndex;         /**< I2C data buffer used length.*/
   uint8_t                 *pTxBuffer;                   /**< I2C data buffer.*/
   uint16_t                nTxIndex;         /**< I2C data buffer used length.*/

   /* Device management */
   I2C_Device_t            i2c_cur_dev;     /**< Current I2C device specifier.*/
   I2C_State_t             i2c_cur_st;             /**< Current I2C bus state.*/
   uint8_t                 i2c_cur_dev_addr;      /**< I2C device bus address.*/
   uint8_t                 bTransDirection;    /**< Transmitting or Receiving */
   uint16_t                nBytesExpected; /**< How many bytes expected to TX or RX. */
   uint16_t                nBytesCurrent; /**< How many bytes have already been TXed or RXed. */

} I2C_BusSettings_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t i2c1RxBuffer[];          /**< Exported to I2CMgr I2C RX buffer */
extern uint8_t i2c1TxBuffer[];          /**< Exported to I2CMgr I2C TX buffer */
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
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus
 *    @arg  I2CBus1
 * @param [in]  I2C_Direction: an STM32 define that specifies the direction for
 * an I2C device
 *    @arg  I2C_Direction_Receiver
 *    @arg  I2C_Direction_Transmitter
 * @return: None
 */
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction);

/**
 * @brief   Get the number of bytes used to address a device.
 *
 * Some devices use 2 byte addresses, others use 1.  This function checks the
 * array of devices and returns the size of the memory addresses.
 *
 * @param [in]  iDev: I2C_Device_t identifier for I2C device
 *    @arg  EEPROM
 * @return uint8_t: number of bytes used to address the internal memory of the
 * I2C device.  1 or 2 bytes.
 */
uint8_t I2C_getDevAddrSize( I2C_Device_t iDev );

/**
 * @brief   Set the direction on the I2C bus.
 *
 * This function sets the direction in the I2C internal structure so the ISR will
 * know what to do when an interrupt fires.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus
 *    @arg  I2CBus1
 * @param [in]  wReadAddr: uint16_t for internal I2C device address where to
 * read from.
 * @param [in]  wReadLen: uint16_t for how many bytes to read from the I2C
 * device.
 * @return: None
 */
void I2C_DMARead( I2C_Bus_t iBus, uint16_t wReadAddr, uint16_t wReadLen );
/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
