/**
 * @file    stm32f4x7_eth_bsp.h
 * @brief   Driver for the DP83848 Ethernet PHY.
 *
 * This file contains the definitions for the driver for the DP83848 Ethernet PHY
 * as connected to the STM324x9I-EVAL2 dev board.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupEthernet
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"

#include "stm32f4xx.h"                                     /* STM32F4 support */
#include "stm32f4xx_rcc.h"                             /* STM32F4 clk support */
#include "stm32f4xx_dma.h"                             /* STM32F4 DMA support */
#include "stm32f4xx_gpio.h"                           /* STM32F4 gpio support */
#include "stm32f4xx_exti.h"                           /* STM32F4 EXTI support */
#include "stm32f4xx_syscfg.h"         /* STM32F4 System configuration support */
#include "misc.h"                            /* STM32F4 miscellaneous defines */

#include "console_output.h"                         /* Console output support */
#include "lwipopts.h"                        /* LWIP options for this project */
#include "bsp.h"
#include "project_includes.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
DBG_DEFINE_THIS_MODULE( DBG_MODL_ETH ); /* For debug system to ID this module */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  EthInitStatus = 0;
__IO uint8_t EthLinkStatus = 0;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Configures the different GPIO ports.
 * @param  None
 * @retval None
 */
static void ETH_GPIO_Config(void);

/**
 * @brief  Configures the Ethernet Interface
 * @param  None
 * @retval None
 */
static void ETH_MACDMA_Config(void);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void ETH_BSP_Config(void)
{
   /* Configure the GPIO ports for ethernet pins */
   ETH_GPIO_Config();

   /* Configure the Ethernet MAC/DMA */
   ETH_MACDMA_Config();

   if (EthInitStatus == 0) {
      err_slow_printf("Ethernet Init failed\n");
      /* TODO: this can be handled better. Find out how to best tackle this from
       * a system POV.  HR */
      while(1);
   } else {
      dbg_slow_printf("Ethernet Init succeeded\n");
   }

   /* Configure the PHY to generate an interrupt on change of link status
    * This can only be configured if
    * A: there's a physical connection to the MII_INT pin on the PHY.
    * B: the IO expander on the dev kit is configured .
    * Some dev kits (STM324x9I-EVAL2) use the IO expander, while some don't. */

//   Eth_Link_PHYITConfig(DP83848_PHY_ADDRESS);

   /* Configure the EXTI for Ethernet link status. */
//   Eth_Link_EXTIConfig();

}

/******************************************************************************/
static void ETH_MACDMA_Config( void )
{
   /* Enable ETHERNET clocks  */
   RCC_AHB1PeriphClockCmd(
         RCC_AHB1Periph_ETH_MAC |
         RCC_AHB1Periph_ETH_MAC_Tx |
         RCC_AHB1Periph_ETH_MAC_Rx,
         ENABLE
   );

   ETH_InitTypeDef ETH_InitStructure;

   /* Reset ETHERNET on AHB Bus */
   ETH_DeInit();

   /* Software reset */
   ETH_SoftwareReset();

   /* Wait for software reset */
   while (ETH_GetSoftwareResetStatus() == SET);

   /* ETHERNET Configuration --------------------------------------------------*/
   /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameters */
   ETH_StructInit(&ETH_InitStructure);

   /* Fill ETH_InitStructure parameters */
   /*------------------------   MAC   -----------------------------------*/
   ETH_InitStructure.ETH_AutoNegotiation           = ETH_AutoNegotiation_Enable;
   ETH_InitStructure.ETH_Mode                      = ETH_Mode_FullDuplex;
   ETH_InitStructure.ETH_LoopbackMode              = ETH_LoopbackMode_Disable;
   ETH_InitStructure.ETH_RetryTransmission         = ETH_RetryTransmission_Disable;
   ETH_InitStructure.ETH_AutomaticPadCRCStrip      = ETH_AutomaticPadCRCStrip_Disable;
   ETH_InitStructure.ETH_ReceiveAll                = ETH_ReceiveAll_Disable;
   ETH_InitStructure.ETH_BroadcastFramesReception  = ETH_BroadcastFramesReception_Enable;
   ETH_InitStructure.ETH_PromiscuousMode           = ETH_PromiscuousMode_Disable;
   ETH_InitStructure.ETH_MulticastFramesFilter     = ETH_MulticastFramesFilter_Perfect;
   ETH_InitStructure.ETH_UnicastFramesFilter       = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
   ETH_InitStructure.ETH_ChecksumOffload           = ETH_ChecksumOffload_Enable;
#endif

   /*------------------------   DMA   -----------------------------------*/
   /* When we use the Checksum offload feature, we need to enable the Store and
    * Forward mode: the store and forward guarantee that a whole frame is stored
    * in the FIFO, so the MAC can insert/verify the checksum, if the checksum is
    * OK the DMA can handle the frame otherwise the frame is dropped */
   ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
   ETH_InitStructure.ETH_ReceiveStoreForward       = ETH_ReceiveStoreForward_Enable;
   ETH_InitStructure.ETH_TransmitStoreForward      = ETH_TransmitStoreForward_Enable;

   ETH_InitStructure.ETH_ForwardErrorFrames        = ETH_ForwardErrorFrames_Disable;
   ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
   ETH_InitStructure.ETH_SecondFrameOperate        = ETH_SecondFrameOperate_Enable;
   ETH_InitStructure.ETH_AddressAlignedBeats       = ETH_AddressAlignedBeats_Enable;
   ETH_InitStructure.ETH_FixedBurst                = ETH_FixedBurst_Enable;
   ETH_InitStructure.ETH_RxDMABurstLength          = ETH_RxDMABurstLength_32Beat;
   ETH_InitStructure.ETH_TxDMABurstLength          = ETH_TxDMABurstLength_32Beat;
   ETH_InitStructure.ETH_DMAArbitration            = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

   /* Configure Ethernet */
   EthInitStatus = ETH_Init(&ETH_InitStructure, DP83848_PHY_ADDRESS);

   /* Enable the Ethernet Rx and Tx Interrupts */
   ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);
}

