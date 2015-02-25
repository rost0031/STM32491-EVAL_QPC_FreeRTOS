/**
 * @file		db.h
 * @brief   A database of settings that works on any memory type.
 *
 * This is an implementation of a database of settings that will be stored by
 * the system.  The intention is to store these settings in the I2C EEPROM, this
 * implementation abstracts away the ability to locate and size the different
 * elements in the different sections of the EEPROM as well as other locations.
 * (See Note). It also provides interfaces to allow DB access using blocking
 * (bare metal) and non-blocking (QP event based).
 *
 * The database consists of a structure that maps all the different types to a
 * memory location in memory and allows non-linear access to read/update the
 * values without manually calculating offsets and sizes.
 *
 * @note: While most of the settings are stored in the main section of the
 * EEPROM, there are a few settings that reside in completely separate RO
 * sections of the EEPROM, such as the UID and SN.  Other settings may be GPIO
 * controlled, such as the board position in the barrel and the processor
 * position on the coupler board.
 *
 * @date 	Feb 24, 2015
 * @author	Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard.  All rights reserved. 
 *
 * @addtogroup groupSettings
 * @{
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DB_H_
#define DB_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * All the different settings that can exist in the settings database
 */
typedef enum DB_Elements {
   DB_VERSION  = 0, /**< Keeps track of DB version to allow upgrades and additions */
   DB_MAC_ADDR, /**< Mac address stored in the special UID section of the RO EEPROM */
   DB_IP_ADDR, /**< IP address stored in main EEPROM */
   DB_SN, /**< Serial number stored in the special SN section of RO EEPROM */

   /* Add more elements here.  If adding elements after code is released, bump
    * the DB_VERSION up. */

   DB_MAX_ELEM /**< Max number of elements that can be stored.  ALWAYS LAST */
} DB_Element_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 *
 */
CBErrorCode DB_getMacAddrBLK(
      uint8_t* pBuffer,
      size_t bufSize,
      AccessType_t accType
);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupSettings
 */
#endif                                                               /* DB_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
