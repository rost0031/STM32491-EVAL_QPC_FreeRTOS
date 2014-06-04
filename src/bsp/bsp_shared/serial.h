// $Id$
/**
 * @file   serial.h
 * @brief  This file contains the definitions for serial port control.
 *
 * @date   05/27/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */
// $Log$

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stm32f2xx_usart.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_dma.h"
#include "bsp.h"

typedef enum USART_Ports
{
   SYSTEM_SERIAL  = 0,
   MAX_SERIAL
} USART_Port;

/*
 * All settings that are needed to set up (almost) all the hardware for each
 * serial port.
 */
typedef struct USART_Settings
{
    USART_Port                   port;

    /* USART settings */
    USART_TypeDef *              usart;
    const uint32_t               usart_baud;
    uint32_t                     usart_clk;
    IRQn_Type                    usart_irq_num;
    ISR_Priority                 usart_irq_prio;

    GPIO_TypeDef *               tx_port;
    const uint16_t               tx_pin;
    const uint16_t               tx_af_pin_source;
    const uint16_t               tx_af;

    GPIO_TypeDef *               rx_port;
    const uint16_t               rx_pin;
    const uint16_t               rx_af_pin_source;
    const uint16_t               rx_af;

    /* Buffer management */
    char                         *buffer;
    uint16_t                     index;
} USART_Settings_t;

/*
 * All settings that are needed to set up (almost) all the hardware for each
 * serial port.
 */
typedef struct USART_DMA_Settings
{
    USART_Port                   port;

    IRQn_Type                    dma_irq_num;
    ISR_Priority                 dma_irq_prio;
    uint32_t                     dma_channel;
    DMA_Stream_TypeDef           dma_stream;

} USART_DMA_Settings_t;

/*
 * Initializes usarts.
 *
 * @param   serial_port: Which serial port to initialize
 *    @arg SYSTEM_SERIAL
 * @return: None
 */
void Serial_Init(
      USART_Port serial_port
);

/*
 * Sets up and initiates a DMA transfer over specified serial port.
 *
 * @param [in] serial_port: Which serial port to initialize
 *    @arg SYSTEM_SERIAL
 * @param [in] *pBuffer: pointer to the buffer with message to send
 * @param [in] wBufferLen: length of the message to send
 * @return: None
 */
void Serial_DMAConfig(
      USART_Port serial_port,
      char *pBuffer,
      uint16_t wBufferLen
);

/*
 * Starts the DMA transfer over serial
 *
 * @param [in] serial_port: Which serial port to initialize DMA transfer with
 *    @arg SYSTEM_SERIAL
 * @return: None
 */
void Serial_DMAStartXfer(
      USART_Port serial_port
);

/**
 * Sends a message out through the specified serial port after base64 encoding
 * it, one character at a time. The last character sent is the '\n' character.
 * That is how the receiver knows the message is complete.
 *
 * @param   serial_port: a USART_Port type that specifies how to look up settings
 * in the serial port settings structure
 * @param   message: pointer to the buffer with message to send
 * @param   len: length of the message to send
 * @return  uint32_t: error code from error.h
 *    @arg  ERR_NONE:   No errors
 *    @arg  ERR_SERIAL_HW_TIMEOUT:     Serial hardware stopped responding
 *    @arg  ERR_SERIAL_MSG_TOO_LONG:   Message being sent is longer than MAX_MSG_LEN
 *    @arg  ERR_SERIAL_MSG_BASE64_ENC_FAILED: Failed to base64 encode the message, likely due to length
 */
uint32_t Serial_send_base64_enc_msg(
      USART_Port serial_port,
      char *message,
      uint16_t len
);

/**
 * Sends a message as it is (raw) out through the specified serial port.
 *
 * @param   serial_port: a USART_Port type that specifies how to look up settings
 *          in the serial port settings structure
 * @param   message: pointer to the buffer with message to send
 * @param   len: length of the message to send
 * @return  uint32_t: error code from error.h
 *    @arg  ERR_NONE:   No errors
 *    @arg  ERR_SERIAL_HW_TIMEOUT:     Serial hardware stopped responding
 *    @arg  ERR_SERIAL_MSG_TOO_LONG:   Message being sent is longer than MAX_MSG_LEN
 */
uint32_t Serial_send_raw_msg(
      USART_Port serial_port,
      char *message,
      uint16_t len
);

#endif                                                           /* SERIAL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
