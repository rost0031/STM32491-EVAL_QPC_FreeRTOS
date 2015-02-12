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

void MENU_printf(
      MsgSrc dst,
      char *fmt,
      ...
)
{
   LrgDataEvt *lrgDataEvt = Q_NEW(LrgDataEvt, DBG_MENU_SIG);
   lrgDataEvt->dataLen = 0;
   lrgDataEvt->src = dst;
   lrgDataEvt->dst = dst;

   DBG_printf("src:%d,dst:%d\n",lrgDataEvt->src, lrgDataEvt->dst);

   /* 4. Pass the va args list to get output to a buffer */
   va_list args;
   va_start(args, fmt);

   /* 5. Print the actual user supplied data to the buffer and set the length */
   lrgDataEvt->dataLen += vsnprintf(
         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
         MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 6. Publish the event*/
   QF_PUBLISH((QEvent *)lrgDataEvt, 0);
}

/******************************************************************************/
void CON_output(
      DBG_LEVEL_T dbgLvl,
      MsgSrc src,
      MsgSrc dst,
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
    * pool.  Allocate with margin so we can fall back on regular slow printfs if
    * there's a problem */
   LrgDataEvt *lrgDataEvt = NULL;
   if ( CON == dbgLvl ) {
      lrgDataEvt = Q_NEW(LrgDataEvt, DBG_MENU_SIG);
      DBG_printf("d:%d s:%d\n", dst, src);
   } else {
      lrgDataEvt = Q_NEW(LrgDataEvt, DBG_LOG_SIG);
   }

   lrgDataEvt->dataLen = 0;
   lrgDataEvt->src = src;
   lrgDataEvt->dst = dst;

   /* 3. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case DBG:
         lrgDataEvt->dataLen += snprintf(
               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
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
         lrgDataEvt->dataLen += snprintf(
               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
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
         lrgDataEvt->dataLen += snprintf(
               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
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
         lrgDataEvt->dataLen += snprintf(
               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
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
//         lrgDataEvt->super.sig = DBG_MENU_SIG;
         break;
      default:
         break;
   }

   /* 4. Pass the va args list to get output to a buffer */
   va_list args;
   va_start(args, fmt);

   /* 5. Print the actual user supplied data to the buffer and set the length */
   lrgDataEvt->dataLen += vsnprintf(
         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
         MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 6. Publish the event*/
   QF_PUBLISH((QEvent *)lrgDataEvt, 0);
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

/******************************************************************************/
CBErrorCode CON_hexToStr(
      const uint8_t* hexData,
      uint16_t hexDataLen,
      char* strDataBuffer,
      uint16_t strDataBufferSize,
      uint16_t* strDataLen,
      uint8_t outputNColumns,
      const char sep
)
{
   CBErrorCode status = ERR_NONE;

   if ( NULL == hexData ) {
      ERR_printf("Passed in a NULL buffer\n");
      return( ERR_MEM_NULL_VALUE );
   }

   if ( 0 == hexDataLen ) {
      ERR_printf("Passed in zero length data buffer for input\n");
      return( ERR_MEM_BUFFER_LEN );
   }

   if ( 0 == strDataBufferSize ) {
      ERR_printf("Passed in zero length data buffer for output\n");
      return( ERR_MEM_BUFFER_LEN );
   } else if ( strDataBufferSize < 5 * hexDataLen ) {
      WRN_printf("Output buffer too small to contain all the data.\n");
      WRN_printf("Should be at least %d long\n", 5 * hexDataLen);
      WRN_printf("Continuing but the resulting data will be incomplete\n");
      status = ERR_MEM_BUFFER_LEN;
   }

   /* Index used to keep track of how far into the buffer we've printed */
   *strDataLen = 0;

   for ( uint16_t i = 0; i < hexDataLen; i++ ) {

      /* Let user zero num of columns but if they do, just give them back one
       * long row of data without any linebreaks. */
      if ( 0 != outputNColumns && i % outputNColumns == 0 && i != 0 ) {
         *strDataLen += snprintf(
               &strDataBuffer[*strDataLen],
               strDataBufferSize - *strDataLen,
               "\n"
         );
      }
      *strDataLen += snprintf(
            &strDataBuffer[*strDataLen],
            strDataBufferSize - *strDataLen,
            "0x%02x%c",
            hexData[i],
            sep
      );

      if ( *strDataLen >= strDataBufferSize ) {
         return( ERR_MEM_BUFFER_LEN );
      }
   }
   return( status );

}

/**
 * @} end group groupConOut
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
