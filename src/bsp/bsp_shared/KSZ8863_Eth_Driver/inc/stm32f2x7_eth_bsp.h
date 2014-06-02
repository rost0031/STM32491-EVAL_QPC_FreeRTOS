// $Id$
/**
 * @file 	stm32f2x7_eth_bsp.h
 * @brief   This file contains the Board Support Package functions for the
 * 			Redwood_H1_Board and Micrel KSZ8863 Ethernet switch
 *
 * @date   	09/27/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2x7_ETH_BSP_H
#define __STM32F2x7_ETH_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint32_t ETH_MACDMA_Config(void);
void ETH_GPIO_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2x7_ETH_BSP_H */
 /*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
