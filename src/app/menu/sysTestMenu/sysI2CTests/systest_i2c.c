/**
 * @file    systest_i2c.c
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
/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "systest_i2c.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "I2C1DevMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Variables to define all the menu items in this submenu */
treeNode_t menuItem_runI2CEEPROMReadTest;
char *const menuSysTest_runI2CEEPROMReadTest_Txt = "Run I2C EEPROM read test.";
char *const menuSysTest_runI2CEEPROMReadTest_SelectKey = "EER";

treeNode_t menuItem_runI2CSNReadTest;
char *const menuSysTest_runI2CSNReadTest_Txt = "Run I2C Serial Number Read test.";
char *const menuSysTest_runI2CSNReadTest_SelectKey = "SNR";

treeNode_t menuItem_runI2CEUI64ReadTest;
char *const menuSysTest_runI2CEUI64ReadTest_Txt = "Run I2C EUI64 Read test.";
char *const menuSysTest_runI2CEUI64ReadTest_SelectKey = "UIR";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void MENU_i2cEEPROMReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   uint16_t memAddr = 0x00;
   uint8_t bytes = 16;
   MENU_printf(dst, "Running an EEPROM read test. Reading %d bytes from 0x%02x\n", bytes, memAddr);
   /* Publish event to start an EEPROM read */
   I2CEEPROMReadReqEvt *i2cEERPOMReadReqEvt = Q_NEW(I2CEEPROMReadReqEvt, EEPROM_RAW_MEM_READ_SIG);
   i2cEERPOMReadReqEvt->addr = memAddr;
   i2cEERPOMReadReqEvt->bytes  = bytes;
   QF_PUBLISH((QEvent *)i2cEERPOMReadReqEvt, AO_DbgMgr);
}

/******************************************************************************/
void MENU_i2cSNReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   MENU_printf(dst, "Running an Serial Number Read test...\n");
   /* Publish event to start an Serial Number read */
   QEvt *qEvt = Q_NEW(QEvt, EEPROM_SN_READ_SIG);
   QF_PUBLISH((QEvent *)qEvt, AO_DbgMgr);
}

/******************************************************************************/
void MENU_i2cEUI64ReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   MENU_printf(dst, "Running an UIE (64-bit) Read test...\n");
   /* Publish event to start an Serial Number read */
   QEvt *qEvt = Q_NEW(QEvt, EEPROM_EUI64_READ_SIG);
   QF_PUBLISH((QEvent *)qEvt, AO_DbgMgr);
}
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
