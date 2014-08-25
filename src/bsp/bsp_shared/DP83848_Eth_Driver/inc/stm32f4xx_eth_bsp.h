/**
 * @file 	stm32f4xx_eth_bsp.h
 * @brief   This file contains the Board Support Package functions for any
 * STM32F4xx board and Texas Instruments DP83848 Ethernet PHY.
 *
 * @date   	08/25/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupDP83848
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_ETH_BSP_H
#define __STM32F4xx_ETH_BSP_H

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
 * end addtogroup groupDP83848
 */

#endif                                               /* __STM32F4xx_ETH_BSP_H */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
