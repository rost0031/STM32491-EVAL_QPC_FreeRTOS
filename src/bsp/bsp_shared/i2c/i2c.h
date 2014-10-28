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
#include "i2c_dev.h"

/* Exported defines ----------------------------------------------------------*/

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
 * @brief I2C_MemAccess_t
 * I2C device internal memory access types
 */
typedef enum I2C_MemAccess {
   I2C_MEM_BYTE  = 0,                   /**< Access memory one byte at a time */
   I2C_MEM_DMA,                                    /**< Access memory via DMA */
} I2C_MemAccess_t;

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
//   I2C_Device_t            i2c_cur_dev;     /**< Current I2C device specifier.*/
   I2C_State_t             i2c_cur_st;             /**< Current I2C bus state.*/
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
 * This function initializes GPIO, clocks, and all the associated hardware for
 * a specified I2C bus interface.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to initialize
 *    @arg
 * @return: None
 */
void I2C_BusInit( I2C_Bus_t iBus );

/**
 * @brief   DeInitialize specified I2C bus
 *
 * This function initializes GPIO used by I2C for input, disables clocks (except
 * for the SYSCFG clock), and all the associated hardware for a specified I2C
 * bus interface.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to de-initialize
 *    @arg
 * @return: None
 */
void I2C_BusDeInit( I2C_Bus_t iBus );

/**
 * @brief   Initialize specified I2C bus for recovery.
 *
 * This function sets up I2C used GPIO for manual toggling.  This is SHOULDN'T
 * be necessary but if there's ever a case of a STOP bit not making it to the
 * I2C slave device, this is the only way to reset the bus.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to initialize for recovery.
 *    @arg
 * @return: None
 */
void I2C_BusInitForRecovery( I2C_Bus_t iBus );

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
//uint8_t I2C_getDevAddrSize( I2C_Device_t iDev );

/**
 * @brief   Start the DMA read operation on the specified I2C bus.
 *
 * This function kicks off the DMA read operation on the passed in I2C bus.
 * Once the operation has completed, the DMA ISR should call the handler
 * function, which in turn will post the event with the data to whoever is
 * subscribed to it.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus
 *    @arg  I2CBus1
 * @param [in]  wReadLen: uint16_t for how many bytes to read from the I2C
 * device.
 * @return: None
 */
void I2C_StartDMARead( I2C_Bus_t iBus, uint16_t wReadLen );

/**
 * @brief   Start the DMA write operation on the specified I2C bus.
 *
 * This function kicks off the DMA write operation on the passed in I2C bus.
 * Once the operation has completed, the DMA ISR should call the handler
 * function, which in turn will post the event with the data to whoever is
 * subscribed to it.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus
 *    @arg  I2CBus1
 * @param [in]  wReadLen: uint16_t for how many bytes to write from the I2C
 * device.
 *
 * @note: The data must already be copied into the s_I2C_Bus[iBus].pTxBuffer
 * buffer.  This should happen the moment that the event that kicks off the
 * write (usually I2C_WRITE_START_SIG) gets read from the I2CMgr AO queue.
 *
 * @return: None
 */
void I2C_StartDMAWrite( I2C_Bus_t iBus, uint16_t wWriteLen );

/**
 * @brief   I2C DMA read callback function
 *
 * This function should only be called from the ISR that handles the DMA ISR
 * that handles the DMA stream that handles this functionality.
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @note: There is a I2C1_DMAReadCallback_cleanup tag defined within this
 * handler.  Use it for a common exit from this handler.  DON'T call return.
 * Instead, use
 * goto I2C1_DMAReadCallback_cleanup;
 * This allows for always correctly clearing the interrupt status bits. Else
 * things will lock up.
 *
 * @param   None
 * @return: None
 */
void I2C1_DMAReadCallback( void );

/**
 * @brief   I2C DMA write callback function
 *
 * This function should only be called from the ISR that handles the DMA ISR
 * that handles the DMA stream that handles this functionality.
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @param   None
 * @return: None
 */
void I2C1_DMAWriteCallback( void );

/**
 * @brief   I2C1 Error Event callback function
 *
 * This function should only be called from the ISR that handles the I2C1 ISR
 * that handles the I2C bus error interrupts.
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @param   None
 * @return: None
 */
void I2C1_ErrorEventCallback( void );

/**
 * @brief   I2C1 Event callback function
 *
 * This function should only be called from the ISR that handles the I2C1 ISR
 * that handles the I2C1 bus regular event interrupts.
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @param   None
 * @return: None
 */
void I2C1_EventCallback( void );

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
