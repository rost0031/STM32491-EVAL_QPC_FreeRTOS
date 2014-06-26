/**
 * @file 	stm32f2x7_eth_bsp.h
 * @brief   This file contains the Board Support Package functions for any
 * STM32F2xx board and Micrel KSZ8863 Ethernet switch.
 *
 * @date   	09/27/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 *
 * @addtogroup groupKSZ8863
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2x7_ETH_BSP_H
#define __STM32F2x7_ETH_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Configures the Ethernet MAC-MAC DMA Interface.
 * @param  None
 * @retval  ETH_ERROR: if error occurred
 *          ETH_SUCCESS: if no error occurred
 */
uint32_t ETH_MACDMA_Config( void );

/**
 * @brief  Configures the GPIO used by ethernet.
 * @param  None
 * @retval None
 */
void ETH_GPIO_Config( void );

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupKSZ8863
 */

#endif                                               /* __STM32F2x7_ETH_BSP_H */
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
