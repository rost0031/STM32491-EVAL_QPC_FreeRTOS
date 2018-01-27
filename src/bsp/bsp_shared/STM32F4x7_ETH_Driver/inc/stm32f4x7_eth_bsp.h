/**
 * @file    stm32f4x7_eth_bsp.c
 * @brief   Driver for the DP83848 Ethernet PHY.
 *
 * This file contains the definitions for the driver for the DP83848 Ethernet PHY
 * as connected to the STM324x9I-EVAL2 dev board.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupEthernet
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_BSP_H
#define __STM32F4x7_ETH_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MII_MODE
#ifdef  MII_MODE
 /* Uncomment if you want to use the internal oscillator.  Make sure to set the
  * jumper on the eval kit appropriately. */
//    #define PHY_CLOCK_MCO
#endif

#define DP83848_PHY_ADDRESS       0x01 /* Relative to STM324x9I-EVAL Board */

/* Specific defines for EXTI line, used to manage Ethernet link status */
#define ETH_LINK_EXTI_LINE             EXTI_Line14
#define ETH_LINK_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOB
#define ETH_LINK_EXTI_PIN_SOURCE       EXTI_PinSource14
#define ETH_LINK_EXTI_IRQn             EXTI15_10_IRQn 
/* PB14 */
#define ETH_LINK_PIN                   GPIO_Pin_14
#define ETH_LINK_GPIO_PORT             GPIOB
#define ETH_LINK_GPIO_CLK              RCC_AHB1Periph_GPIOB
/* PHY registers */
#define PHY_MICR                  0x11 /* MII Interrupt Control Register */
#define PHY_MICR_INT_EN           ((uint16_t)0x0002) /* PHY Enable interrupts */
#define PHY_MICR_INT_OE           ((uint16_t)0x0001) /* PHY Enable output interrupt events */
#define PHY_MISR                  0x12 /* MII Interrupt Status and Misc. Control Register */
#define PHY_MISR_LINK_INT_EN      ((uint16_t)0x0020) /* Enable Interrupt on change of link status */
#define PHY_LINK_STATUS           ((uint16_t)0x2000) /* PHY link status interrupt mask */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Configure ethernet, PHY, MAC, and EthDMA.
 * @param  None
 * @retval None
 */
void  ETH_BSP_Config( void );

/**
 * @brief  Configure the PHY to generate an interrupt on change of link status.
 * @param PHYAddress: external PHY address
 * @retval None
 */
uint32_t Eth_Link_PHYITConfig( uint16_t PHYAddress );

/**
 * @brief  EXTI configuration for Ethernet link status.
 * @param PHYAddress: external PHY address
 * @retval None
 */
void Eth_Link_EXTIConfig( void );

/**
 * @brief  This function handles Ethernet link status.  It should be called from
 * the actual ISR that handles the EXTI line that is connected to the MII_INT
 * pin on the PHY.
 * @param  None
 * @retval None
 */
void Eth_Link_ITHandler( uint16_t PHYAddress );

#ifdef __cplusplus
}
#endif

/**
 * @} end group groupEthernet
 */
#endif                                               /* __STM32F4x7_ETH_BSP_H */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
