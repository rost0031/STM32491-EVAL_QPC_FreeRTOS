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
I2CBus1_DevSettings_t s_I2CBus1_Dev[MAX_I2C1_DEV] =
{
      {
            /* "External" device settings */
            EEPROM,                    /**< i2c_dev */
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
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x98,                      /**< i2c_mem_addr */
            0x98,                      /**< i2c_mem_min_addr */
            0x9F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE / 2,      /**< i2c_mem_page_size */
      }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
uint8_t I2C_getI2C1DevAddrSize( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_dev_addr_size );
}

/******************************************************************************/
uint16_t I2C_getI2C1DevAddr( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_dev_addr );
}

/******************************************************************************/
uint8_t I2C_getI2C1MemAddrSize( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_mem_addr_size );
}

/******************************************************************************/
uint16_t I2C_getI2C1MemAddr( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_mem_addr );
}

/******************************************************************************/
void I2C_setI2C1MemAddr( I2CBus1_Dev_t iDev, uint16_t addr )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   s_I2CBus1_Dev[iDev].i2c_mem_addr = 0x00FF & addr;
}

/******************************************************************************/
uint16_t I2C_getI2C1MinMemAddr( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_mem_min_addr );
}

/******************************************************************************/
uint16_t I2C_getI2C1MaxMemAddr( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_mem_max_addr );
}

/******************************************************************************/
uint8_t I2C_getI2C1PageSize( I2CBus1_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C1_DEVICE( iDev ) );

   return( s_I2CBus1_Dev[iDev].i2c_mem_page_size );
}
/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
