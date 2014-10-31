/**
 * @file 	i2c.c
 * @brief   Basic driver for I2C bus.
 *
 * @date   	06/30/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "i2c_dev.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "I2CBusMgr.h"                                 /* For I2C event types */
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief RX Buffer for I2C1 bus
 */
uint8_t          i2c1RxBuffer[MAX_I2C_READ_LEN];

/**
 * @brief TX Buffer for I2C1 bus
 */
uint8_t          i2c1TxBuffer[MAX_I2C_WRITE_LEN];

/**
 * @brief An internal structure that holds almost all the settings for the I2C
 * Busses
 */
I2C_BusSettings_t s_I2C_Bus[MAX_I2C_BUS] =
{
      {
            I2CBus1,                   /**< i2c_sys_bus */

            /* I2C bus settings */
            I2C1,                      /**< i2c_bus */
            100000,                    /**< i2c_bus_speed (Hz)*/
            RCC_APB1Periph_I2C1,       /**< i2c_clk */

            /* I2C interrupt settings */
            I2C1_EV_IRQn,              /**< i2c_ev_irq_num */
            I2C1_EV_PRIO,              /**< i2c_ev_irq_prio */
            I2C1_ER_IRQn,              /**< i2c_er_irq_num */
            I2C1_ER_PRIO,              /**< i2c_er_irq_prio */

            /* I2C GPIO settings for SCL */
            GPIOB,                     /**< scl_port */
            GPIO_Pin_6,                /**< scl_pin */
            GPIO_PinSource6,           /**< scl_af_pin_source */
            GPIO_AF_I2C1,              /**< scl_af */
            RCC_AHB1Periph_GPIOB,      /**< scl_clk */

            /* I2C GPIO settings for SDA */
            GPIOB,                     /**< sda_port */
            GPIO_Pin_9,                /**< sda_pin */
            GPIO_PinSource9,           /**< sda_af_pin_source */
            GPIO_AF_I2C1,              /**< sda_af */
            RCC_AHB1Periph_GPIOB,      /**< sda_clk */

            /* Common I2C DMA settings */
            DMA1,                      /**< i2c_dma */
            DMA_Channel_1,             /**< i2c_dma_channel */
            (uint32_t)&(I2C1->DR),     /**< i2c_dma_dr_addr */
            RCC_AHB1Periph_DMA1,       /**< i2c_dma_clk */

            /* TX I2C DMA settings */
            DMA1_Stream6,              /**< i2c_dma_tx_stream */
            DMA1_Stream6_IRQn,         /**< i2c_dma_tx_irq_num */
            DMA1_Stream6_PRIO,         /**< i2c_dma_tx_irq_prio */
            DMA_FLAG_TCIF6 |
            DMA_FLAG_FEIF6 |
            DMA_FLAG_DMEIF6 |
            DMA_FLAG_TEIF6 |
            DMA_FLAG_HTIF6,            /**< i2c_dma_tx_flags */

            /* RX I2C DMA settings */
            DMA1_Stream0,              /**< i2c_dma_rx_stream */
            DMA1_Stream0_IRQn,         /**< i2c_dma_rx_irq_num */
            DMA1_Stream0_PRIO,         /**< i2c_dma_rx_irq_prio */
            DMA_FLAG_TCIF0 |
            DMA_FLAG_FEIF0 |
            DMA_FLAG_DMEIF0 |
            DMA_FLAG_TEIF0 |
            DMA_FLAG_HTIF0,            /**< i2c_dma_rx_flags */

            /* Buffer management */
            &i2c1RxBuffer[0],          /**< *pRxBuffer */
            0,                         /**< nRxindex */
            &i2c1TxBuffer[0],          /**< *pTxBuffer */
            0,                         /**< nTxindex */

            /* Device management */
            I2C_Direction_Transmitter, /**< bTransDirection */
            0,                         /**< nBytesExpected */
            0,                         /**< nBytesCurrent */
      }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void I2C_BusInit( I2C_Bus_t iBus )
{
   /* Deinit the bus just in case. */
   I2C_BusDeInit( iBus );

   /* 1. Enable clocks ------------------------------------------------------ */
   /* Enable I2C clock */
   RCC_APB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_clk, ENABLE );

   /* Enable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         ENABLE
   );

   /* Enable the SYSCFG clock */
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE );

   /* Enable I2C DMA clock */
   RCC_AHB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_dma_clk, ENABLE );

   /* 2. Set up GPIO -------------------------------------------------------- */
   /* Connect PXx to I2C SCL alt function. */
   GPIO_PinAFConfig(
         s_I2C_Bus[iBus].scl_port,
         s_I2C_Bus[iBus].scl_af_pin_source,
         s_I2C_Bus[iBus].scl_af
   );

   /* Connect PXx to I2C SDA alt function. */
   GPIO_PinAFConfig(
         s_I2C_Bus[iBus].sda_port,
         s_I2C_Bus[iBus].sda_af_pin_source,
         s_I2C_Bus[iBus].sda_af
   );

   /* Configure common elements of the SDA and SCL pins */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_OD;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

   /* 3. Set up DMA I2C settings. The R/W function will set the structure --- */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );

   /* 4. Set up interrupts -------------------------------------------------- */
   /* Set up Interrupt controller to handle I2C Error interrupts */
   NVIC_Config(
         s_I2C_Bus[iBus].i2c_er_irq_num,
         s_I2C_Bus[iBus].i2c_er_irq_prio
   );

   /* Enable I2C Error Interrupts */
   I2C_ITConfig(
         s_I2C_Bus[iBus].i2c_bus,
         I2C_IT_ERR,
         ENABLE
   );

   /* Initialize the IRQ and priorities for I2C DMA */
   NVIC_Config(
         s_I2C_Bus[iBus].i2c_dma_rx_irq_num,
         s_I2C_Bus[iBus].i2c_dma_rx_irq_prio
   );

   NVIC_Config(
         s_I2C_Bus[iBus].i2c_dma_tx_irq_num,
         s_I2C_Bus[iBus].i2c_dma_tx_irq_prio
   );

   /* I2C configuration */
   I2C_InitTypeDef  I2C_InitStructure;
   I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C; /* Mode for the M24CXX EEPROM used by the STM324x9I-EVAL2 dev kit */
   I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1         = 0x00;
   I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed          = s_I2C_Bus[iBus].i2c_bus_speed;

   /* Apply I2C configuration */
   I2C_Init( s_I2C_Bus[iBus].i2c_bus, &I2C_InitStructure );
   I2C_AcknowledgeConfig( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Configure I2C Filters */
   I2C_AnalogFilterCmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );
   I2C_DigitalFilterConfig( s_I2C_Bus[iBus].i2c_bus, 0x0F );

   /* I2C Peripheral Enable */
