/**
 * @file 	i2c_dev.c
 * @brief   Data for devices on the I2C Busses.
 *
 * @date   	10/28/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
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

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C ); /* For debug system to ID this module */

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
CBErrorCode I2C_calcEepromPageWriteSizes(
      uint8_t *pageSize,
      uint8_t *writeSizeFirstPage,
      uint8_t *writeSizeLastPage,
      uint8_t *writeTotalPages,
      uint16_t i2cMemAddr,
      uint16_t bytesToWrite
)
{
   /* Check if we are going to write over the end of the eeprom */
   if (i2cMemAddr + bytesToWrite > I2C_getMaxMemAddr( EEPROM )) {
      return ( ERR_I2C1DEV_MEM_OUT_BOUNDS );
   }

   /* Check if the address and length of write to see if it's more than a page
    * and if the start is page aligned. */
   *pageSize = I2C_getPageSize( EEPROM );

   /* Calculate space available in the first page */
   uint8_t pageSpace = (((i2cMemAddr/(*pageSize)) + 1) * (*pageSize)) - i2cMemAddr;

   /* Calculate how many bytes to write in the first page */
   *writeSizeFirstPage = MIN(pageSpace, bytesToWrite);

   /* Calculate how many bytes to write on the last page*/
   *writeSizeLastPage = (bytesToWrite - *writeSizeFirstPage) % *pageSize;

   /* Calculate how many pages we are going to be writing (full and partial) */
   *writeTotalPages = 0;
   if (bytesToWrite > *writeSizeFirstPage) {
      *writeTotalPages = ((bytesToWrite - *writeSizeFirstPage)/(*pageSize)) + 2;
   } else {
      *writeTotalPages = 1;
   }
   return( ERR_NONE );
}

/******************************************************************************/
/* Blocking Functions - Don't call unless before threads/AOs started or after */
/* they crashed                                                               */
/******************************************************************************/

/******************************************************************************/
CBErrorCode I2C_readUi64RomBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
)
{
   CBErrorCode status = I2C_readBufferBLK(
         I2CBus1,                               // I2C_Bus_t iBus,
         I2C_getDevAddr( EUI_ROM ),         // uint8_t i2cDevAddr,
         I2C_getMemAddr( EUI_ROM ) + offset,// uint16_t i2cMemAddr,
         I2C_getMemAddrSize( EUI_ROM ),     // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToRead                            // uint16_t bytesToRead
   );

   return status;
}

/******************************************************************************/
CBErrorCode I2C_readSnRomBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
)
{
   CBErrorCode status = I2C_readBufferBLK(
         I2CBus1,                               // I2C_Bus_t iBus,
         I2C_getDevAddr( SN_ROM ),          // uint8_t i2cDevAddr,
         I2C_getMemAddr( SN_ROM ) + offset, // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( SN_ROM ),      // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToRead                            // uint16_t bytesToRead
   );

   return status;
}

/******************************************************************************/
CBErrorCode I2C_readEepromBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToRead
)
{
   CBErrorCode status = I2C_readBufferBLK(
         I2CBus1,                               // I2C_Bus_t iBus,
         I2C_getDevAddr( EEPROM ),          // uint8_t i2cDevAddr,
         I2C_getMemAddr( EEPROM ) + offset, // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( EEPROM ),      // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToRead                            // uint16_t bytesToRead
   );

   return status;
}

/******************************************************************************/
CBErrorCode I2C_writeEepromBLK(
      uint8_t* pBuffer,
      uint16_t offset,
      uint16_t bytesToWrite
)
{
   CBErrorCode status = I2C_writeBufferBLK(
         I2CBus1,                               // I2C_Bus_t iBus,
         I2C_getDevAddr( EEPROM ),          // uint8_t i2cDevAddr,
         I2C_getMemAddr( EEPROM ) + offset, // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( EEPROM ),      // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToWrite                           // uint16_t bytesToWrite
   );

   return status;

}
/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/

