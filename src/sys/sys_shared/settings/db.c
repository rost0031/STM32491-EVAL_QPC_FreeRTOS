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
#include "ipAndMac.h"                                  /* for default IP addr */

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
   DB_Elem_t    elem;  /**< Specifies which settings element is being described */
   DB_ElemLoc_t  loc;  /**< Specifies how the db element is stored */
   size_t       size;  /**< Specifies the size of the db element */
   uint16_t    offset; /**< Specifies the offset from beginning of EEPROM memory
                            where the element is stored */
} SettingsDB_Desc_t;

/**
 * DB element structure mapping that resides in the main memory of the EEPROM
 */
typedef struct {
   uint32_t dbMagicWord; /**< Magic word that specifies whether a DB exists.  If
                              this is not present, we have to initialize the DB
                              in EEPROM memory to some default.*/
   uint16_t dbVersion;   /**< Version of the database */
   uint8_t ipAddr[4];  /**< 4 values of the 111.222.333.444 ip address in hex */
} SettingsDB_t;


/* Private defines -----------------------------------------------------------*/

/**< Magic word that will be stored at the head of the DB.  If this is not
 * there, the DB needs to be initialized to a default and updated after. */
#define DB_MAGIC_WORD_DEF   0xdefec8db

/**< Current version of the DB.  This needs to be bumped once the FW is out in
 * the field and any changes are made to the DB. This is to allow for a smooth
 * upgrade of the DB. */
#define DB_VERSION_DEF      0x0001

/* Private macros ------------------------------------------------------------*/

/**< Macro to get size of the element stored in the EEPROM memory */
#define DB_SIZE_OF_ELEM(s,m)     ((size_t) sizeof(((s *)0)->m))

/**< Macro to get the offset of the element stored in the EEPROM memory */
#define DB_LOC_OF_ELEM(s,m)      offsetof(s, m)

/* Private variables and Local objects ---------------------------------------*/

/**< Array to specify where all the DB elements reside */
static const SettingsDB_Desc_t settingsDB[DB_MAX_ELEM] = {
      { DB_MAGIC_WORD,  DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbMagicWord), DB_LOC_OF_ELEM(SettingsDB_t, dbMagicWord)},
      { DB_VERSION,     DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbVersion)  , DB_LOC_OF_ELEM(SettingsDB_t, dbVersion)  },
      { DB_MAC_ADDR,    DB_UI_ROM,  6                                         , 2                                        },
      { DB_IP_ADDR,     DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, ipAddr)     , DB_LOC_OF_ELEM(SettingsDB_t, ipAddr)     },
      { DB_SN,          DB_SN_ROM,  16                                        , 0                                        },
};

/**< Default settings that belong in the main memory of the EEPROM */
static const SettingsDB_t DB_defaultEeepromSettings = {
      .dbMagicWord = DB_MAGIC_WORD_DEF,
      .dbVersion   = DB_VERSION_DEF,
      .ipAddr = {STATIC_IPADDR0, STATIC_IPADDR1, STATIC_IPADDR2, STATIC_IPADDR3}
};

/* Private function prototypes -----------------------------------------------*/
static size_t DB_getElemSize( DB_Elem_t elem );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
CBErrorCode DB_isValid( void )
{
   I2C_Bus_t i2cBus = I2CBus1;

   /* Look up Device and Memory addresses and their sizes */
   uint16_t i2cDevAddr = I2C_getI2C1DevAddr( EEPROM );
   uint8_t i2cMemAddrSize = I2C_getI2C1MemAddrSize( EEPROM );
   uint16_t i2cMemAddr = I2C_getI2C1MemAddr( EEPROM );

   /* 3. Call the actual I2C blocking read function */
   uint32_t db_magicWord = 0;
   I2C_readBufferBLK(
         i2cBus,
         i2cDevAddr,
         i2cMemAddr + settingsDB[DB_MAGIC_WORD].offset,
         i2cMemAddrSize,
         (uint8_t *)&db_magicWord,
         settingsDB[DB_MAGIC_WORD].size
   );

   if ( db_magicWord != DB_MAGIC_WORD_DEF ) {
      wrn_slow_printf(
            "Magic word in DB (0x%08x) doesn't match expected (0x%08x)\n",
            db_magicWord,
            DB_MAGIC_WORD_DEF
      );
      return( ERR_DB_NOT_INIT);
   }

   uint16_t db_version = 0;
   I2C_readBufferBLK(
         i2cBus,
         i2cDevAddr,
         i2cMemAddr + settingsDB[DB_VERSION].offset,
         i2cMemAddrSize,
         (uint8_t *)&db_version,
         settingsDB[DB_VERSION].size
   );

   if ( db_version != DB_VERSION_DEF ) {
      wrn_slow_printf(
            "DB version in EEPROM (0x%04x) doesn't match expected (0x%04x)\n",
            db_version,
            DB_VERSION_DEF
      );
      return( ERR_DB_VER_MISMATCH);
   }

   return (ERR_NONE);
}

/******************************************************************************/
CBErrorCode DB_initToDefault( void )
{
   /* Write the default DB to EEPROM. */
   CBErrorCode status = I2C_writeEepromBLK(
         (uint8_t *)&DB_defaultEeepromSettings.dbMagicWord,
         settingsDB[DB_MAGIC_WORD].offset,
         settingsDB[DB_MAGIC_WORD].size
   );
   if ( ERR_NONE != status ) {
      err_slow_printf(
            "Unable to write Magic Word to EEPROM. Error: 0x%08x\n",
            status
      );
      return status;
   }

   status = I2C_writeEepromBLK(
         (uint8_t *)&DB_defaultEeepromSettings.dbVersion,
         settingsDB[DB_VERSION].offset,
         settingsDB[DB_VERSION].size
   );
   if ( ERR_NONE != status ) {
      err_slow_printf(
            "Unable to write DB Version to EEPROM. Error: 0x%08x\n",
            status
      );
      return status;
   }

   status = I2C_writeEepromBLK(
         (uint8_t *)&DB_defaultEeepromSettings.ipAddr,
         settingsDB[DB_IP_ADDR].offset,
         settingsDB[DB_IP_ADDR].size
   );
   if ( ERR_NONE != status ) {
      err_slow_printf(
            "Unable to write IP Address to EEPROM. Error: 0x%08x\n",
            status
      );
      return status;
   }

   return status;
}
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

   /* 3. Call the actual I2C blocking read function */
   I2C_readBufferBLK(
         i2cBus,
         i2cDevAddr,
         i2cMemAddr + 2, /* Only need 6 of the 8 bytes */
         i2cMemAddrSize,
         pBuffer,
         DB_getElemSize( DB_MAC_ADDR )
   );


   return (ERR_NONE);
}

/******************************************************************************/
static size_t DB_getElemSize( DB_Elem_t elem )
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