//   I2C_Cmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

}

/******************************************************************************/
void I2C_BusDeInit( I2C_Bus_t iBus )
{
   /* DeInit the I2C IP ----------------------------------------------------- */
   I2C_DeInit( s_I2C_Bus[iBus].i2c_bus );

   /* Disable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         DISABLE
   );

   /* Disable I2C DMA clock */
   RCC_AHB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_dma_clk, DISABLE );

   /* 2. Set up GPIO for input ---------------------------------------------- */

   /* Configure common elements of the SDA and SCL pins to not use I2C periph */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

   /* 3. DeInit DMA I2C ----------------------------------------------------- */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );
}

/******************************************************************************/
void I2C_BusInitForRecovery( I2C_Bus_t iBus )
{
   /* 1. DeInit the I2C bus */
   I2C_BusDeInit( iBus );

   /* 2. Set up GPIO for output --------------------------------------------- */

   /* Enable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         ENABLE
   );

   /* Configure common elements of the SDA and SCL pins to not use I2C periph */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C - Output for this pin since SCL
    * is the pin that will be toggled */
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C - Input for this pin since SDA
    * needs to be checked to see if it's cleared after toggling the SCL line */
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

}

/******************************************************************************/
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction )
{
   /* Check inputs */
   assert_param( IS_I2C_DIRECTION( I2C_Direction ) );
   assert_param( IS_I2C_BUS( iBus ) );

   s_I2C_Bus[iBus].bTransDirection = I2C_Direction;
}

/******************************************************************************/
uint8_t I2C_getDirection( I2C_Bus_t iBus )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   return( s_I2C_Bus[iBus].bTransDirection );
}

