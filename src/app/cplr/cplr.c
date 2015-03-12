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
#include "i2c_dev.h"                                 /* For I2C functionality */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_CPLR );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
QEQueue CPLR_evtQueue;         /**< raw queue to talk between FreeRTOS and QP */

TaskHandle_t xHandle_CPLR;                       /**< Handle to the CPLR task */
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

   CBErrorCode status = ERR_NONE; /* Keep track of failures of various func
                                     calls and commands.  If this is ever set
                                     to something other than ERR_NONE, it will
                                     be printed out at the end of the for loop*/

   for (;;) {                         /* Beginning of the thread forever loop */
      /* Check if there's data in the queue and process it if there. */

      evt = QEQueue_get(&CPLR_evtQueue);
      if ( evt != (QEvt *)0 ) { /* Check whether an event is present in queue */

         switch( evt->sig ) {        /* Identify the event by its signal enum */
            case CPLR_ETH_SYS_TEST_SIG:
               DBG_printf(
                     "Received CPLR_ETH_SYS_TEST_SIG (%d) signal with event EthEvt of len: %d\n",
                     evt->sig,
                     ((EthEvt const *)evt)->msg_len
               );
#if 0
               /* Going to use this signal to test some stuff in this thread */
               /* Do a read from the EEPROM on the I2C Bus */
               status = I2C_readDevMemEVT(
                     EEPROM,                                         // I2C_Dev_t iDev,
                     0x00,                                           // uint16_t offset,
                     17,                                             // uint16_t bytesToRead,
                     ACCESS_FREERTOS,                                // AccessType_t accType,
                     NULL                                            // QActive* callingAO
               );
               if ( ERR_NONE != status ) {
                  ERR_printf("Error calling I2C_readDevMemEVT()\n");
                  goto CPLR_Task_ERR_HANDLE; /* Stop and jump to error handling */
               }

               /* If we got here, there were no errors during the call to
                * I2C_readDevMemEVT() so we can expect an event from that driver
                */
               uint32_t timeout = 1000;
               QEvt const *evtI2CDone = 0;

               /* We only expect an I2C read done signal and no others */
               do {
                  evtI2CDone = QEQueue_get(&CPLR_evtQueue);
                  if (evtI2CDone != (QEvt *)0 ) {
                     break;
                  } else {
                     vTaskDelay(1);
                  }
               } while ( --timeout != 0 );

               if ( 0 == timeout ) {
                  ERR_printf("Timed out waiting for an event\n");
               } else {
                  switch( evtI2CDone->sig ) {
                     case I2C1_DEV_READ_DONE_SIG:
                        DBG_printf("Got I2C1_DEV_READ_DONE_SIG\n");
                        break;
                     default:
                        WRN_printf("Unknown signal %d\n", evtI2CDone->sig);
                        break;
                  }
                  QF_gc(evt);
               }

#endif
               uint8_t buffer[20];
               uint16_t bytesRead = 0;

               DBG_printf("Issuing I2C_readDevMemFRT()\n");
               status = I2C_readDevMemFRT(
                     EEPROM,                          // I2C_Dev_t iDev,
                     0x00,                            // uint16_t offset,
                     buffer,                          // uint8_t *pBuffer,
                     sizeof(buffer),                  // uint16_t nBufferSize,
                     &bytesRead,                      // uint16_t *pBytesRead,
                     17                               // uint16_t nBytesToRead
               );

               char tmp[120];
               uint16_t tmpLen = 0;
               status = CON_hexToStr(
                     (const uint8_t *)buffer,             // data to convert
                     bytesRead,                           // length of data to convert
                     tmp,                                 // where to write output
                     sizeof(tmp),                         // max size of output buffer
                     &tmpLen,                             // size of the resulting output
                     0,                                   // no columns
                     ' ',                                 // separator
                     true                                 // bPrintX
               );
               DBG_printf("I2C_readDevMemFRT() returned having read %d bytes: %s\n", bytesRead, tmp);
               break;

            default:
               WRN_printf("Received an unknown signal: %d. Ignoring...\n", evt->sig);
               break;

         }
         /* If any data from the event needs to be used after garbage
          * collection, it should be locally stored if. */


CPLR_Task_ERR_HANDLE:             /* Handle any error that may have occurred. */
         /* Print error if exists */
         ERR_COND_OUTPUT(
               status,
               ACCESS_FREERTOS,
               "Error 0x%08x occurred in FreeRTOS thread!!!\n",
               status
         );

         QF_gc(evt); /* !!! Don't forget to garbage collect the event after
                        processing the event.  After this, any data to which
                        this pointer points to may not be valid and should not
                        be referenced. */
      }

//      vTaskSuspend(NULL);
      vTaskDelay(1); /* Always task-delay so this thread can go to sleep while
                        other threads/AOs can process their data */
   }                                        /* End of the thread forever loop */
}

/**
 * @}
 * end addtogroup groupCoupler
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
