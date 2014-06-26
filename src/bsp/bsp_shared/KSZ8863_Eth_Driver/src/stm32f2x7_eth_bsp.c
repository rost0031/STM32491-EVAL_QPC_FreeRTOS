/**
 * @file 	stm32f2x7_eth_bsp.c
 * @brief   This file contains the Board Support Package functions for any
 * 			STM32F2xx board and Micrel KSZ8863 Ethernet switch
 *
 * @date   	09/27/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 *
 * @addtogroup groupKSZ8863
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2x7_eth.h"
#include "stm32f2x7_eth_bsp.h"
#include "stm32f2xx_rcc.h"                           /* For STM32 rcc support */
#include "stm32f2xx_gpio.h"                         /* For STM32 gpio support */
#include "stm32f2xx_syscfg.h"                     /* For STM32 syscfg support */
#include "lwipopts.h"                                     /* For LWIP options */

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/**
 * @brief   This driver uses RMII mode to interface to the processor.
 */
#define RMII_MODE
/* Uncomment the define below to clock the PHY from external 25MHz crystal
 * (only for MII mode) */
#ifdef 	MII_MODE
#define PHY_CLOCK_MCO
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
uint32_t ETH_MACDMA_Config(void)
{
	/* Initialize some variables */
	uint32_t ETH_Status = ETH_ERROR;
	ETH_InitTypeDef ETH_InitStructure;

	/* Enable ETHERNET clock  */
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_ETH_MAC |
			RCC_AHB1Periph_ETH_MAC_Tx |
			RCC_AHB1Periph_ETH_MAC_Rx,
			ENABLE);

	/* Reset ETHERNET on AHB Bus */
	ETH_DeInit();

	/* Software reset */
	ETH_SoftwareReset();

	/* Wait for software reset */
	while (ETH_GetSoftwareResetStatus() == SET);

	/* ETHERNET Configuration --------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);

	/* Fill ETH_InitStructure parametrs */
	/*------------------------   MAC   -----------------------------------*/
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	//ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
	ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;

	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

	/*------------------------   DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	/* Configure Ethernet - The address is being sent in as 0 since we are not doing any actual
	 * PHY configuration.  The ETH_Init function has been modified to not do any PHY configuration */
	ETH_Status = ETH_Init(&ETH_InitStructure, 0);

	/* Enable the Ethernet Rx and Tx Interrupts */
	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);

	return(ETH_Status);
}

/******************************************************************************/
void ETH_GPIO_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure ETH_RESET (PA8) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* MII/RMII Media interface selection --------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM322xG-EVAL  */
#ifdef PHY_CLOCK_MCO

	/* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
	RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
#endif /* PHY_CLOCK_MCO */

	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#elif defined RMII_MODE  /* Mode RMII */

	/* Output PLL clock divided by 2 (50MHz) on MCO pin (PA8) to clock the PHY */
	// There's an external 50MHz xtal driving this clock as opposed to using the processor PLL
	//RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_2);

	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif

	/* Ethernet pins configuration ************************************************/
	/*    Redwood_H1_Board
       ETH_RST --------------------------> PA8
       ETH_MDIO -------------------------> PA2
       ETH_MDC --------------------------> PC1
       ETH_RMII_REF_CLK -----------------> PA1
       ETH_RMII_CRS_DV ------------------> PA7
       ETH_RMII_RXD0 --------------------> PC4
       ETH_RMII_RXD1 --------------------> PC5
       ETH_RMII_TX_EN -------------------> PB11
       ETH_RMII_TXD0 --------------------> PB12
       ETH_RMII_TXD1 --------------------> PB13
	 */

	/* Configure the rest of the pins for Alternate Function (AF) */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

	/* Configure PA1, PA2 and PA7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	/* Configure PB11, PB12, and PB13 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);

	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
}

/**
 * @}
 * end addtogroup groupKSZ8863
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/

