/**
 * @file   serial.c
 * @brief  Definitions for USART/UART/serial port.
 *
 * @date   05/27/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSerial
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "serial.h"
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "stm32f4xx_rcc.h"                         /* For STM32F4 clk support */
#include "stm32f4xx_dma.h"                         /* For STM32F4 DMA support */
#include "stm32f4xx_gpio.h"                       /* For STM32F4 gpio support */
#include "stm32f4xx_usart.h"                      /* For STM32F4 uart support */

#include "assert.h"
#include "project_includes.h"
#include "base64_wrapper.h"
#include <stdio.h>
#include "qp_port.h"                                        /* for QP support */
#include "CBSignals.h"
#include "CBErrors.h"
#include "bsp.h"
#include "SerialMgr.h"
#include "DbgMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_SERIAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/**
 * @brief Maximum Timeout values for flags and events waiting loops.
 * These timeouts are not based on accurate values, they just guarantee that
 * the application will not remain stuck if the serial communication is
 * corrupted.  You may modify these timeout values depending on CPU frequency
 * and application conditions (interrupts routines ...).
 */
#define SERIAL_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SERIAL_LONG_TIMEOUT         ((uint32_t)(10 * SERIAL_FLAG_TIMEOUT))

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief Buffers for Serial interfaces
 */
static char          Uart1TxBuffer[MAX_MSG_LEN];
static char          Uart1RxBuffer[MENU_MAX_CMD_LEN];
/**
 * @brief An internal array of structures that holds almost all the settings for
 * the all serial ports used in the system.
 */
static USART_Settings_t a_UARTSettings[SERIAL_MAX] =
{
      {
            SERIAL_UART1,                /**< port */

            /* USART settings */
            USART1,                    /**< usart */
            115200,                    /**< usart_baud */
            RCC_APB2Periph_USART1,     /**< usart_clk */
            USART1_IRQn,               /**< usart_irq_num */
            USART1_PRIO,               /**< usart_irq_prio */

            GPIOA,                     /**< tx_port */
            GPIO_Pin_9,                /**< tx_pin */
            GPIO_PinSource9,           /**< tx_af_pin_source */
            GPIO_AF_USART1,            /**< tx_af */
            RCC_AHB1Periph_GPIOA,      /**< tx_gpio_clk */

            GPIOA,                     /**< rx_port */
            GPIO_Pin_10,               /**< rx_pin */
            GPIO_PinSource10,          /**< rx_af_pin_source */
            GPIO_AF_USART1,            /**< rx_af */
            RCC_AHB1Periph_GPIOA,      /**< rx_gpio_clk */

            /* Buffer management */
            &Uart1TxBuffer[0],         /**< *bufferTX */
            0,                         /**< indexTX */
            &Uart1RxBuffer[0],         /**< *bufferTX */
            0,                         /**< indexRX */
      }
};

