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
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "I2CMgr.h"                                    /* For I2C event types */
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define EE_PAGESIZE        16    /**< EEProm's internal page size (in bytes). */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief RX Buffer for I2C device
 */
uint8_t          i2c1RxBuffer[MAX_MSG_LEN];

/**
 * @brief TX Buffer for I2C device
 */
uint8_t          i2c1TxBuffer[MAX_MSG_LEN];

/**
 * @brief An internal structure that holds settings for I2C devices on all I2C
 * busses.
 */
I2C_BusDevice_t s_I2C_Dev[MAX_I2C_DEV] =
{
      {
            /* "External" device settings */
            EEPROM,                    /**< i2c_dev */
            I2C1,                      /**< i2c_bus */
            0xA0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x00                       /**< i2c_mem_addr */
      },
      {
            /* "External" device settings */
            SN_ROM,                    /**< i2c_dev */
            I2C1,                      /**< i2c_bus */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x80                       /**< i2c_mem_addr */
      },
      {
            /* "External" device settings */
            UIE_ROM,                   /**< i2c_dev */
            I2C1,                      /**< i2c_bus */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x90                       /**< i2c_mem_addr */
      }
};

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
            EEPROM,                    /**< i2c_cur_dev */
            I2C_IDLE_ST,               /**< i2c_cur_st */
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
   /* Set up Interrupt controller to handle I2C Event interrupts */
//   NVIC_Config(
//         s_I2C_Bus[iBus].i2c_ev_irq_num,
//         s_I2C_Bus[iBus].i2c_ev_irq_prio
//   );

   /* Set up Interrupt controller to handle I2C Error interrupts */
   NVIC_Config(
         s_I2C_Bus[iBus].i2c_er_irq_num,
         s_I2C_Bus[iBus].i2c_er_irq_prio
   );

   /* Enable All I2C Interrupts */
   I2C_ITConfig(
         s_I2C_Bus[iBus].i2c_bus,
         I2C_IT_ERR, //I2C_IT_EVT | I2C_IT_BUF |
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
uint8_t I2C_getDevAddrSize( I2C_Device_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr_size );
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
//   isr_dbg_slow_printf("\n\n DMA!!!\n\n");
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
//      uint16_t nI2CBusTimeout = MAX_I2C_TIMEOUT;
//      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
//         if((nI2CBusTimeout--) == 0) {
//            ERR_printf("Timeout waiting for I2C Stop bit flag reset!\n");
//
//            /* Error condition.  Make sure to do proper cleanup*/
//            goto I2C1_DMAReadCallback_cleanup;
//         }
//      }

      I2C_AcknowledgeConfig(I2C1, ENABLE);        /* Re-enable Acknowledgment */
      /* End of STM32 I2C HW bug workaround */

      /* Disable DMA so it doesn't keep outputting the buffer. */
      DMA_Cmd( DMA1_Stream0, DISABLE );

      /* Publish event with the read data */
      I2CDataEvt *i2cDataEvt = Q_NEW( I2CDataEvt, I2C_READ_DONE_SIG );
      i2cDataEvt->i2cDevice = s_I2C_Bus[I2CBus1].i2c_cur_dev;
      i2cDataEvt->wDataLen = s_I2C_Bus[I2CBus1].nBytesExpected;
      MEMCPY(
            i2cDataEvt->bufData,
            s_I2C_Bus[I2CBus1].pRxBuffer,
            i2cDataEvt->wDataLen
      );
      QF_PUBLISH( (QEvent *)i2cDataEvt, AO_I2CMgr );

//      DBG_printf("%d loop iters\n", MAX_I2C_TIMEOUT - nI2CBusTimeout);

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
//   isr_dbg_slow_printf("\n\n DMA!!!\n\n");
   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) ) {
      DMA_Cmd( DMA1_Stream6, DISABLE );

      I2C_GenerateSTOP(I2C1, ENABLE);                        /* Generate Stop */

      /* Publish event with the read data */
      I2CEvt *i2cEvt = Q_NEW( I2CEvt, I2C_WRITE_DONE_SIG );
      i2cEvt->i2cDevice = s_I2C_Bus[I2CBus1].i2c_cur_dev;
      i2cEvt->wDataLen = s_I2C_Bus[I2CBus1].nBytesExpected;
      QF_PUBLISH( (QEvent *)i2cEvt, AO_I2CMgr );

      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit( DMA1_Stream6, DMA_IT_TCIF6 );
   }

   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
