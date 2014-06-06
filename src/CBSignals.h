/**
 * @file 	CBSignals.h
 * Contains all the signals used by the state machines
 *
 * @date   	06/03/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

#ifndef CBSIGNALS_H_
#define CBSIGNALS_H_

/**
 * Q_USER_SIG is the first signal that user applications are allowed to use.
 * Should always be at the very top of the very first enum list
 */
enum FirstSignal {
   FIRST_SIG = Q_USER_SIG
};

/* INSERT NEW SIGNAL CATEGORIES AFTER HERE */
enum CommStackSignals {
   MSG_SEND_OUT_SIG = FIRST_SIG,
   MSG_RECEIVED_SIG,
   TIME_TEST_SIG,
   MSG_MAX_SIG,
};

enum SerialMgrSignals {
   UART_DMA_START_SIG = MSG_MAX_SIG,
   UART_DMA_DONE_SIG,
   UART_DMA_TIMEOUT_SIG,
   UART_DMA_MAX_SIG
};

/* INSERT NEW SIGNAL CATEGORIES BEFORE HERE...POINT MAX_SHARED_SIG TO LAST SIGNAL */
enum FinalSignal {
   MAX_SHARED_SIG = UART_DMA_MAX_SIG,  /*Last published shared signal - should always be at the bottom of this list */
};

#endif                                                        /* CBSIGNALS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
