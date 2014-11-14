/**
 * @file    systest_i2c.h
 * @brief   Handles the system I2C tests menu items.
 *
 * @date    10/29/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYSTEST_I2C_H_
#define SYSTEST_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "Shared.h"
#include "ktree.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Variables to define all the menu items in this submenu */
extern treeNode_t menuItem_runI2CEEPROMReadTest;
extern char *const menuSysTest_runI2CEEPROMReadTest_Txt;
extern char *const menuSysTest_runI2CEEPROMReadTest_SelectKey;

extern treeNode_t menuItem_runI2CSNReadTest;
extern char *const menuSysTest_runI2CSNReadTest_Txt;
extern char *const menuSysTest_runI2CSNReadTest_SelectKey;

extern treeNode_t menuItem_runI2CEUI64ReadTest;
extern char *const menuSysTest_runI2CEUI64ReadTest_Txt;
extern char *const menuSysTest_runI2CEUI64ReadTest_SelectKey;

extern treeNode_t menuItem_runI2CEEPROMWriteTest;
extern char *const menuSysTest_runI2CEEPROMWriteTest_Txt;
extern char *const menuSysTest_runI2CEEPROMWriteTest_SelectKey;

extern treeNode_t menuItem_runI2CIOExpRegReadTest;
extern char *const menuSysTest_runI2CIOExpRegReadTest_Txt;
extern char *const menuSysTest_runI2CIOExpRegReadTest_SelectKey;

extern treeNode_t menuItem_runI2CIOExpRegWriteTest;
extern char *const menuSysTest_runI2CIOExpRegWriteTest_Txt;
extern char *const menuSysTest_runI2CIOExpRegWriteTest_SelectKey;

extern treeNode_t menuItem_runI2CTscPosReadTest;
extern char* const menuSysTest_runI2CTscPosReadTest_Txt;
extern char* const menuSysTest_runI2CTscPosReadTest_SelectKey;
/* Exported functions --------------------------------------------------------*/
/**
 * @brief Called by the menu item to run an I2C EEPROM read test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cEEPROMReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C Serial Number Read test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cSNReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C EUI64 Read test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cEUI64ReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C EEPROM write test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cEEPROMWriteTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C IO Expander register read test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cIOExpRegReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C IO Expander register write test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cIOExpRegWriteTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to run an I2C Touch Screen Controller (TSC)
 * Read Position test.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_i2cTscPosReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);
/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                      /* SYSTEST_I2C_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
