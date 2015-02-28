/**
 * @file 	CBErrors.h
 * @brief   Contains all the error codes that can be generated by the board
 *
 * @date   	05/28/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBERRORS_H_
#define CBERRORS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @enum Error codes that can be generated by this board.
 *
 * The error code consists of a 32 bit value:
 *
 * The most significant 16 bits map to the category.
 * The least significant 16 bits map to low level errors as you see fit.
 *
 * For example:
 * Error code - 0x00000004
 * Category   - 0x0000: This is the serial communications category
 * Error      - 0x0004: This is the low level error code that the board can
 * specify.  Each category gets 0xFFFF error codes (65535) dedicated to it.
 *
 */
typedef enum CBErrors
{
   ERR_NONE                                                    = 0x00000000,

   /* HW error category.                         0x00000001 - 0x0000FFFF */
   ERR_SERIAL_HW_TIMEOUT                                       = 0x00000001,
   ERR_SERIAL_MSG_TOO_LONG                                     = 0x00000002,
   ERR_SERIAL_MSG_BASE64_ENC_FAILED                            = 0x00000003,
   ERR_STM32_HW_CRYPTO_FAILED                                  = 0x00000004,
   ERR_LOW_POWER_RESET                                         = 0x00000005,
   ERR_WINDOW_WATCHDOG_RESET                                   = 0x00000006,
   ERR_INDEPENDENT_WATCHDOG_RESET                              = 0x00000007,
   ERR_SOFTWARE_RESET                                          = 0x00000008,
   ERR_POR_PDR_RESET                                           = 0x00000009,
   ERR_PIN_RESET                                               = 0x0000000A,
   ERR_BOR_RESET                                               = 0x0000000B,

   /* Memory error category                      0x00010000 - 0x0001FFFF */
   ERR_MEM_NULL_VALUE                                          = 0x00010000,
   ERR_MEM_BUFFER_LEN                                          = 0x00010001,

   /* NOR error category                         0x00030000 - 0x0003FFFF */
   ERR_NOR_ERROR                                               = 0x00030000,
   ERR_NOR_TIMEOUT                                             = 0x00030001,
   ERR_NOR_BUSY                                                = 0x00030002,

   /* COMM error category                        0x00040000 - 0x0004FFFF */
   ERR_COMM_UNKNOWN_MSG_SOURCE                                 = 0x00040000,
   ERR_COMM_INVALID_MSG_LEN                                    = 0x00040001,
   ERR_COMM_UNIMPLEMENTED_MSG                                  = 0x00040002,

   /* MENU error category                        0x00050000 - 0x0005FFFF */
   ERR_MENU_NODE_STORAGE_ALLOC_NULL                            = 0x00050000,
   ERR_MENU_TEXT_STORAGE_ALLOC_NULL                            = 0x00050001,
   ERR_MENU_UNKNOWN_CMD                                        = 0x00050002,
   ERR_MENU_CURRENT_NODE_NULL                                  = 0x00050003,
   ERR_MENU_CMD_NOT_FOUND_AT_THIS_MENU                         = 0x00050004,

   /* I2CBus error category                      0x00060000 - 0x0006FFFF */
   ERR_I2CBUS_BUSY                                             = 0x00060000,
   ERR_I2CBUS_RCVRY_SDA_STUCK_LOW                              = 0x00060001,
   ERR_I2CBUS_RCVRY_EV5_NOT_REC                                = 0x00060002,
   ERR_I2CBUS_RCVRY_EV6_NOT_REC                                = 0x00060003,
   ERR_I2CBUS_EV5_TIMEOUT                                      = 0x00060004,
   ERR_I2CBUS_EV5_NOT_REC                                      = 0x00060005,
   ERR_I2CBUS_INVALID_PARAMS_FOR_7BIT_ADDR                     = 0x00060006,
   ERR_I2CBUS_INVALID_PARAMS_FOR_SEND_DATA                     = 0x00060007,
   ERR_I2CBUS_EV6_TIMEOUT                                      = 0x00060008,
   ERR_I2CBUS_EV6_NOT_REC                                      = 0x00060009,
   ERR_I2CBUS_EV8_TIMEOUT                                      = 0x0006000A,
   ERR_I2CBUS_EV8_NOT_REC                                      = 0x0006000B,
   ERR_I2CBUS_INVALID_PARAMS_FOR_BUS_CHECK_FREE                = 0x0006000C,
   ERR_I2CBUS_RXNE_FLAG_TIMEOUT                                = 0x0006000D,
   ERR_I2CBUS_STOP_BIT_TIMEOUT                                 = 0x0006000E,
   ERR_I2CBUS_WRITE_BYTE_TIMEOUT                               = 0x0006000F,

   /* I2C1Dev error category                     0x00070000 - 0x0007FFFF */
   ERR_I2C1DEV_CHECK_BUS_TIMEOUT                               = 0x00070000,
   ERR_I2C1DEV_EV5_TIMEOUT                                     = 0x00070001,
   ERR_I2C1DEV_EV6_TIMEOUT                                     = 0x00070002,
   ERR_I2C1DEV_EV8_TIMEOUT                                     = 0x00070003,
   ERR_I2C1DEV_READ_MEM_TIMEOUT                                = 0x00070004,
   ERR_I2C1DEV_WRITE_MEM_TIMEOUT                               = 0x00070005,
   ERR_I2C1DEV_READ_REG_TIMEOUT                                = 0x00070006,
   ERR_I2C1DEV_WRITE_REG_TIMEOUT                               = 0x00070007,
   ERR_I2C1DEV_ACK_DIS_TIMEOUT                                 = 0x00070008,
   ERR_I2C1DEV_ACK_EN_TIMEOUT                                  = 0x00070009,
   ERR_I2C1DEV_MEM_OUT_BOUNDS                                  = 0x0007000A,

   /* Settings Database error category           0x00080000 - 0x0008FFFF */
   ERR_DB_NOT_INIT                                             = 0x00080000,
   ERR_DB_VER_MISMATCH                                         = 0x00080001,

   /* Reserved errors                            0xFFFFFFFE - 0xFFFFFFFF */
   ERR_UNIMPLEMENTED                                           = 0xFFFFFFFE,
   ERR_UNKNOWN                                                 = 0xFFFFFFFF
} CBErrorCode;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */
#endif                                                         /* CBERRORS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
