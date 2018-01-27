/**
 * @file    comm.c
 * @brief   Parser for various messages that come over any communication busses.
 *
 * @date    09/29/2014
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupComm
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "comm.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_COMM );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
CBErrorCode COMM_parseMsg( uint8_t *pBuffer, uint16_t bufferLen, MsgSrc msgSource )
{
   CBErrorCode status = ERR_NONE; /* Clear initial error and set it if it happens */

   if ( SERIAL_CON == msgSource ) {
      /* The message originated from the serial console.  Based on that
       * information we can assume:
       * A. It's plain text
       * B. It has to be a menu command of some sort
       */
      switch( bufferLen ) {
         case 1:
            if ( pBuffer[0] == '?') {

            } else {
               status = ERR_COMM_UNIMPLEMENTED_MSG;
               WRN_printf(
                     "ErrCode 0x%08x: ERR_COMM_UNIMPLEMENTED_MSG (%c) from source: %d\n",
                     status,
                     pBuffer[0],
                     msgSource
               );
            }

            break;
         case 0:
         default:
            status = ERR_COMM_INVALID_MSG_LEN;
            WRN_printf(
                  "ErrCode 0x%08x: Ignoring message of 0 len from source: %d\n",
                  status,
                  msgSource
            );
            //TODO: error handle better here. maybe write it to the EEPROM?

            break;
      }

   } else {
      status = ERR_COMM_UNKNOWN_MSG_SOURCE;
      WRN_printf(
            "ErrCode 0x%08x: Ignoring message is from an unimplemented source: %d\n",
            status,
            msgSource
      );
      //TODO: error handle better here. maybe write it to the EEPROM?
   }

   return( status );
}

/******************************************************************************/
CBErrorCode COMM_parseMsgOfLen1(
      uint8_t *pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSource
)
{
   CBErrorCode status = ERR_NONE; /* Clear initial error and set it if it happens */

   switch( pBuffer[0] ) {
      case '?':

         break;

      default:
         status = ERR_COMM_UNIMPLEMENTED_MSG;
         WRN_printf(
            "ErrCode 0x%08x: Ignoring unimplemented message %c from source: %d.\n",
            pBuffer[0],
            msgSource
         );
         //TODO: error handle better here. maybe write it to the EEPROM?
         break;
   }

   return( status );
}
/**
 * @}
 * end addtogroup groupComm
 */

/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
