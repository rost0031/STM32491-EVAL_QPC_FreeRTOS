/**
 * @file    nor.h
 * @brief   Basic driver for M29WV128G NOR Flash memory on the FMC bus on the
 * STM324x9I-EVAL board.
 *
 * @date    09/23/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupNOR
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NOR_H_
#define NOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp.h"
#include "CBErrors.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
  * @brief  FMC NOR ID typedef
  */
typedef struct
{
  uint16_t Manufacturer_Code;
  uint16_t Device_Code1;
  uint16_t Device_Code2;
  uint16_t Device_Code3;
} NOR_IDTypeDef;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize NOR Flash
 *
 * This function initializes the FMC for NOR flash.
 *
 * @param [in]  None
 *    @arg
 * @return: None
 */
void NOR_Init( void );

/**
 * @brief   Initialize NOR Flash GPIO
 *
 * This function initializes GPIO, clocks, and all the associated hardware for
 * the NOR flash.
 *
 * @param [in]  None
 *    @arg
 * @return: None
 */
void NOR_GPIOInit( void );

/**
 * @brief  Reads NOR memory's Manufacturer and Device Code.
 * @param [out] pNOR_ID: pointer to a NOR_IDTypeDef structure which will hold
 *         the Manufacturer and Device Code.
 * @return  None
 */
void NOR_ReadID( NOR_IDTypeDef* pNOR_ID );

/**
 * @brief  Performs a block erase operation giving its address.
 *         This function returns the NOR memory status after block erase.
 * @param  [in] uwBlockAddress: Address of the block to erase.
 * @return CBErrorCode: The returned value can be:
 *   @arg  ERR_NONE
 *   @arg  ERR_NOR_ERROR
 *   @arg  ERR_NOR_TIMEOUT.
 */
CBErrorCode NOR_EraseBlock( uint32_t uwBlockAddress );

/**
 * @brief  Erases the entire chip.
 * @param  None
 * @retval CBErrorCode: The returned value can be:
 *   @arg  ERR_NONE
 *   @arg  ERR_NOR_ERROR
 *   @arg  ERR_NOR_TIMEOUT
 */
CBErrorCode NOR_EraseChip( void );

/**
 * @brief  Writes a half-word to the NOR memory.
 *         This function returns the NOR memory status after writing half word.
 * @param  [in] uwWriteAddress: uint32_t NOR memory internal address to write to.
 *         [in] uhData: uint16_t word of data to write.
 * @retval CBErrorCode: The returned value can be:
 *   @arg  ERR_NONE
 *   @arg  ERR_NOR_ERROR
 *   @arg  ERR_NOR_TIMEOUT
 */
CBErrorCode NOR_WriteHalfWord( uint32_t uwWriteAddress, uint16_t uhData );

/**
 * @brief  Writes a buffer to the NOR memory.
 *         This function returns the NOR memory status after writing the buffer
 *         to NOR Flash.
 * @param  [in] *pBuffer: uint16_t pointer to buffer.
 *         [in] uwWriteAddress: uint32_t NOR memory internal address to write to.
 *         [in] uwBufferSize: uint32_t number of Half words to write.
 * @retval CBErrorCode: The returned value can be:
 *   @arg  ERR_NONE
 *   @arg  ERR_NOR_ERROR
 *   @arg  ERR_NOR_TIMEOUT
 *   @arg  ERR_NOR_BUSY
 */
CBErrorCode NOR_WriteBuffer(
      uint16_t* pBuffer,
      uint32_t uwWriteAddress,
      uint32_t uwBufferSize
);

/**
 * @brief  Reads a half-word from the NOR memory.
 * @param  [in] uwReadAddress: uint32_t NOR memory internal address to read from.
 * @return: uint16_t Half-word read from the NOR memory.
 */
uint16_t NOR_ReadHalfWord( uint32_t uwReadAddress );

/**
 * @brief  Reads a block of data from the FMC NOR memory.
 * @param  [out] *pBuffer: uint16_t pointer to the buffer that receives the data
 *         read from the NOR memory.
 * @param  [in] uwReadAddress: uint32_t NOR memory internal address to read from.
 * @param  [in] uwBufferSize: uint32_t number of half-words to read.
 * @retval None
 */
void NOR_ReadBuffer(
      uint16_t* pBuffer,
      uint32_t uwReadAddress,
      uint32_t uwBufferSize
);

/**
 * @brief  Returns the NOR memory to Read mode.
 * @param  None
 * @retval None
 */
void NOR_ReturnToReadMode( void );

/**
 * @brief  Resets the NOR memory.
 * Resets the errors in the NOR memory status register, and sets NOR to Read mode.
 *
 * @param  None
 * @retval None
 */
void NOR_Reset( void );

/**
 * @brief  Returns the NOR operation status.
 * @param  [in] Timeout: uint32_t NOR programming Timeout
 * @retval CBErrorCode: The returned value can be:
 *   @arg  ERR_NONE
 *   @arg  ERR_NOR_ERROR
 *   @arg  ERR_NOR_TIMEOUT
 *   @arg  ERR_NOR_BUSY
 */
CBErrorCode NOR_GetStatus( uint32_t Timeout );

/**
 * @brief  Runs a destructive test of the the NOR flash memory.
 * @param  None
 * @retval None
 */
void NOR_TestDestructive( void );

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
void NOR_CallbackExample( void );

/**
 * @}
 * end addtogroup groupNOR
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* NOR_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