/******************************************************************************/
void I2C_StartDMARead( I2C_Bus_t iBus, uint16_t wReadLen )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   /* Set the structure buffer management for how many bytes to expect and how
    * many are currently there (none). */
   s_I2C_Bus[iBus].nBytesExpected = wReadLen;
   s_I2C_Bus[iBus].nBytesCurrent  = 0;
   s_I2C_Bus[iBus].nRxIndex       = 0;

   /* Clear out the DMA settings */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   /* Set up a new DMA structure for reading */
   DMA_InitTypeDef    DMA_InitStructure;
   DMA_InitStructure.DMA_Channel             = s_I2C_Bus[iBus].i2c_dma_channel;
   DMA_InitStructure.DMA_PeripheralBaseAddr  = s_I2C_Bus[iBus].i2c_dma_dr_addr;
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Enable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;
   DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)s_I2C_Bus[iBus].pRxBuffer;
   DMA_InitStructure.DMA_BufferSize          = s_I2C_Bus[iBus].nBytesExpected;

   DBG_printf("Attempting to read via DMA %d bytes\n", DMA_InitStructure.DMA_BufferSize);

   /* Initialize the DMA with the filled in structure */
   DMA_Init( s_I2C_Bus[iBus].i2c_dma_rx_stream, &DMA_InitStructure );

   /* Enable the TransferComplete interrupt in the DMA */
   DMA_ITConfig( s_I2C_Bus[iBus].i2c_dma_rx_stream, DMA_IT_TC, ENABLE );

   /* Clear any pending flags on RX Stream */
   DMA_ClearFlag(
         s_I2C_Bus[iBus].i2c_dma_rx_stream,
         s_I2C_Bus[iBus].i2c_dma_rx_flags
   );

   /* I2Cx DMA Enable */
   I2C_DMACmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Finally, activate DMA */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, ENABLE );

}

/******************************************************************************/
void I2C_StartDMAWrite( I2C_Bus_t iBus, uint16_t wWriteLen )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   /* Set the structure buffer management for how many bytes to expect and how
    * many are currently there (none). */
   s_I2C_Bus[iBus].nBytesExpected = wWriteLen;
   s_I2C_Bus[iBus].nBytesCurrent  = 0;
   s_I2C_Bus[iBus].nTxIndex       = 0;

   /* Clear out the DMA settings */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );

   /* Set up a new DMA structure for reading */
   DMA_InitTypeDef    DMA_InitStructure;
   DMA_InitStructure.DMA_Channel             = s_I2C_Bus[iBus].i2c_dma_channel;
   DMA_InitStructure.DMA_PeripheralBaseAddr  = s_I2C_Bus[iBus].i2c_dma_dr_addr;
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Enable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;
   DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)s_I2C_Bus[iBus].pTxBuffer;
   DMA_InitStructure.DMA_BufferSize          = s_I2C_Bus[iBus].nBytesExpected;

   DBG_printf("Attempting to write via DMA %d bytes\n", DMA_InitStructure.DMA_BufferSize);

   /* Initialize the DMA with the filled in structure */
   DMA_Init( s_I2C_Bus[iBus].i2c_dma_tx_stream, &DMA_InitStructure );

   /* Enable the TransferComplete interrupt in the DMA */
   DMA_ITConfig( s_I2C_Bus[iBus].i2c_dma_tx_stream, DMA_IT_TC, ENABLE );

   /* Clear any pending flags on RX Stream */
   DMA_ClearFlag(
         s_I2C_Bus[iBus].i2c_dma_tx_stream,
         s_I2C_Bus[iBus].i2c_dma_tx_flags
   );

   /* I2Cx DMA Enable */
   I2C_DMACmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Finally, activate DMA */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, ENABLE );
}


/******************************************************************************/
inline void I2C1_DMAReadCallback( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0) ) {
      /* Start of STM32 I2C HW bug workaround:
       * This is a workaround for the STM32 I2C hardware bug.  You have to send
       * the STOP bit before receiving the last byte.  In the case of DMA xfers,
       * this means doing it before you shut off the DMA stream.
       * See the STM32 errata for more details. - HR */

      I2C_AcknowledgeConfig( I2C1, DISABLE);        /* Disable Acknowledgment */

      I2C_GenerateSTOP(I2C1, ENABLE);                        /* Generate Stop */

      /* Wait for the byte to be received.
       * Note: There's really no way around waiting for this flag to be reset
       * outside of the ISR since you have to do this BEFORE you shut off DMA
       * which has to be done in this ISR. Thankfully, this flag gets reset
       * fairly quickly (125 times through the loop). - HR. */
      uint16_t nI2CBusTimeout = MAX_I2C_TIMEOUT;
      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
         if((nI2CBusTimeout--) == 0) {
            ERR_printf("Timeout waiting for I2C Stop bit flag reset!\n");

            /* Error condition.  Make sure to do proper cleanup*/
            goto I2C1_DMAReadCallback_cleanup;
         }
      }

      I2C_AcknowledgeConfig(I2C1, ENABLE);        /* Re-enable Acknowledgment */
      /* End of STM32 I2C HW bug workaround */

      /* Disable DMA so it doesn't keep outputting the buffer. */
      DMA_Cmd( DMA1_Stream0, DISABLE );

