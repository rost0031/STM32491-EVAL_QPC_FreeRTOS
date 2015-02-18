/**
 * @file		cplr.c
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

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "dbg_out_cntrl.h"
#include "cplr.h"
#include "LWIPMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_CPLR );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
QEQueue CPLR_evtQueue;         /**< raw queue to talk between FreeRTOS and QP */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void CPLR_Task( void* pvParameters )
{
   (void) pvParameters;

   for (;;) {
      /* Check if there's data in the queue and process it if there. */
      QEvt const *ethEvt;
      ethEvt = QEQueue_get(&CPLR_evtQueue);
      if ( ethEvt != (QEvt *)0 ) {
         DBG_printf("Received EthEvt of len: %d\n", ((EthEvt const *)ethEvt)->msg_len);
         QF_gc(ethEvt); /* !!! Don't forget to garbage collect the event after processing */
      }
      vTaskDelay(1);
   }
}

/**
 * @}
 * end addtogroup groupCoupler
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