static USART_DMA_Settings_t a_UARTDMASettings[SERIAL_MAX] =
{
      {
            SERIAL_UART1,                /**< port */

            DMA2_Stream7_IRQn,         /**< dma_irq_num */
            DMA2_Stream7_PRIO,         /**< dma_irq_prio */
            DMA_Channel_4,             /**< dma_channel */
            DMA2_Stream7,              /**< dma_stream */
            RCC_AHB1Periph_DMA2,       /**< dma_clk */
      }
};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void Serial_Init(
      SerialPort_T serial_port
)
{
   assert(serial_port < SERIAL_MAX);/* Don't allow goofy values for settings */

   /* 1 - Enable all the necessary clocks ---------------------------------- */
   /* Enable UART TX AND RX GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         a_UARTSettings[serial_port].tx_gpio_clk | a_UARTSettings[serial_port].rx_gpio_clk,
         ENABLE
   );

   if ( SERIAL_UART1 == serial_port ) {
      /* Rest of USARTs use a different APBus1 */
      RCC_APB2PeriphClockCmd( a_UARTSettings[serial_port].usart_clk, ENABLE );
   }

   /* 2 - Set all the GPIO ------------------------------------------------- */
   /* Set up alt function */
   GPIO_PinAFConfig(
         a_UARTSettings[serial_port].tx_port,
         a_UARTSettings[serial_port].tx_af_pin_source,
         a_UARTSettings[serial_port].tx_af
   );

   GPIO_PinAFConfig(
         a_UARTSettings[serial_port].rx_port,
         a_UARTSettings[serial_port].rx_af_pin_source,
         a_UARTSettings[serial_port].rx_af
   );

   /* Configure basic alt function structure common to TX and RX */
   GPIO_InitTypeDef   GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;

   /* Configure USART TX */
   GPIO_InitStructure.GPIO_Pin     = a_UARTSettings[serial_port].tx_pin;
   GPIO_Init(a_UARTSettings[serial_port].tx_port, &GPIO_InitStructure);

   /* Configure USART RX */
   GPIO_InitStructure.GPIO_Pin     = a_UARTSettings[serial_port].rx_pin;
   GPIO_Init(a_UARTSettings[serial_port].rx_port, &GPIO_InitStructure);

   /* 3 - Set all the USART ------------------------------------------------ */
   /* All system serial devices are configured as follows:
       - BaudRate = 115200 baud
       - Word Length = 8 Bits
       - One Stop Bit
       - No parity
       - Hardware flow control disabled (RTS and CTS signals)
       - Receive and transmit enabled

       If this changes as more serial devices are added, make sure to either
       include the new settings in the a_UARTSettings array of settings or use
       a case statement to decide which devices get which settings.
    */
   USART_InitTypeDef  USART_InitStructure;
   USART_InitStructure.USART_Parity         = USART_Parity_No;

   USART_InitStructure.USART_BaudRate       = a_UARTSettings[serial_port].usart_baud;
   USART_InitStructure.USART_WordLength     = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits       = USART_StopBits_1;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode           = USART_Mode_Rx | USART_Mode_Tx;

   /* USART configuration */
   USART_Init( a_UARTSettings[serial_port].usart, &USART_InitStructure );

   NVIC_Config(
         a_UARTSettings[serial_port].usart_irq_num,
         a_UARTSettings[serial_port].usart_irq_prio
   );

   /* Enable USART interrupts */
   USART_ITConfig(
         a_UARTSettings[serial_port].usart,                   /* Which USART */
         USART_IT_RXNE,                         /* Which interrupt to choose */
         ENABLE                                         /* ENABLE or DISABLE */
   );

   /* Enable USART */
   USART_Cmd( a_UARTSettings[serial_port].usart, ENABLE );

}

/******************************************************************************/
void Serial_DMAConfig(
      SerialPort_T serial_port,
      char *pBuffer,
      uint16_t wBufferLen
)
{
   assert(wBufferLen <= MAX_MSG_LEN);

   /* Enable the DMA clock */
   RCC_AHB1PeriphClockCmd( a_UARTDMASettings[serial_port].dma_clk, ENABLE );

   /* Set up Interrupt controller to handle USART DMA */
   NVIC_Config(
         a_UARTDMASettings[serial_port].dma_irq_num,
         a_UARTDMASettings[serial_port].dma_irq_prio
   );

   /* Copy over the buffer and index. TODO: maybe do this in the StartXfer()? */
   a_UARTSettings[serial_port].indexTX = wBufferLen;
   MEMCPY( a_UARTSettings[serial_port].bufferTX, pBuffer, wBufferLen );

   DMA_DeInit( a_UARTDMASettings[serial_port].dma_stream );

   DMA_InitTypeDef  DMA_InitStructure;
   DMA_InitStructure.DMA_Channel             = a_UARTDMASettings[serial_port].dma_channel;
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral; // Transmit
   DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)a_UARTSettings[serial_port].bufferTX;
   DMA_InitStructure.DMA_BufferSize          = (uint16_t)a_UARTSettings[serial_port].indexTX;
   DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)&(a_UARTSettings[serial_port].usart)->DR;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Enable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

   DMA_Init( a_UARTDMASettings[serial_port].dma_stream, &DMA_InitStructure );

   /* Enable the USART Tx DMA request */
   USART_DMACmd( a_UARTSettings[serial_port].usart, USART_DMAReq_Tx, ENABLE );

   /* Enable DMA Stream Transfer Complete interrupt */
   DMA_ITConfig( a_UARTDMASettings[serial_port].dma_stream, DMA_IT_TC, ENABLE );
}

/******************************************************************************/
void Serial_DMAStartXfer(
      SerialPort_T serial_port
)
{
   /* Enable the DMA TX Stream */
   DMA_Cmd(a_UARTDMASettings[serial_port].dma_stream, ENABLE);
}

/******************************************************************************/
uint32_t Serial_send_base64_enc_msg(
      SerialPort_T serial_port,
      char *message,
      uint16_t len
)
{
   char encoded_serial_buf[MAX_MSG_LEN];
   int encoded_sz = base64_encode( message, len, encoded_serial_buf, MAX_MSG_LEN );

   if(encoded_sz < 1)
   {
      err_slow_printf("Encoding failed\n");
      return ERR_SERIAL_MSG_BASE64_ENC_FAILED;
   }

   /* Return the result of the raw message send function */
   return( Serial_send_raw_msg( serial_port, encoded_serial_buf, encoded_sz ) );
}

