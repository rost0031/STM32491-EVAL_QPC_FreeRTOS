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
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev.  All rights reserved. 
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
   DB_MAGIC_WORD = 0,      /**< Magic word that specifies if a DB exists or
                                needs to be init to a default. */
   DB_VERSION,             /**< Keeps track of DB version to allow upgrades and
                                additions */
   DB_MAC_ADDR,            /**< Mac address stored in the special UID section of
                                the RO EEPROM */
   DB_IP_ADDR,             /**< IP address stored in main EEPROM */
   DB_SN,                  /**< Serial number stored in the special SN section
                                of RO EEPROM */

   /* Add more elements here.  If adding elements after code is released, bump
    * the DB_VERSION up. */

   DB_MAX_ELEM   /**< Max number of elements that can be stored.  ALWAYS LAST */
} DB_Elem_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief   Get a string representation of a DB element.
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to retrieve.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_MAC_ADDR: MAC address stored in the RO part of DB.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *    @arg DB_SN: Serial number stored in the RO part of DB.
 * @return str: char* representation of DB element if found,
 *             "" if not found.
 */
char* DB_elemToStr( DB_Elem_t elem );

/**
 * @brief   Check if Settings DB in EEPROM is valid.
 *
 * This function checks for a magic number at the beginning of the EEPROM and
 * the version to see if they match what's expected.
 *
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
CBErrorCode DB_isValid( AccessType_t accessType );

/**
 * @brief   Initialize the settings DB in EEPROM to a stored default.
 *
 * This function initializes the settings DB in EEPROM to a stored default.
 * This should only happen if it is found to be invalid by DB_isValid().
 *
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
CBErrorCode DB_initToDefault( AccessType_t accessType  );

/**
 * @brief   Get an element from the settings DB.
 *
 * This function retrieves an element from the database of settings.
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to retrieve.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_MAC_ADDR: MAC address stored in the RO part of DB.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *    @arg DB_SN: Serial number stored in the RO part of DB.
 *
 * @param  [out] *pBuffer: uint8_t pointer to a buffer where to store the
 *                         retrieved element.
 * @param  [in] bufSize: size of the pBuffer.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
CBErrorCode DB_getElemBLK(
      DB_Elem_t elem,
      uint8_t* pBuffer,
      size_t bufSize,
      AccessType_t accessType
);

/**
 * @brief   Set an element in the settings DB.
 *
 * This function sets an element to the database of settings.
 *
 * @note: Some elements in the DB cannot be set since they are in RO part of the
 * DB (RO EEPROM, GPIO, Flash, etc).
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to set.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *
 * @param  [in] *pBuffer: uint8_t pointer to a buffer where to element to be set
 *                         is.
 * @param  [in] bufSize: size of the pBuffer.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
CBErrorCode DB_setElemBLK(
      DB_Elem_t elem,
      uint8_t* pBuffer,
      size_t bufSize,
      AccessType_t accessType
);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupSettings
 */
#endif                                                               /* DB_H_ */
/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