//      /* Publish event with the read data directly to the target AO */
//      I2CBusDataEvt *i2cBusDataEvt = Q_NEW( I2CBusDataEvt, I2C_READ_DONE_SIG );
//      i2cBusDataEvt->i2cBus = I2CBus1;
//      i2cBusDataEvt->errorCode = ERR_NONE;
//      i2cBusDataEvt->dataLen = s_I2C_Bus[I2CBus1].nBytesExpected;
//      MEMCPY(
//            i2cBusDataEvt->dataBuf,
//            s_I2C_Bus[I2CBus1].pRxBuffer,
//            i2cBusDataEvt->dataLen
//      );
//      QACTIVE_POST(AO_I2CBusMgr[I2CBus1], &i2cBusDataEvt, AO_I2CBusMgr[I2CBus1]);
//      QF_PUBLISH( (QEvent *)i2cBusDataEvt, AO_I2CBusMgr[I2CBus1] );

      /* Don't transport the data with the event.  The appropriate I2CBusMgr AO
       * will handle copying out the data from the buffers.  Nobody else is
       * allowed to touch this bus so there's no contention. */
      static QEvt const qEvt = { I2C_BUS_DMA_DONE_SIG, 0U, 0U };
      QACTIVE_POST(AO_I2CBusMgr[I2CBus1], &qEvt, AO_I2CBusMgr[I2CBus1]);

//      DBG_printf("%d loop iters\n", MAX_I2C_TIMEOUT - nI2CBusTimeout);

/* Tag for a common exit from this handler.  DON'T call return.  Instead, use
 * goto I2C1_DMAReadCallback_cleanup;
 * This allows for always correctly clearing the interrupt status bits. Else
 * things will lock up. */
I2C1_DMAReadCallback_cleanup:

      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit( DMA1_Stream0, DMA_IT_TCIF0 );
   }

   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
inline void I2C1_DMAWriteCallback( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) ) {
      DMA_Cmd( DMA1_Stream6, DISABLE );

      I2C_GenerateSTOP(I2C1, ENABLE);                        /* Generate Stop */

      /* Publish event with the read data */
//      I2CEvt *i2cEvt = Q_NEW( I2CEvt, I2C_WRITE_DONE_SIG );
//      i2cEvt->i2cDevice = s_I2C_Bus[I2CBus1].i2c_cur_dev;
//      i2cEvt->wDataLen = s_I2C_Bus[I2CBus1].nBytesExpected;
//      QF_PUBLISH( (QEvent *)i2cEvt, AO_I2CMgr );
      static QEvt const qEvt = { I2C_BUS_DMA_DONE_SIG, 0U, 0U };
      QACTIVE_POST(AO_I2CBusMgr[I2CBus1], &qEvt, AO_I2CBusMgr[I2CBus1]);

/* Tag for a common exit from this handler.  DON'T call return.  Instead, use
 * goto I2C1_DMAWriteCallback_cleanup;
 * This allows for always correctly clearing the interrupt status bits. Else
 * things will lock up. */
//I2C1_DMAWriteCallback_cleanup:  // Currently not used.
      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit( DMA1_Stream6, DMA_IT_TCIF6 );
   }

   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
inline void I2C1_EventCallback( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
inline void I2C1_ErrorEventCallback( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Read SR1 register to get I2C error */
   __IO uint16_t regVal = I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0xFF00;
   if (regVal != 0x0000) {
      /* Clears error flags */
      I2C1->SR1 &= 0x00FF;

      ERR_printf("I2C Error: 0x%04x.  Resetting error bits\n", regVal);
      I2C_BusInit( I2CBus1 );
   }
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