inline void I2C1_EventCallback( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Get Last I2C Event */
   __IO uint32_t I2CEvent = I2C_GetLastEvent( I2C1 );

   /* Check which I2C device on this bus is being used */
   //   if ( EEPROM == s_I2C_Bus[I2CBus1].i2c_cur_dev ) {
   switch (I2CEvent) {
      //      case 0x00000000:
      //      case 0x00000001:
      //      case 0x00004001:
      //      case 0x00034044:
      //         /* This is the I2C "Event" that is always triggering so we explicitly
      //          * ignore it here */
      //         break;

      case I2C_EVENT_MASTER_MODE_SELECT: /* BUSY, MSL and SB flag */

         if ( I2C_GEN_START_ST == s_I2C_Bus[I2CBus1].i2c_cur_st ) {
            /* Set new state */
            s_I2C_Bus[I2CBus1].i2c_cur_st = I2C_MASTER_TX_MODE_SELECTED_ST;

            isr_dbg_slow_printf("Selecting master\n");
//            DBG_printf("Selecting master\n");
            /* Send slave Address for write */
            I2C_Send7bitAddress(
                  I2C1,                                  // This is always the bus used in this ISR
                  s_I2C_Dev[s_I2C_Bus[I2CBus1].i2c_cur_dev].i2c_dev_addr,   // Look up the current device address for this bus
                  s_I2C_Bus[I2CBus1].bTransDirection     // Direction of data on this bus
            );
         } else if ( I2C_GEN_2ND_START_ST == s_I2C_Bus[I2CBus1].i2c_cur_st ) {

            /* Set the direction to receive */
            I2C_SetDirection( I2CBus1,  I2C_Direction_Receiver);

            /* Send slave Address for read */
            I2C_Send7bitAddress(
                  I2C1,           /* This is always the bus used in this ISR */
                  s_I2C_Dev[s_I2C_Bus[I2CBus1].i2c_cur_dev].i2c_dev_addr,  /* Look up the current device address for this bus */
                  s_I2C_Bus[I2CBus1].bTransDirection    /* Direction of data on this bus */
            );

            isr_dbg_slow_printf("Selecting master again\n");

//            DBG_printf("Selecting master again\n");
         }

         /* Create and publish event for I2CMgr */
//         QEvt *qEvt = Q_NEW( QEvt, I2C_EV_MASTER_MODE_SELECT_SIG );
//         QF_PUBLISH( (QEvt *)qEvt, AO_I2CMgr );

//         isr_dbg_slow_printf("I2C_EVENT_MASTER_MODE_SELECT\n");
         break;

      case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
         isr_dbg_slow_printf("I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED\n");
//         DBG_printf("I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED\n");
         break;

      /* Check on EV6 */
      case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: /* BUSY, MSL, ADDR, TXE and TRA flags */
         isr_dbg_slow_printf("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED\n");
//         DBG_printf("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED\n");
         if ( I2C_MASTER_TX_MODE_SELECTED_ST == s_I2C_Bus[I2CBus1].i2c_cur_st ) {

            /* 2 byte address for device */
            if ( 2 == I2C_getDevAddrSize(s_I2C_Bus[I2CBus1].i2c_cur_dev) ) {
            isr_dbg_slow_printf("MSB\n");

               /* Set new state */
               s_I2C_Bus[I2CBus1].i2c_cur_st = I2C_SENT_MSB_ADDR_ST;

               /* Create and publish event for I2CMgr */
//            QEvt *qEvt1 = Q_NEW( QEvt, I2C_EV_MASTER_TX_MODE_SELECTED_SIG );
//            QF_PUBLISH( (QEvt *)qEvt1, AO_I2CMgr );

               /* Send the MSB of the address first to the I2C device */
               I2C_SendData(
                     I2C1,
                     (uint8_t)((s_I2C_Dev[s_I2C_Bus[I2CBus1].i2c_cur_dev].i2c_mem_addr & 0xFF00) >> 8)
               );
            } else { /* Single byte address for device */
               isr_dbg_slow_printf("Single byte address\n");
               /* Set new state and use the LSB state so we can skip to the next
                * step */
               s_I2C_Bus[I2CBus1].i2c_cur_st = I2C_SENT_LSB_ADDR_ST;

               /* Create and publish event for I2CMgr */
//            QEvt *qEvt1 = Q_NEW( QEvt, I2C_EV_MASTER_TX_MODE_SELECTED_SIG );
//            QF_PUBLISH( (QEvt *)qEvt1, AO_I2CMgr );

               /* Send the entire 1 byte address to the I2C device */
               I2C_SendData(
                     I2C1,
                     (uint8_t)((s_I2C_Dev[s_I2C_Bus[I2CBus1].i2c_cur_dev].i2c_mem_addr & 0x00FF))
               );
            }

//            I2C_GetITStatus()
//            I2C_GetFlagStatus( I2C1, I2C_FLAG_MSL | I2C_FLAG_ADDR | I2C_FLAG_TXE | I2C_FLAG_TRA );
//            isr_dbg_slow_printf("I2C (before) MSL: %x, ADDR: %x, TXE: %x, TRA: %x\n",
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_MSL),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_TRA)
//            );
//            I2C_SendData(I2C1, (uint8_t)((0x0000 & 0xFF00) >> 8));
//
//            isr_dbg_slow_printf("I2C (after) MSL: %x, ADDR: %x, TXE: %x, TRA: %x\n",
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_MSL),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE),
//                  I2C_GetFlagStatus(I2C1, I2C_FLAG_TRA)
//            );
//
//            /*!< Test on EV8 and clear it */
//            uint16_t sEETimeout = 10000;
//            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
////            while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr msb\n");
//                  break;
//               }
//            }
//
//            /*!< Send the EEPROM's internal address to read from: LSB of the address */
//            I2C_SendData(I2C1, (uint8_t)(0x0000 & 0x00FF));
//            sEETimeout = 10000;
//            while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr lsb\n");
//                  break;
//               }
//            }
//            isr_dbg_slow_printf("Sent addr\n");
         }



