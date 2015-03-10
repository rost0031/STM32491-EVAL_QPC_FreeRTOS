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
   QEvt const *evt; /* This pointer to an event always lives but should be
                       garbage collected after finishing to process it so the
                       memory in the pool to which it points can be reused. If
                       this thread needs to wait on another event while still
                       processing this (main) one, a different local pointer
                       should be used and garbage collected after. */

   for (;;) {                         /* Beginning of the thread forever loop */
      /* Check if there's data in the queue and process it if there. */

      evt = QEQueue_get(&CPLR_evtQueue);
      if ( evt != (QEvt *)0 ) { /* Check whether an event is present in queue */

         switch( evt->sig ) {        /* Identify the event by its signal enum */



         }
         /* If any data from the event needs to be used after garbage
          * collection, it should be locally stored if. */

         DBG_printf("Received EthEvt of len: %d\n", ((EthEvt const *)evt)->msg_len);


         QF_gc(evt); /* !!! Don't forget to garbage collect the event after
                        processing the event.  After this, any data to which
                        this pointer points to may not be valid and should not
                        be referenced. */
      }

      vTaskDelay(1); /* Always task-delay so this thread can go to sleep while
                        other threads/AOs can process their data */
   }
}

/**
 * @}
 * end addtogroup groupCoupler
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