/******************************************************************************/
uint32_t Serial_send_raw_msg(
      SerialPort_T serial_port,
      char *message,
      uint16_t len
)
{
   if ( len >= MAX_MSG_LEN ) {
      return ERR_SERIAL_MSG_TOO_LONG;
   }

   /* Assumes nothing about the size of the message, send it all and let the
    * receiver interpret it. It is up to the sender and receiver to agree on a
    * terminating character.  In most cases, base64 encoded messages will be
    * sent and they use a newline to terminate.  In some cases (scanner), this
    * will not be the case */
   for ( uint16_t i = 0; i < len; i++ ) {
      uint32_t timeout = SERIAL_LONG_TIMEOUT;   // every byte starts a new timeout count

      while( RESET == USART_GetFlagStatus( a_UARTSettings[ serial_port ].usart, USART_FLAG_TXE ) ) {
         if( (timeout--) <= 0 ) {
            err_slow_printf("!!! - Hardware not responding while trying to send a serial msg\n");
            return ERR_SERIAL_HW_TIMEOUT;
         }
      }
      USART_SendData( a_UARTSettings[ serial_port ].usart, message[i] );
   }

   return ERR_NONE;
}

/******************************************************************************/
inline void Serial_DMASendCallback( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) ) {
      /* Disable DMA so it doesn't keep outputting the buffer. */
      DMA_Cmd(DMA2_Stream7, DISABLE);

      /* Publish event stating that the count has been reached */
      QEvt *qEvt = Q_NEW(QEvt, UART_DMA_DONE_SIG);
      QF_PUBLISH((QEvent *)qEvt, AO_SerialMgr );

      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
   }

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   /* the usual end of FreeRTOS ISR... */
   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

/******************************************************************************/
inline void Serial_UART1Callback(void)
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   while (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
      uint8_t data = (uint8_t)USART_ReceiveData(USART1);

      if ( '\n' == data && a_UARTSettings[SERIAL_UART1].indexRX > 0 ) {

         /* 1. Construct a new msg event indicating that a msg has been received */
//         MsgEvt *msgEvt = Q_NEW( MsgEvt, MSG_RECEIVED_SIG );
//
//         /* 2. Fill the msg payload with payload (the actual received msg)*/
//         MEMCPY(
//               msgEvt->msg,
//               a_UARTSettings[SERIAL_UART1].bufferRX,
//               a_UARTSettings[SERIAL_UART1].indexRX
//         );
//         msgEvt->msg_len = a_UARTSettings[SERIAL_UART1].indexRX;
//         msgEvt->msg_src = SERIAL_UART1;
//
//         /* 3. Publish the newly created event to current AO */
//         QF_PUBLISH( (QEvent *)msgEvt, AO_SerialMgr );

         /* If a newline is received and the buffer is not empty, post event
          * with the buffer data */
         a_UARTSettings[SERIAL_UART1].bufferRX[ a_UARTSettings[SERIAL_UART1].indexRX++ ] = data;

         /* Serial can only receive menu commands */
         MenuEvt *menuEvt = Q_NEW( MenuEvt, DBG_MENU_REQ_SIG );

         /* 2. Fill the msg payload with payload (the actual received msg)*/
         MEMCPY(
               menuEvt->buffer,
               a_UARTSettings[SERIAL_UART1].bufferRX,
               a_UARTSettings[SERIAL_UART1].indexRX
         );
         menuEvt->bufferLen = a_UARTSettings[SERIAL_UART1].indexRX;
         menuEvt->msgSrc = SERIAL_CON;

         /* 3. Publish the newly created event to current AO */
         QF_PUBLISH( (QEvent *)menuEvt, AO_SerialMgr );

         a_UARTSettings[SERIAL_UART1].indexRX = 0;       /* Reset the RX buffer */

      } else if ( '\r' == data ) {
         /* If a linefeed is received, toss it out. */
         data = 0;
      } else {
         if ( a_UARTSettings[SERIAL_UART1].indexRX >= MENU_MAX_CMD_LEN ) {
            WRN_printf(
                  "Attempting to RX a serial msg over %d bytes which will overrun the buffer. Ignoring\n",
                  MENU_MAX_CMD_LEN
            );
            a_UARTSettings[SERIAL_UART1].indexRX = 0;       /* Reset the RX buffer */
         } else {
            /* If any other data is recieved, add it to the buffer */
            a_UARTSettings[SERIAL_UART1].bufferRX[ a_UARTSettings[SERIAL_UART1].indexRX++ ] = data;
         }
      }


   }

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   /* the usual end of FreeRTOS ISR... */
   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}
/**
 * @}
 * end addtogroup groupSerial
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
