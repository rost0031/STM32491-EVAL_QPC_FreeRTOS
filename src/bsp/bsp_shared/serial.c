// $Id$
/**
 * @file   serial.c
 * @brief  This file contains the definitions for serial port control.
 *
 * @date   05/27/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */
// $Log$

#include "serial.h"
#include "stm32f2xx.h"
#include "project_includes.h"
#include "base64_wrapper.h"
#include <stdio.h>
#include "qp_port.h"                                               /* QP-port */
#include "CommStackMgr.h"
#include "CBErrors.h"
#include <assert.h>

/**
 * @brief Definition for the debug, connected to UART4
 */
#define DEBUG_USART                    UART4
#define DEBUG_CLK                      RCC_APB1Periph_UART4
#define DEBUG_TX_PIN                   GPIO_Pin_10
#define DEBUG_TX_GPIO_PORT             GPIOC
#define DEBUG_TX_SOURCE                GPIO_PinSource10
#define DEBUG_TX_AF                    GPIO_AF_UART4
#define DEBUG_RX_PIN                   GPIO_Pin_11
#define DEBUG_RX_GPIO_PORT             GPIOC
#define DEBUG_RX_SOURCE                GPIO_PinSource11
#define DEBUG_RX_AF                    GPIO_AF_UART4
#define DEBUG_IRQn                     UART4_IRQn
#define DEBUG_BAUD                     115200

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the serial communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define SERIAL_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SERIAL_LONG_TIMEOUT         ((uint32_t)(10 * SERIAL_FLAG_TIMEOUT))

/* Buffers for Serial interfaces */
static char          system_serial_buffer[MAX_MSG_LEN];

/**
 * An internal structure that holds almost all the settings for the USART ports
 */
static USART_Settings s_USART_Port[MAX_SERIAL] =
{
      {
            SYSTEM_SERIAL,              // port

            /* USART settings */
            DEBUG_USART,                // usart;
            DEBUG_BAUD,                 // usart_baud;
            DEBUG_CLK,                  // usart_clk;
            DEBUG_IRQn,                 // usart_irq_num;

            DEBUG_TX_GPIO_PORT,         // tx_port;
            DEBUG_TX_PIN,               // tx_pin;
            DEBUG_TX_SOURCE,            // tx_af_pin_source;
            DEBUG_TX_AF,                // tx_af;

            DEBUG_RX_GPIO_PORT,         // rx_port;
            DEBUG_RX_PIN,               // rx_pin;
            DEBUG_RX_SOURCE,            // rx_af_pin_source;
            DEBUG_RX_AF,                // rx_af;

            /* Buffer management */
            &system_serial_buffer[0],   // *buffer;
            0                           // index
      }
};

