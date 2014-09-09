/**
 * @file    console_output.c
 * @brief   Serial output to the UART.
 *
 * This file contains the definitions for debug and output functions over the
 * serial DMA console and over regular serial blocking.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupConOut
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "console_output.h"
#include "qp_port.h"                                        /* for QP support */
#include "CBSignals.h"
#include "CBErrors.h"
#include "Shared.h"                                   /*  Common Declarations */
#include "time.h"
#include "SerialMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_SERIAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void CON_output(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   time_T time = TIME_getTime();

   /* 2. Construct a new msg event pointer and allocate storage in the QP event
    * pool */
   SerialDataEvt *serDataEvt = Q_NEW(SerialDataEvt, UART_DMA_START_SIG);
   serDataEvt->wBufferLen = 0;

   /* 3. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case DBG:
         serDataEvt->wBufferLen += snprintf(
               &serDataEvt->buffer[serDataEvt->wBufferLen],
               MAX_MSG_LEN,
               "DBG-%02d:%02d:%02d:%03d-%s():%d:",
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
               "LOG-%02d:%02d:%02d:%03d-%s():%d:",
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
               "WRN-%02d:%02d:%02d:%03d-%s():%d:",
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
               "ERR-%02d:%02d:%02d:%03d-%s():%d:",
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
}

/******************************************************************************/
void CON_slow_output(
      DBG_LEVEL_T dbgLvl,
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
               "DBG-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
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
               "LOG-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
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
               "WRN-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
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
               "ERR-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
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
               "D-ISR!-%02d:%02d:%02d:%03d-:%d:",
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
 * @} end group groupConOut
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