//            /*!< Send the EEPROM's internal address to read from: MSB of the address first */
//            I2C_SendData(I2C1, (uint8_t)((0x0000 & 0xFF00) >> 8));
//
//            /*!< Test on EV8 and clear it */
//            uint16_t sEETimeout = 10000;
//            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr msb\n");
//               }
//            }
//
//            /*!< Send the EEPROM's internal address to read from: LSB of the address */
//            I2C_SendData(I2C1, (uint8_t)(0x0000 & 0x00FF));
//            sEETimeout = 10000;
//            while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr lsb\n");
//               }
//            }
//            isr_dbg_slow_printf("Sent addr\n");
/* Create and publish event for I2CMgr */
//            QEvt *qEvt = Q_NEW( QEvt, I2C_MSTR_MODE_SELECTED_SIG );
//            QF_PUBLISH( (QEvt *)qEvt, AO_I2CMgr );
         break;

         /* Check on EV8 */
      case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
//         isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTING\n");
         break;

      case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
         isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTED\n");

         if ( I2C_SENT_MSB_ADDR_ST == s_I2C_Bus[I2CBus1].i2c_cur_st ) {

            /* Create and publish event for I2CMgr */
            QEvt *qEvt2 = Q_NEW( QEvt, I2C_SENT_MSB_ADDR_SIG );
            QF_PUBLISH( (QEvt *)qEvt2, AO_I2CMgr );

            /* Set new state */
            s_I2C_Bus[I2CBus1].i2c_cur_st = I2C_SENT_LSB_ADDR_ST;

            /* Send the LSB of the address to the I2C device */
            I2C_SendData(
                  I2C1,
                  (uint8_t)(s_I2C_Dev[s_I2C_Bus[I2CBus1].i2c_cur_dev].i2c_mem_addr & 0xFF00)
            );
            isr_dbg_slow_printf("Sent LSB\n");
         } else if ( I2C_SENT_LSB_ADDR_ST == s_I2C_Bus[I2CBus1].i2c_cur_st ) {
            isr_dbg_slow_printf("Ready to send second start\n");
            /* Set new state */
            s_I2C_Bus[I2CBus1].i2c_cur_st = I2C_GEN_2ND_START_ST;

            /* Send START condition */
            I2C_GenerateSTART( I2C1, ENABLE );
         }



