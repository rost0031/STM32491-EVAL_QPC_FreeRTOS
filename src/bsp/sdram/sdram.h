/**
 * @file    sdram.h
 * @brief   Basic driver for MT48LC2M3B2B5-7E SDRAM memory on the FMC bus on the
 * STM324x9I-EVAL board.
 *
 * @date    09/24/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSDRAM
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SDRAM_H_
#define SDRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp.h"
#include "CBErrors.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Configures the FMC and GPIOs to interface with the SDRAM memory.
 *          This function must be called before any read/write operation
 *          on the SDRAM.
 *
 * @param   None
 * @return: None
 */
void SDRAM_Init( void );

/**
 * @brief   Initialize SDRAM GPIO
 *
 * This function initializes GPIO and clocks SDRAM flash on the FMC.
 *
 * @param [in]  None
 * @return: None
 */
void SDRAM_GPIOInit( void );

/**
 * @brief  Executes the SDRAM memory initialization sequence.
 * @param  None.
 * @retval None.
 */
void SDRAM_InitSequence( void );

/**
 * @brief  Writes a Entire-word buffer to the SDRAM memory.
 * @param  [in] pBuffer: uint32_t pointer to buffer.
 * @param  [in] uwWriteAddress: uint32_t SDRAM memory internal address from
 *         which the data will be written.
 * @param  [in] uwBufferSize: uint32_t number of words to write.
 * @retval None.
 */
void SDRAM_WriteBuffer(
      uint32_t *pBuffer,
      uint32_t uwWriteAddress,
      uint32_t uwBufferSize
);

/**
 * @brief  Reads data buffer from the SDRAM memory.
 * @param  [out] pBuffer: uint32_t pointer to buffer where to store read data.
 * @param  [in] uwReadAddress: uint32_t SDRAM memory internal address from which
 *         the data will be read.
 * @param  [in] uwBufferSize: uint32_t number of words to write.
 * @retval None.
 */
void SDRAM_ReadBuffer(
      uint32_t* pBuffer,
      uint32_t uwReadAddress,
      uint32_t uwBufferSize
);

/**
 * @brief  Runs a destructive test of the the SDRAM.
 * @param  None
 * @retval None
 */
void SDRAM_TestDestructive( void );

/**
 * @brief   NOR Event callback function
 *
 * This function should only be called from the ISR that handles the NOR ISR
 * that handles the FMC regular event interrupts.
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @param   None
 * @return: None
 */
void SDRAM_CallbackExample( void );

/**
 * @}
 * end addtogroup groupSDRAM
 */

#ifdef __cplusplus
}
#endif

#endif                                                            /* SDRAM_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
