/**
 * @file   stm32f4xx_eth_conf.h
 * @brief  Configuration file for the STM32F4xx ETH MAC driver.
 *
 * @date   08/25/2015
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupDP83848
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_ETH_CONF_H
#define __STM32F2xx_ETH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "stm32f4xx_eth.h"

/* Exported defines ----------------------------------------------------------*/

/**
 * @brief Flag that configures whether time stamping and/or IPv4 checksum is done
 * in hardware.
 *
 * Uncomment the line below when using time stamping and/or IPv4 checksum offload
 */
#define USE_ENHANCED_DMA_DESCRIPTORS

/**
 * @brief Defines function that overrides the default ethernet delay with a
 * more precise one.
 *
 * Uncomment the line below if you want to use user defined Delay function (for
 * precise timing), otherwise default _eth_delay_ function defined within the
 * Ethernet driver is used (less precise timing)
 */
//#define USE_Delay

#ifdef USE_Delay
#include "main.h"                /* Header file where the Delay function prototype is exported */
#define _eth_delay_    Delay     /* User can provide more timing precise _eth_delay_ function */
#else
#define _eth_delay_    ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif


/**
 * @brief   Flag that allows custom configuration of ethernet buffers.
 *
 * Uncomment the line below to allow custom configuration of the Ethernet
 * driver buffers
 */
#define CUSTOM_DRIVER_BUFFERS_CONFIG

/* Redefinition of the Ethernet driver buffers size and count */
#ifdef  CUSTOM_DRIVER_BUFFERS_CONFIG
/**
 * @brief Ethernet RX buffer size
 */
#define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE

/**
 * @brief Ethernet TX buffer size
 */
#define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE

/**
 * @brief How many ethernet RX buffers to allocate
 */
#define ETH_RXBUFNB        2

/**
 * @brief How many ethernet TX buffers to allocate
 */
#define ETH_TXBUFNB        2
#endif

/* PHY configuration section **************************************************/
/* PHY Reset delay */ 
#define PHY_RESET_DELAY    ((uint32_t)0x000FFFFF) 
/* PHY Configuration delay */ 
#define PHY_CONFIG_DELAY   ((uint32_t)0x00FFFFFF)

/* The PHY status register value change from a PHY to another, so the user have 
   to update this value depending on the used external PHY */
#define PHY_SR    ((uint16_t)16) /* Value for DP83848 PHY */

/* The Speed and Duplex mask values change from a PHY to another, so the user
   have to update this value depending on the used external PHY */
#define PHY_SPEED_STATUS            ((uint16_t)0x0002) /* Value for DP83848 PHY */
#define PHY_DUPLEX_STATUS           ((uint16_t)0x0004) /* Value for DP83848 PHY */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */  

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupDP83848
 */

#endif                                              /* __STM32F4xx_ETH_CONF_H */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