/* Create and publish event for I2CMgr */
//            QEvt *qEvt1 = Q_NEW( QEvt, I2C_MSTR_BYTE_TRANSMITTED_SIG );
//            QF_PUBLISH( (QEvt *)qEvt1, AO_I2CMgr );
//
//         if (Tx_Idx == (uint8_t)NumberOfByteToTransmit) {
//            /* Send STOP condition */
//            I2C_GenerateSTOP(I2C1, ENABLE);
//            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
//         } else {
//            /* Transmit Data TxBuffer */
//            I2C_SendData(I2C1, TxBuffer[Tx_Idx++]);
//         }

         break;

         //         case I2C_EVENT_MASTER_BYTE_RECEIVED:
         //            isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_RECEIVED\n");
         //
         //            /* Read the byte received */
         //            s_I2C_Bus[I2CBus1].pRxBuffer[ s_I2C_Bus[I2CBus1].nRxindex++ ] = I2C_ReceiveData( I2C1 );
         //               ++s_I2C_Bus[I2CBus1].nBytesCurrent;
         //
         //               /* Start
         //                * This is a workaround for the STM32 I2C hardware bug.  You have
         //                * to send the STOP bit before receiving the last byte.  See the
         //                * STM32 errata for more details. */
         //               if ( 1 == s_I2C_Bus[I2CBus1].nBytesExpected - s_I2C_Bus[I2CBus1].nBytesCurrent) {
         //                  /* Disable Acknowledgment */
         //                  I2C_AcknowledgeConfig( I2C1, DISABLE );
         //
         //                  /* Send STOP Condition */
         //                  I2C_GenerateSTOP( I2C1, ENABLE );
         //               }/* End */
         //
         //               /* Check if all the expected bytes have been read in from the bus */
         //               if ( s_I2C_Bus[I2CBus1].nBytesExpected == s_I2C_Bus[I2CBus1].nBytesCurrent ) {
         //                  /* Create an event and fill it with the data from the RX buffer of the bus.*/
         //                  I2CDataEvt *qEvtI2CReadDone = Q_NEW( I2CDataEvt, I2C_READ_DONE_SIG );
         //                  qEvtI2CReadDone->i2cDevice = EEPROM;
         //                  qEvtI2CReadDone->wBufferLen = s_I2C_Bus[I2CBus1].nBytesCurrent;
         //                  MEMCPY(
         //                        qEvtI2CReadDone->buffer,
         //                        s_I2C_Bus[I2CBus1].pRxBuffer,
         //                        qEvtI2CReadDone->wBufferLen
         //                  );
         //                  QF_PUBLISH( (QEvt *)qEvtI2CReadDone, AO_I2CMgr );
         //               }
         //
         //               /* Re-Enable Acknowledgment to be ready for another reception */
         //               I2C_AcknowledgeConfig(I2C1, ENABLE);
         //
         //               break;
      default:
//         isr_dbg_slow_printf("dc: %08x\n", (unsigned int)I2CEvent);
         break;

   }
   //   }else {
   //      isr_dbg_slow_printf("Invalid I2C device selected: %d\n", s_I2C_Bus[I2CBus1].i2c_cur_dev );
   //   }

//   isr_dbg_slow_printf("I2C Event\n");
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

      isr_dbg_slow_printf("I2C Error: 0x%04x\n", regVal);
      I2C_BusInit( I2CBus1 );
   }
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
