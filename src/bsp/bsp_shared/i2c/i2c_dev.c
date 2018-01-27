/**
 * @file 	i2c_dev.c
 * @brief   Data for devices on the I2C Busses.
 *
 * @date   	10/28/2014
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c_defs.h"
#include "i2c.h"
#include "i2c_dev.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */
#include "I2C1DevMgr.h"                    /* For access to the I2C1DevMgr AO */
#include "cplr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C_DEV ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief An internal structure that holds settings for I2C devices on all I2C
 * busses.
 */
I2C_DevSettings_t s_I2C_Dev[MAX_I2C_DEV] =
{
      {
            /* "External" device settings */
            EEPROM,                    /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xA0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x00,                      /**< i2c_mem_addr */
            0x00,                      /**< i2c_mem_min_addr */
            0xFF,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE,          /**< i2c_mem_page_size */
      },
      {
            /* "External" device settings */
            SN_ROM,                    /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x80,                      /**< i2c_mem_addr */
            0x80,                      /**< i2c_mem_min_addr */
            0x8F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE,          /**< i2c_mem_page_size */
      },
      {
            /* "External" device settings */
            EUI_ROM,                   /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x98,                      /**< i2c_mem_addr */
            0x98,                      /**< i2c_mem_min_addr */
            0x9F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE / 2,      /**< i2c_mem_page_size */
      },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
uint8_t I2C_getDevAddrSize( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr_size );
}

/******************************************************************************/
uint16_t I2C_getDevAddr( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr );
}

/******************************************************************************/
uint8_t I2C_getMemAddrSize( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr_size );
}

/******************************************************************************/
uint16_t I2C_getMemAddr( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr );
}

/******************************************************************************/
void I2C_setI2CMemAddr( I2C_Dev_t iDev, uint16_t addr )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   s_I2C_Dev[iDev].i2c_mem_addr = 0x00FF & addr;
}

/******************************************************************************/
uint16_t I2C_getMinMemAddr( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_min_addr );
}

/******************************************************************************/
uint16_t I2C_getMaxMemAddr( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_max_addr );
}

/******************************************************************************/
uint8_t I2C_getPageSize( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_page_size );
}

/******************************************************************************/
I2C_Bus_t I2C_getBus( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_bus );
}

/******************************************************************************/
char* I2C_devToStr( I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   switch ( iDev ) {
      case EEPROM: return("EEPROM"); break;
      case SN_ROM: return("SN_ROM"); break;
      case EUI_ROM: return("EUI_ROM"); break;
      default: return(""); break;
   }
}

/******************************************************************************/
CBErrorCode I2C_calcPageWriteSizes(
      uint8_t *writeSizeFirstPage,
      uint8_t *writeSizeLastPage,
      uint8_t *writeTotalPages,
      uint16_t i2cMemAddr,
      uint16_t bytesToWrite,
      uint16_t pageSize
)
{
   /* Check if we are going to write over the end of the eeprom */
   if (i2cMemAddr + bytesToWrite > I2C_getMaxMemAddr( EEPROM )) {
      return ( ERR_I2C1DEV_MEM_OUT_BOUNDS );
   }

   /* Calculate space available in the first page */
   uint8_t pageSpace = (((i2cMemAddr/(pageSize)) + 1) * (pageSize)) - i2cMemAddr;

   /* Calculate how many bytes to write in the first page */
   *writeSizeFirstPage = MIN(pageSpace, bytesToWrite);

   /* Calculate how many bytes to write on the last page*/
   *writeSizeLastPage = (bytesToWrite - *writeSizeFirstPage) % pageSize;

   /* Calculate how many pages we are going to be writing (full and partial) */
   *writeTotalPages = 0;
   if (bytesToWrite > *writeSizeFirstPage) {
      *writeTotalPages = ((bytesToWrite - *writeSizeFirstPage)/(pageSize)) + 2;
   } else {
      *writeTotalPages = 1;
   }
   return( ERR_NONE );
}

