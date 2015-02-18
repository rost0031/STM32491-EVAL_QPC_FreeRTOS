/**
 * @file		cplr.h
 * @brief   Main coupler thread/task
 *
 * @date 	Feb 13, 2015
 * @author	Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard.  All rights reserved. 
 *
 * @addtogroup groupCoupler
 * @{
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CPLR_H_
#define CPLR_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "qequeue.h"                             /* For QE "raw" event queues */

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
 extern QEQueue CPLR_evtQueue; /**< Global raw queue to talk between FreeRTOS and QP */

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 *
 */
void CPLR_Task( void* pvParameters );

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupCoupler
 */
#endif                                                             /* CPLR_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
