/**
 * @file   serial.h
 * @brief  Declarations for USART/UART/serial port.
 *
 * @date   05/27/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSerial
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SERIAL_H_
#define SERIAL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "bsp_defs.h"
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * \enum SerialPort_T
 * Serial ports available on the system.
 */
typedef enum SerialPorts {
   SERIAL_SYS  = 0,                           /**< Serial debug/basic IO port */
   /* Insert more serial port enumerations here... */
   SERIAL_MAX     /**< Maximum number of available serial ports on the system */
} SerialPort_T;

/**
 * \struct USART_Settings_t
 * Most of the settings that are needed to set up all the hardware for each
 * serial port.
 */
typedef struct USART_Settings
{
    SerialPort_T        port;                         /**< System serial port */

    USART_TypeDef *     usart;                           /**< STM32 UART port */
    const uint32_t      usart_baud;                       /**< UART Baud rate */
    uint32_t            usart_clk;                            /**< UART clock */
    IRQn_Type           usart_irq_num;                  /**< STM32 IRQ number */
    ISR_Priority        usart_irq_prio;               /**< STM32 IRQ priority */

    GPIO_TypeDef *      tx_port;                            /**< UART TX port */
    const uint16_t      tx_pin;                             /**< UART TX pin  */
    const uint16_t      tx_af_pin_source;       /**< UART TX alt-func pin src */
    const uint16_t      tx_af;                      /**< UART TX alt function */
    uint32_t            tx_gpio_clk;                  /**< UART TX GPIO clock */

    GPIO_TypeDef *      rx_port;                            /**< UART RX port */
    const uint16_t      rx_pin;                              /**< UART RX pin */
    const uint16_t      rx_af_pin_source;       /**< UART RX alt-func pin src */
    const uint16_t      rx_af;                      /**< UART RX alt function */
    uint32_t            rx_gpio_clk;                  /**< UART RX GPIO clock */

    /* Buffer management */
    char                *buffer;             /**< Serial port in data buffer. */
    uint16_t            index;   /**< Serial port in data buffer used length. */
} USART_Settings_t;

/**
 * \struct USART_DMA_Settings_t
 * Most of the settings that are needed to set up all the hardware for each
 * serial port DMA access.
 */
typedef struct USART_DMA_Settings
{
    SerialPort_T        port;              /**< System serial port specifier. */

    IRQn_Type           dma_irq_num;          /**< STM32 serial DMA IRQ number*/
    ISR_Priority        dma_irq_prio;      /**< STM32 serial DMA IRQ priority */
    uint32_t            dma_channel;            /**< STM32 serial DMA channel */
    DMA_Stream_TypeDef* dma_stream;              /**< STM32 serial DMA stream */
    const uint32_t      dma_clk;       /**< STM32 DMA clock for use with uart */

} USART_DMA_Settings_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize specified UART/USART.
 *
 * This function initializes GPIO, UART, and all the associated hardware for
 * a specified serial port.
 *
 * @param [in]  serial_port: Which serial port to initialize
 *    @arg SYSTEM_SERIAL
 * @return: None
 */
void Serial_Init(
      SerialPort_T serial_port
);

/**
 * @brief   Set up and initiate a DMA transfer over specified serial port.
 *
 * This function sets up the DMA and appropriate interrupts for the specified
 * serial port and gets the system to state where the only thing left to do to
 * kick off the transfer with one short and fast command.
 *
 * @param [in] serial_port: Which serial port to initialize
 *    @arg SYSTEM_SERIAL
 * @param [in] *pBuffer: pointer to the buffer with message to send
 * @param [in] wBufferLen: length of the message to send
 * @return: None
 */
void Serial_DMAConfig(
      SerialPort_T serial_port,
      char *pBuffer,
      uint16_t wBufferLen
);

/**
 * @brief   Starts the DMA transfer over serial.
 *
 * This function should only be called after Serial_DMAConfig() has been called.
 * It kicks off the prepared DMA transfer and enables the interrupt that will
 * signify its completion.
 *
 * @param [in] serial_port: Which serial port to initialize DMA transfer with
 *    @arg SYSTEM_SERIAL
 * @return: None
 */
void Serial_DMAStartXfer(
      SerialPort_T serial_port
);

/**
 * @brief   Send a base64 encoded message over serial.
 *
 * Sends a message out through the specified serial port after base64 encoding
 * it, one character at a time. The last character sent is the '\n' character.
 * That is how the receiver knows the message is complete.
 *
 * @param [in]  serial_port: a USART_Port type that specifies how to look up settings
 * in the serial port settings structure
 * @param [in]  *message: pointer to the buffer with message to send
 * @param [in]  len: length of the message to send
 * @retval  uint32_t: error code from error.h
 *    @arg  ERR_NONE:   No errors
 *    @arg  ERR_SERIAL_HW_TIMEOUT:     Serial hardware stopped responding
 *    @arg  ERR_SERIAL_MSG_TOO_LONG:   Message being sent is longer than MAX_MSG_LEN
 *    @arg  ERR_SERIAL_MSG_BASE64_ENC_FAILED: Failed to base64 encode the message, likely due to length
 */
uint32_t Serial_send_base64_enc_msg(
      SerialPort_T serial_port,
      char *message,
      uint16_t len
);

/**
 * @brief   Send a raw message over serial.
 *
 * Sends a message as it is (raw) out through the specified serial port.
 *
 * @param [in]  serial_port: a USART_Port type that specifies how to look up settings
 * in the serial port settings structure
 * @param [in]  *message: pointer to the buffer with message to send
 * @param [in]  len: length of the message to send
 * @retval  uint32_t: error code from error.h
 *    @arg  ERR_NONE:   No errors
 *    @arg  ERR_SERIAL_HW_TIMEOUT:     Serial hardware stopped responding
 *    @arg  ERR_SERIAL_MSG_TOO_LONG:   Message being sent is longer than MAX_MSG_LEN
 */
uint32_t Serial_send_raw_msg(
      SerialPort_T serial_port,
      char *message,
      uint16_t len
);

/**
 * @}
 * end addtogroup groupSerial
 */
#endif                                                           /* SERIAL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
