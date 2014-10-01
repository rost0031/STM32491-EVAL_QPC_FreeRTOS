/**
 * @file 	CBSignals.h
 * @brief   Contains all the signals used by the state machines
 *
 * @date   	06/03/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBSIGNALS_H_
#define CBSIGNALS_H_

/* Includes ------------------------------------------------------------------*/
#include "qep_port.h"
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @enum First signal
 * Q_USER_SIG is the first signal that user applications are allowed to use.
 * Should always be at the very top of the very first enum list
 */
enum FirstSignal {
   FIRST_SIG = Q_USER_SIG
};

/**
 * @enum Signals used by CommStackMgr
 */
enum CommStackSignals {
   MSG_SEND_OUT_SIG = FIRST_SIG, /** This signal must start at the previous category max signal */
   MSG_RECEIVED_SIG,
   TIME_TEST_SIG,
   MSG_MAX_SIG,
};

/**
 * @enum Signals used by SerialMgr
 */
enum SerialMgrSignals {
   UART_DMA_START_SIG = MSG_MAX_SIG, /** This signal must start at the previous category max signal */
   UART_DMA_DONE_SIG,
   UART_DMA_TIMEOUT_SIG,
   UART_DMA_MAX_SIG
};

/**
 * @enum Signals used by I2CMgr
 */
enum I2CMgrSignals {
   I2C_READ_START_SIG = UART_DMA_MAX_SIG, /** This signal must start at the previous category max signal */
   I2C_READ_DONE_SIG,
   I2C_WRITE_START_SIG,
   I2C_WRITE_DONE_SIG,
   I2C_TIMEOUT_SIG,
   I2C_CHECK_EV_SIG,
   I2C_DMA_TIMEOUT_SIG,
   I2C_EV_MASTER_MODE_SELECT_SIG,
   I2C_EV_MASTER_TX_MODE_SELECTED_SIG,
   I2C_SENT_MSB_ADDR_SIG,
   I2C_SENT_LSB_ADDR_SIG,
   I2C_RECOVERY_TIMEOUT_SIG,
   I2C_MAX_SIG
};

/**
 * @enum Signals used by MenuMgr
 */
enum MenuMgrSignals {
   MENU_GENERAL_REQ_SIG = I2C_MAX_SIG, /** This signal must start at the previous category max signal */


   MENU_MAX_SIG
};
/* INSERT NEW SIGNAL CATEGORIES BEFORE HERE...POINT MAX_SHARED_SIG TO LAST SIGNAL */

/**
 * @enum Final signal.
 * @note MAX_SHARED_SIG should be updated to point to the very last signal
 * before it.
 */
enum FinalSignal {
   MAX_SHARED_SIG = MENU_MAX_SIG,  /**< Last published shared signal - should always be at the bottom of this list */
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */

#endif                                                        /* CBSIGNALS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