/******************************************************************************/
void ETH_GPIO_Config( void )
{
   GPIO_InitTypeDef GPIO_InitStructure;

   /* MII/RMII Media interface selection --------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM324xG-EVAL  */
#ifdef PHY_CLOCK_MCO

   /* Configure MCO (PA8) - This pin is only needed if using an internal
    * oscillator instead of a crystal. */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
   RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
#endif /* PHY_CLOCK_MCO */

   SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#elif defined RMII_MODE  /* Mode RMII with STM324xG-EVAL */

   SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif


   /* Enable SYSCFG clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   /* Enable GPIOs clocks */
   RCC_AHB1PeriphClockCmd(
         RCC_AHB1Periph_GPIOA |
         RCC_AHB1Periph_GPIOB |
         RCC_AHB1Periph_GPIOC |
         RCC_AHB1Periph_GPIOF |
         RCC_AHB1Periph_GPIOG |
         RCC_AHB1Periph_GPIOH |
         RCC_AHB1Periph_GPIOI,
         ENABLE
   );

   /* Ethernet pins configuration *********************************************/
   /*
        ETH_MDIO -------------------------> PA2
        ETH_MDC --------------------------> PC1
        ETH_PPS_OUT ----------------------> PB5
        ETH_MII_CRS ----------------------> PH2 - not used
        ETH_MII_COL ----------------------> PH3 - not used
        ETH_MII_RX_ER --------------------> PI10
        ETH_MII_RXD2 ---------------------> PH6
        ETH_MII_RXD3 ---------------------> PH7
        ETH_MII_TX_CLK -------------------> PC3
        ETH_MII_TXD2 ---------------------> PC2
        ETH_MII_TXD3 ---------------------> PB8
        ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
        ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
        ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
        ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
        ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
        ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
        ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
    */

   /* Common pin configuration */
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

   /* Configure PB5 and PB8 */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);

   /* Configure PC1, PC2, PC3, PC4 and PC5 */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

   /* Configure PG11, PG14 and PG13 */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
   GPIO_Init(GPIOG, &GPIO_InitStructure);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

   /* Configure PH6, PH7 */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_Init(GPIOH, &GPIO_InitStructure);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource7, GPIO_AF_ETH);

}

/******************************************************************************/
uint32_t Eth_Link_PHYITConfig( uint16_t PHYAddress )
{
   uint32_t tmpreg = 0;

   /* Read MICR register */
   tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MICR);

   /* Enable output interrupt events to signal via the INT pin */
   tmpreg |= (uint32_t)PHY_MICR_INT_EN | PHY_MICR_INT_OE;
   if(!(ETH_WritePHYRegister(PHYAddress, PHY_MICR, tmpreg)))
   {
      /* Return ERROR in case of write timeout */
      err_slow_printf("Failed to enable interrupts on PHY\n");
      return ETH_ERROR;
   }

   /* Read MISR register */
   tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MISR);

   /* Enable Interrupt on change of link status */
   tmpreg |= (uint32_t)PHY_MISR_LINK_INT_EN;
   if(!(ETH_WritePHYRegister(PHYAddress, PHY_MISR, tmpreg)))
   {
      /* Return ERROR in case of write timeout */
      err_slow_printf("Failed to enable 'change of link status' interrupt on PHY\n");
      return ETH_ERROR;
   }

   /* Return SUCCESS */
   return ETH_SUCCESS;
}

/******************************************************************************/
void Eth_Link_EXTIConfig( void )
{
   GPIO_InitTypeDef GPIO_InitStructure;
   EXTI_InitTypeDef EXTI_InitStructure;

   /* Enable the INT (PB14) Clock */
   RCC_AHB1PeriphClockCmd(ETH_LINK_GPIO_CLK, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   /* Configure INT pin as input */
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Pin = ETH_LINK_PIN;
   GPIO_Init(ETH_LINK_GPIO_PORT, &GPIO_InitStructure);

   /* Connect EXTI Line to INT Pin */
   SYSCFG_EXTILineConfig(ETH_LINK_EXTI_PORT_SOURCE, ETH_LINK_EXTI_PIN_SOURCE);

   /* Configure EXTI line */
   EXTI_InitStructure.EXTI_Line = ETH_LINK_EXTI_LINE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   /* Enable and set the EXTI interrupt */
   NVIC_Config( EXTI15_10_IRQn, ETH_LINK_PRIO );
}

/******************************************************************************/
void Eth_Link_ITHandler( uint16_t PHYAddress )
{
   /* Check whether the link interrupt has occurred or not */
   if(((ETH_ReadPHYRegister(PHYAddress, PHY_MISR)) & PHY_LINK_STATUS) != 0)
   {
      EthLinkStatus = ~EthLinkStatus;

      if(EthLinkStatus != 0) {
         log_slow_printf("Network cable unplugged\n");
      } else {
         log_slow_printf("Network cable plugged in\n");
      }
   }
}

/**
 * @} end group groupEthernet
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