/******************************************************************************/
CBErrorCode I2C_readDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesRead,
      uint16_t nBytesToRead
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   /* Check buffer sizes */
   if( nBytesToRead > nBufferSize ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   /* Issue a non-blocking call to read I2C */
   status = I2C_readDevMemEVT(
         iDev,                                        // I2C_Dev_t iDev,
         offset,                                      // uint16_t offset,
         nBytesToRead,                                // uint16_t bytesToRead,
         ACCESS_FREERTOS,                             // AccessType_t accType,
         (QActive *)NULL                              // QActive* callingAO
   );

   if( ERR_NONE != status ) {
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   /* Suspend the task.  Once something has been put into the queue, the AO that
    * put it there, will wake up this task. */
   DBG_printf("Suspending Task\n");
   vTaskSuspend( xHandle_CPLR );

   DBG_printf("Task resumed\n");
   /* The task has been awakened by the I2CXDevMgr AO because it has put an
    * event into the raw queue */
   QEvt const *evtI2CDone = QEQueue_get(&CPLR_evtQueue);
   if (evtI2CDone != (QEvt *)0 ) {
      LOG_printf("Found expected event in queue\n");
      switch( evtI2CDone->sig ) {
         case I2C1_DEV_READ_DONE_SIG:
            DBG_printf("Got I2C1_DEV_READ_DONE_SIG\n");
            *pBytesRead = ((I2CReadDoneEvt *) evtI2CDone)->bytes;
            status = ((I2CReadDoneEvt *) evtI2CDone)->status;
            MEMCPY(pBuffer,
                  ((I2CReadDoneEvt *) evtI2CDone)->dataBuf,
                  *pBytesRead
            );

            break;
         default:
            WRN_printf("Unknown signal %d\n", evtI2CDone->sig);
            break;
      }
      QF_gc(evtI2CDone);         /* Don't forget to garbage collect the event */
   } else {
      WRN_printf("Expected event not found in queue\n");
   }

I2C_readDevMemFRT_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         ACCESS_FREERTOS,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesWritten,
      uint16_t nBytesToWrite
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   /* Check buffer sizes */
   if( nBytesToWrite > nBufferSize ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   /* Issue a non-blocking call to read I2C */
   status = I2C_writeDevMemEVT(
         iDev,                                        // I2C_Dev_t iDev,
         offset,                                      // uint16_t offset,
         nBytesToWrite,                               // uint16_t bytesToRead,
         ACCESS_FREERTOS,                             // AccessType_t accType,
         (QActive *)NULL,                             // QActive* callingAO
         pBuffer                                      // uint8_t* pBuffer
   );

   if( ERR_NONE != status ) {
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   /* Suspend the task.  Once something has been put into the queue, the AO that
    * put it there, will wake up this task. */
   DBG_printf("Suspending Task\n");
   vTaskSuspend( xHandle_CPLR );
   DBG_printf("Task resumed\n");

   /* The task has been awakened by the I2CXDevMgr AO because it has put an
    * event into the raw queue */
   QEvt const *evtI2CDone = QEQueue_get(&CPLR_evtQueue);
   if (evtI2CDone != (QEvt *)0 ) {
      LOG_printf("Found expected event in queue\n");
      switch( evtI2CDone->sig ) {
         case I2C1_DEV_WRITE_DONE_SIG:
            DBG_printf("Got I2C1_DEV_WRITE_DONE_SIG\n");
            *pBytesWritten = ((I2CWriteDoneEvt *) evtI2CDone)->bytes;
            status = ((I2CWriteDoneEvt *) evtI2CDone)->status;
            break;
         default:
            WRN_printf("Unknown signal %d\n", evtI2CDone->sig);
            break;
      }
      QF_gc(evtI2CDone);         /* Don't forget to garbage collect the event */
   } else {
      WRN_printf("Expected event not found in queue\n");
   }

I2C_writeDevMemFRT_ERR_HANDLER:   /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         ACCESS_FREERTOS,
         "Error 0x%08x writing I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_readDevMemEVT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToRead,
      AccessType_t accType,
      QActive* callingAO
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   QSignal sig = 0;               /* Signal which will be used in the event to
                                     be posted. This may change based on which
                                     AO will be posted to depending on which I2C
                                     device was passed in */
   QActive* aoToPostTo = 0;       /* AO to which to directly post the even. This
                                     will change depending on which I2C device
                                     was passed in */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   switch( iDev ) {
      case EEPROM:                              /* Intentionally fall through */
      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToRead > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_readDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         sig = I2C1_DEV_RAW_MEM_READ_SIG;
         aoToPostTo = AO_I2C1DevMgr;
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_readDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   /* Create the event and directly post it to the right AO. */
   I2CReadReqEvt *i2cReadReqEvt  = Q_NEW(I2CReadReqEvt, sig);
   i2cReadReqEvt->i2cDev         = iDev;
   i2cReadReqEvt->addr           = I2C_getMemAddr( iDev ) + offset;
   i2cReadReqEvt->bytes          = bytesToRead;
   i2cReadReqEvt->accessType     = accType;
   QACTIVE_POST(aoToPostTo, (QEvt *)(i2cReadReqEvt), callingAO);


I2C_readDevMemEVT_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeDevMemEVT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToWrite,
      AccessType_t accType,
      QActive* callingAO,
      uint8_t *pBuffer
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   QSignal sig = 0;               /* Signal which will be used in the event to
                                     be posted. This may change based on which
                                     AO will be posted to depending on which I2C
                                     device was passed in */
   QActive* aoToPostTo = 0;       /* AO to which to directly post the even. This
                                     will change depending on which I2C device
                                     was passed in */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   switch( iDev ) {
      case EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToWrite > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         sig = I2C1_DEV_RAW_MEM_WRITE_SIG;
         aoToPostTo = AO_I2C1DevMgr;
         break;

      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         status = ERR_I2C_DEV_IS_READ_ONLY;
         goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   /* Create the event and directly post it to the right AO. */
   I2CWriteReqEvt *i2cWriteReqEvt   = Q_NEW(I2CWriteReqEvt, sig);
   i2cWriteReqEvt->i2cDev           = iDev;
   i2cWriteReqEvt->addr             = I2C_getMemAddr( iDev ) + offset;
   i2cWriteReqEvt->bytes            = bytesToWrite;
   i2cWriteReqEvt->accessType       = accType;
   MEMCPY(
         i2cWriteReqEvt->dataBuf,
         pBuffer,
         i2cWriteReqEvt->bytes
   );
   QACTIVE_POST(aoToPostTo, (QEvt *)(i2cWriteReqEvt), callingAO);


I2C_writeDevMemEVT_ERR_HANDLER:   /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x writing I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
/* Blocking Functions - Don't call unless before threads/AOs started or after */
/* they crashed                                                               */
/******************************************************************************/

/******************************************************************************/
CBErrorCode I2C_readDevMemBLK(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToRead,
      AccessType_t accType,
      uint8_t* pBuffer,
      uint8_t  bufSize
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   if ( bufSize < bytesToRead ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   switch( iDev ) {
      case EEPROM:                              /* Intentionally fall through */
      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToRead > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   status = I2C_readBufferBLK(
         I2C_getBus( iDev),                 // I2C_Bus_t iBus,
         I2C_getDevAddr( iDev ),            // uint8_t i2cDevAddr,
         I2C_getMemAddr( iDev ) + offset,   // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( iDev ),        // uint8_t i2cMemAddrSize,
         pBuffer,                           // uint8_t* pBuffer,
         bytesToRead                        // uint16_t bytesToRead
   );

I2C_readDevMemBLK_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeDevMemBLK(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint16_t bytesToWrite,
      AccessType_t accType,
      uint8_t* pBuffer,
      uint8_t  bufSize
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   if ( bufSize < bytesToWrite ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_writeDevMemBLK_ERR_HANDLER; /* Stop and jump to error handling */
   }

   switch( iDev ) {
      case EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToWrite > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         break;
      case EUI_ROM:                             /* Intentionally fall through */
      case SN_ROM:
         status = ERR_I2C_DEV_IS_READ_ONLY;
         goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   status = I2C_writeBufferBLK(
         I2C_getBus( iDev),                     // I2C_Bus_t iBus,
         I2C_getDevAddr( iDev ),                // uint8_t i2cDevAddr,
         I2C_getMemAddr( iDev ) + offset,       // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( iDev ),            // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToWrite,                          // uint16_t bytesToWrite
         I2C_getPageSize( iDev )                // uint16_t pageSize
   );

I2C_writeDevMemBLK_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x writing I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/