/******************************************************************************/
void Serial_Init( USART_Port serial_port )
{
    assert(serial_port < MAX_SERIAL);

    NVIC_InitTypeDef   NVIC_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    USART_InitTypeDef  USART_InitStructure;

    /* Rest of USARTs use a different APBus1 */
    RCC_APB1PeriphClockCmd( s_USART_Port[serial_port].usart_clk, ENABLE );

    /* USART configured RFID1, RFID2, and SYSTEM_SERIAL as follows:
       - BaudRate = 115200 baud
       - Word Length = 8 Bits
       - One Stop Bit
       - No parity
       - Hardware flow control disabled (RTS and CTS signals)
       - Receive and transmit enabled
     */

    USART_InitStructure.USART_Parity         = USART_Parity_No;

    USART_InitStructure.USART_BaudRate       = s_USART_Port[serial_port].usart_baud;
    USART_InitStructure.USART_WordLength     = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits       = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode           = USART_Mode_Rx | USART_Mode_Tx;

    /* Setting interrupt for USART */
    NVIC_InitStructure.NVIC_IRQChannel       = s_USART_Port[serial_port].usart_irq_num;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd    = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Set up alt function */
    GPIO_PinAFConfig(
          s_USART_Port[serial_port].tx_port,
          s_USART_Port[serial_port].tx_af_pin_source,
          s_USART_Port[serial_port].tx_af
    );

    GPIO_PinAFConfig(
          s_USART_Port[serial_port].rx_port,
          s_USART_Port[serial_port].rx_af_pin_source,
          s_USART_Port[serial_port].rx_af
    );

    /* Configure basic alt function structure*/
    GPIO_InitStructure.GPIO_Mode             = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType            = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd             = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed            = GPIO_Speed_50MHz;

    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Pin              = s_USART_Port[serial_port].tx_pin;
    GPIO_Init(s_USART_Port[serial_port].tx_port, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Pin              = s_USART_Port[serial_port].rx_pin;
    GPIO_Init(s_USART_Port[serial_port].rx_port, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(s_USART_Port[serial_port].usart, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(s_USART_Port[serial_port].usart, ENABLE);

    /* Enable USART interrupts */
    USART_ITConfig(s_USART_Port[serial_port].usart, USART_IT_RXNE, ENABLE);
}

/******************************************************************************/
uint32_t Serial_send_base64_enc_msg( USART_Port serial_port, char *message, uint16_t len )
{
   char encoded_serial_buf[MAX_MSG_LEN];
   int encoded_sz = base64_encode( message, len, encoded_serial_buf, MAX_MSG_LEN );

   if(encoded_sz < 1)
   {
      debug_printf("Encoding failed\n");
      return ERR_SERIAL_MSG_BASE64_ENC_FAILED;
   }

   /* Return the result of the raw message send function */
   return( Serial_send_raw_msg( serial_port, encoded_serial_buf, encoded_sz ) );
}

/******************************************************************************/
uint32_t Serial_send_raw_msg( USART_Port serial_port, char *message, uint16_t len )
{
   if ( len >= MAX_MSG_LEN )
   {
      return ERR_SERIAL_MSG_TOO_LONG;
   }

   /* Assumes nothing about the size of the message, send it all and let the
    * receiver interpret it. It is up to the sender and receiver to agree on a
    * terminating character.  In most cases, base64 encoded messages will be
    * sent and they use a newline to terminate.  In some cases (scanner), this
    * will not be the case */
   for ( uint16_t i = 0; i < len; i++ )
   {
      uint32_t timeout = SERIAL_LONG_TIMEOUT;   // every byte starts a new timeout count

      while( RESET == USART_GetFlagStatus( s_USART_Port[ serial_port ].usart, USART_FLAG_TXE ) )
      {
         if( (timeout--) <= 0 )
         {
            debug_printf("!!! - Hardware not responding while trying to send a serial msg\n");
            return ERR_SERIAL_HW_TIMEOUT;
         }
      }
      USART_SendData( s_USART_Port[ serial_port ].usart, message[i] );
   }

   return ERR_NONE;
}

/**
 * @brief   Receives a message from through the serial port, one character at a
 *          time. The last character received is either the '\n' or '\r'
 *          character. The message gets stored in a buffer until the end-of-line
 *          character is received.
 * @param   None
 * @retval  None
 */
void UART4_IRQHandler(void) __attribute__((__interrupt__));
void UART4_IRQHandler(void)
{
    QK_ISR_ENTRY();                        /* inform QK about entering an ISR */
    uint8_t data;

    while (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        data = (uint8_t)USART_ReceiveData(UART4);

        // A line feed signals the end of the data stream (message)
        if (data != '\n')
        {
            s_USART_Port[SYSTEM_SERIAL].buffer[ s_USART_Port[SYSTEM_SERIAL].index++ ] = data;
        }
        else
        {
            s_USART_Port[SYSTEM_SERIAL].buffer[ s_USART_Port[SYSTEM_SERIAL].index++ ] = data;
            /* 1. Construct a new msg event indicating that a msg has been received */
            MsgEvt *msgEvt = Q_NEW( MsgEvt, MSG_RECEIVED_SIG );

            /* 2. Fill the msg payload with payload (the actual received msg)*/
            MEMCPY( msgEvt->msg, s_USART_Port[SYSTEM_SERIAL].buffer, s_USART_Port[SYSTEM_SERIAL].index );
            msgEvt->msg_len = s_USART_Port[SYSTEM_SERIAL].index;
            msgEvt->msg_src = SERIAL;

            /* 3. Publish the newly created event to current AO */
            QF_PUBLISH( (QEvent *)msgEvt, AO_CommStackMgr );

            // Reset the serial receive buffer
            s_USART_Port[SYSTEM_SERIAL].index = 0;
        }

        //This check serves as the timeout for the While loop and also protects serial_index from running over
        if ( s_USART_Port[SYSTEM_SERIAL].index >= MAX_MSG_LEN )
        {
           debug_printf("Attempting to send a serial msg over %d bytes which will overrun the buffer.", MAX_MSG_LEN);
           assert( s_USART_Port[SYSTEM_SERIAL].index >= MAX_MSG_LEN );
        }
    }
    QK_ISR_EXIT();                        /* inform QK about exiting an ISR */
}
