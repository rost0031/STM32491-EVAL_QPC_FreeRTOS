/**
 * @file    console_output.c
 * @brief   Non blocking, DMA output over UART.
 *
 * This file contains the definitions for debug and output functions over the
 * serial DMA console.
 *
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSerial
 * @{
 */
#define QP_IMPL           /* this is QP implementation */
/* Includes ------------------------------------------------------------------*/
#include "console_output.h"
#include "qp_port.h"                                        /* for QP support */
#include "CBSignals.h"
#include "CBErrors.h"
#include "Shared.h"                                   /*  Common Declarations */
#include "time.h"
#include "SerialMgr.h"
#include "qk.h"
#include "qf_pkg.h"                             /* For crit entry/exit macros */


/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void CON_output(
      DEBUG_LEVEL_T dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
)
{

   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   time_T time = TIME_getTime();

//   QF_CRIT_STAT_
//   QF_CRIT_ENTRY_();                                /* Start critical section */
   /* 2. Construct a new msg event pointer and allocate storage in the QP event
    * pool */
   SerialDataEvt *serDataEvt = Q_NEW(SerialDataEvt, UART_DMA_START_SIG);
   serDataEvt->wBufferLen = 0;
//   sprintf(serDataEvt->buffer, "test\n");
//   QF_PUBLISH((QEvent *)serDataEvt, 0);

   /* 3. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case DBG:
         serDataEvt->wBufferLen += snprintf(
               &serDataEvt->buffer[serDataEvt->wBufferLen],
               MAX_MSG_LEN,
               "DBG-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case LOG:
         serDataEvt->wBufferLen += snprintf(
               &serDataEvt->buffer[serDataEvt->wBufferLen],
               MAX_MSG_LEN,
               "LOG-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case WRN:
         serDataEvt->wBufferLen += snprintf(
               &serDataEvt->buffer[serDataEvt->wBufferLen],
               MAX_MSG_LEN,
               "WRN-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case ERR:
         serDataEvt->wBufferLen += snprintf(
               &serDataEvt->buffer[serDataEvt->wBufferLen],
               MAX_MSG_LEN,
               "ERR-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case CON: // This is used to print menu so don't prepend anything
      default:
         break;
   }

   /* 4. Pass the va args list to get output to a buffer */
   va_list args;
   va_start(args, fmt);

   /* 5. Print the actual user supplied data to the buffer and set the length */
   serDataEvt->wBufferLen += vsnprintf(
         &serDataEvt->buffer[serDataEvt->wBufferLen],
         MAX_MSG_LEN - serDataEvt->wBufferLen, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 6. Publish the event*/
   QF_PUBLISH((QEvent *)serDataEvt, 0);
//   QF_CRIT_EXIT_();                                   /* End critical section */
}

/******************************************************************************/
void CON_slow_output(
      DEBUG_LEVEL_T dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   time_T time = TIME_getTime();

   /* Temporary local buffer and index to compose the msg */
   char tmpBuffer[MAX_MSG_LEN];
   uint8_t tmpBufferIndex = 0;

   /* 2. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case DBG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_LEN,
               "DBG-SLOW!-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case LOG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_LEN,
               "LOG-SLOW!-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case WRN:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_LEN,
               "WRN-SLOW!-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case ERR:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_LEN,
               "ERR-SLOW!-%02d:%02d:%02d:%05d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case ISR:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_LEN,
               "D-ISR!-%02d:%02d:%02d:%05d-:%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               wLineNumber
         );
         break;
      case CON: // This is used to print menu so don't prepend anything
      default:
         break;
   }

   /* 3. Pass the va args list to get output to a buffer, making sure to not
    * overwrite the prepended data. */
   va_list args;
   va_start(args, fmt);

   tmpBufferIndex += vsnprintf(
         &tmpBuffer[tmpBufferIndex],
         MAX_MSG_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 4. Print directly to the console now. THIS IS A SLOW OPERATION! */
   fwrite(tmpBuffer, tmpBufferIndex, 1, stderr);
}

/**
 * @} end addtogroup groupSerial
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
