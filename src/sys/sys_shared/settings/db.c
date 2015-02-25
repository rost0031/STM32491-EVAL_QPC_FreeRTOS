/**
 * @file		db.c
 * @brief   A simple generic database of settings that worked on any memory type.
 *
 * @date    Feb 24, 2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard.  All rights reserved. 
 *
 * @addtogroup groupSettings
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "dbg_out_cntrl.h"
#include <stddef.h>
#include "i2c_dev.h"                               /* for I2C device mappings */
#include "i2c.h"                                     /* for I2C functionality */
#include "db.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DB );  /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/**
 * DB elements that can be retrieved.
 */
typedef enum {
   DB_EEPROM = 0,           /**< Setting is located in the main EEPROM memory */
   DB_SN_ROM,         /**< Setting is located in the RO SNR section of EEPROM */
   DB_UI_ROM,        /**< Setting is located in the RO UI64 section of EEPROM */
   DB_GPIO,                          /**< Setting is specified via a GPIO pin */
   DB_FLASH                  /**< Setting is located in the main FLASH memory */
} DB_ElemLoc_t;

/**
 * Description type of each DB element (location, etc).
 */
typedef struct {
   DB_Element_t elem;  /**< Specifies which settings element is being described */
   DB_ElemLoc_t  loc;  /**< Specifies how the db element is stored */
   size_t       size;  /**< Specifies the size of the db element */
} SettingsDB_Desc_t;

/**
 * DB element structure mapping that resides in the main memory of the EEPROM
 */
typedef struct {
   uint16_t dbVersion;                           /**< Version of the database */
   uint8_t ipAddr[4];  /**< 4 values of the 111.222.333.444 ip address in hex */
} SettingsDB_t;


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/**< Macro to get size of the element stored in the EEPROM memory */
#define DB_SIZE_OF_ELEM(s,m)     ((size_t) sizeof(((s *)0)->m))

/**< Macro to get the offset of the element stored in the EEPROM memory */
#define DB_LOC_OF_ELEM(s,m)      offsetof(s, m)

/* Private variables and Local objects ---------------------------------------*/

/**< Array to specify where all the DB elements reside */
static const SettingsDB_Desc_t settingsDB[DB_MAX_ELEM] = {
      { DB_VERSION,  DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbVersion)  },
      { DB_MAC_ADDR, DB_UI_ROM,  6                                         },
      { DB_IP_ADDR,  DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, ipAddr)     },
      { DB_SN,       DB_SN_ROM,  16                                        },
};

/* Private function prototypes -----------------------------------------------*/
static size_t DB_getElemSize( DB_Element_t elem );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
CBErrorCode DB_getMacAddrBLK(
      uint8_t* pBuffer,
      size_t bufSize,
      AccessType_t accType
)
{

   /* 1. Check the buffer size and if it's allocated */
   /* Only check buffer size if using blocking access */
   if ( ACCESS_BLOCKING == accType ) {
      if ( NULL == pBuffer ) {
         return( ERR_MEM_NULL_VALUE );
      }

      if ( DB_getElemSize( DB_MAC_ADDR ) > bufSize ) {
         return( ERR_MEM_BUFFER_LEN );
      }
   }

   /* 2. Look up mappings for devices, memory addresses, etc. */
   I2C_Bus_t i2cBus = I2CBus1;
   /* For now, only I2CBus1 exists */
   if ( !IS_I2C1_DEVICE( EUI_ROM ) ) {
      if (ACCESS_BLOCKING == accType ) {
         err_slow_printf("I2C Device %d doesn't exist on I2CBus1\n");
      } else {
         ERR_printf("I2C Device %d doesn't exist on I2CBus1\n");
      }
      return( ERR_UNIMPLEMENTED );
   }

   /* Look up Device and Memory addresses and their sizes */
   uint16_t i2cDevAddr = I2C_getI2C1DevAddr( EUI_ROM );
   uint16_t i2cMemAddr = I2C_getI2C1MemAddr( EUI_ROM );
   uint8_t i2cMemAddrSize = I2C_getI2C1MemAddrSize( EUI_ROM );

   I2C_readBufferBlocking(
          pBuffer,
          i2cBus,
          i2cDevAddr,
          i2cMemAddr + 2, /* Only need 6 of the 8 bytes */
          i2cMemAddrSize,
          DB_getElemSize( DB_MAC_ADDR )
    );


   return (ERR_NONE);
}

/******************************************************************************/
static size_t DB_getElemSize( DB_Element_t elem )
{
   /* Make sure that the element actually exists */
   if ( DB_MAX_ELEM <= elem ) {
      return 0;
   }

   return( settingsDB[elem].size );
}

/**
 * @}
 * end addtogroup groupCoupler
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
