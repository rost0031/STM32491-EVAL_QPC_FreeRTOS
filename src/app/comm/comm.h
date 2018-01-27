/**
 * @file 	comm.h
 * @brief   Parser for various messages that come over any communication busses.
 *
 * @date   	09/29/2014
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupComm
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMM_H_
#define COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
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
CBErrorCode COMM_parseMsg( uint8_t *pBuffer, uint16_t bufferLen, MsgSrc msgSource );

/**
 * @}
 * end addtogroup groupComm
 */

#ifdef __cplusplus
}
#endif

#endif                                                             /* COMM_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
