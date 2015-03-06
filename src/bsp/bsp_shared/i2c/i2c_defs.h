/**
 * @file    i2c_defs.h
 * @brief   Defines and types for I2C busses and devices.
 *
 * These have to be kept separate from the i2c.h and i2c_dev.h so that both can
 * include this without circular dependencies.
 *
 * @date    03/05/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_DEFS_H_
#define I2C_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp_defs.h"
#include "Shared.h"
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Exported defines ----------------------------------------------------------*/
#define I2C_SPEED             400000                /**< Speed of the I2C Bus */
#define MAX_I2C_WRITE_LEN     20  /**< Max size of the I2C buffer for writing */
#define MAX_I2C_READ_LEN      20  /**< Max size of the I2C buffer for reading */

#define EEPROM_PAGE_SIZE   16    /**< Size of the page in bytes on the EEPROM */

/* Exported types ------------------------------------------------------------*/

/**
 * I2C Operations available on the system.
 */
typedef enum I2C_Operations {
   I2C_OP_NONE  = 0,                                /**< No current operation */
   I2C_OP_MEM_READ,                    /**< Reading from an I2C device memory */
   I2C_OP_MEM_WRITE,                     /**< Writing to an I2C device memory */
   I2C_OP_REG_READ,                  /**< Reading from an I2C device register */
   I2C_OP_REG_WRITE,                   /**< Writing to an I2C device register */
   /* Insert more I2C operations here... */
} I2C_Operation_t;
/**
 * I2C device internal memory access types
 */
typedef enum I2C_MemAccess {
   I2C_MEM_BYTE  = 0,                   /**< Access memory one byte at a time */
   I2C_MEM_DMA,                                    /**< Access memory via DMA */
   /* Insert more I2C access types here... */
} I2C_MemAccess_t;

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
   const uint32_t          i2c_dma_tx_flags;            /**< I2C DMA TX Flags */

   /* RX I2C DMA settings */
   DMA_Stream_TypeDef *    i2c_dma_rx_stream;      /**< I2C DMA stream for RX */
   IRQn_Type               i2c_dma_rx_irq_num;     /**< I2C DMA RX IRQ number */
   ISR_Priority            i2c_dma_rx_irq_prio;  /**< I2C DMA RX IRQ priority */
   const uint32_t          i2c_dma_rx_flags;            /**< I2C DMA TX Flags */

   /* Buffer management */
   uint8_t                 *pRxBuffer;                   /**< I2C data buffer.*/
   uint16_t                nRxIndex;         /**< I2C data buffer used length.*/
   uint8_t                 *pTxBuffer;                   /**< I2C data buffer.*/
   uint16_t                nTxIndex;         /**< I2C data buffer used length.*/

   /* Device management */
   uint8_t                 bTransDirection;    /**< Transmitting or Receiving */
   uint16_t                nBytesExpected; /**< How many bytes expected to TX or RX. */
   uint16_t                nBytesCurrent; /**< How many bytes have already been TXed or RXed. */

} I2C_BusSettings_t;

/**
 * @brief I2C_Device_t
 * I2C Devices available on all the I2C busses.
 */
typedef enum I2C_Devices {
   EEPROM  = 0,                         /**< EEPROM attached to I2C */
   SN_ROM,
   EUI_ROM,
   /* Insert more I2C device enumerations here... */
   MAX_I2C_DEV     /**< Maximum number of available I2C devices on I2CBus1 */
} I2C_Dev_t;

/**
 * @brief I2C_DeviceSettings_t
 * Settings for the various I2C devices that are attached to the I2C busses.
 */
typedef struct I2C_DeviceSettings
{
   /* "External" device settings */
   const I2C_Dev_t         i2c_dev;          /**< System I2C device specifier */
   const I2C_Bus_t         i2c_bus;          /**< Which I2C bus the device is on */
   const uint16_t          i2c_dev_addr_size;  /**< Size of the I2C device address */
   const uint16_t          i2c_dev_addr;  /**< I2C Device address of the device */

   /* Internal device settings */
   const uint8_t           i2c_mem_addr_size; /**< Size of mem addr on device */
   uint16_t                i2c_mem_addr;   /**< I2C last accessed mem address */
   const uint16_t          i2c_mem_min_addr;/**< The first address that can be accessed */
   const uint16_t          i2c_mem_max_addr;/**< The last address that can be accessed */
   const uint8_t           i2c_mem_page_size; /**< Size of a page of memory */

} I2C_DevSettings_t;

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
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                         /* I2C_DEFS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
