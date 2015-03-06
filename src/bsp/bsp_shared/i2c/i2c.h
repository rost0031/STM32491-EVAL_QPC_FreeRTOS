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
#include "i2c_defs.h"
/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C bus is defined in the system
 * @param [in] BUS:  I2C_Bus_t type I2C bus specifier.
 * @retval
 *    1: Bus exists and is valid
 *    0: Bus doesn't exist or isn't defined
 */
#define IS_I2C_BUS(BUS) ((BUS) == I2CBus1)

/**
 * @brief   Macro as a wrapper for the I2C Timeout callback.
 * This macro is a wrapper around the I2C_TIMEOUT_UserCallbackRaw() function.
 * This wrapper grabs the function name and line where the callback was called
 * from an passes it to the callback.
 * @param [in] bus: I2C_Bus_t type that specifies what i2c bus the error occurred on.
 * @param [in] error: CBErrorCode that specifies the error that occurred.
 * @return None
 */
#define I2C_TIMEOUT_UserCallback( bus, error ) \
      I2C_TIMEOUT_UserCallbackRaw(bus, error, __func__, __LINE__);

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
 * an I2C bus
 *    @arg  I2C_Direction_Receiver
 *    @arg  I2C_Direction_Transmitter
 * @return: None
 */
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction);

/**
 * @brief   Get the direction on the I2C bus.
 *
 * This function gets the current direction set on the specified I2C bus.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus
 *    @arg  I2CBus1
 * @return I2C_Direction: the direction currently set for an I2C bus.
 *    @arg  I2C_Direction_Receiver
 *    @arg  I2C_Direction_Transmitter
 */
uint8_t I2C_getDirection( I2C_Bus_t iBus );

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
 * @brief  Reads a block of data from a memory device on any I2C bus.
 *
 * @note:  This is a slow function that should not be called by any threads or
 * objects. It's for use in case of crashes and before the entire system has
 * come up.
 *
 * @param [in] iBus: I2C_Bus_t type specifying the I2C bus where device lives.
 *    @arg I2CBus1
 * @param [in] i2cDevAddr: address of the device on the I2C bus.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 * @param [in] i2cMemAddrSize: size of the memory address i2cMemAddr.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @param [in|out] *pBuffer: uint8_t pointer to buffer where to store read data.
 * @param [in] bytesToRead : uint8_t variable specifying how many bytes to read.
 *
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readBufferBLK(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToRead
);

/**
 * @brief  Writes a block of data to a memory device on any I2C bus.
 *
 * @note:  This is a slow function that should not be called by any threads or
 * objects. It's for use in case of crashes and before the entire system has
 * come up.
 *
 * @note: This function also handles writing over page boundaries and assumes
 * that like most memory devices, a page boundary exists that if written over,
 * loops around and continues writing at the beginning of the page.
 *
 * @param [in] iBus: I2C_Bus_t type specifying the I2C bus where device lives.
 *    @arg I2CBus1
 * @param [in] i2cDevAddr: address of the device on the I2C bus.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 * @param [in] i2cMemAddrSize: size of the memory address i2cMemAddr.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @param [in] *pBuffer: uint8_t pointer to buffer to the data to be written.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 *
 * @return CBErrorCode: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeBufferBLK( /* TODO: does this belong in i2c_dev? */
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToWrite
);

/******************************************************************************/
/***                      Callback functions for I2C                        ***/
/******************************************************************************/

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
