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
   I2CMemReadReqEvt *i2cMemReadReqEvt = Q_NEW(I2CMemReadReqEvt, EEPROM_RAW_MEM_READ_SIG);
   i2cMemReadReqEvt->addr = memAddr;
   i2cMemReadReqEvt->bytes  = bytes;
   QF_PUBLISH((QEvent *)i2cMemReadReqEvt, AO_CommStackMgr);
}
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
