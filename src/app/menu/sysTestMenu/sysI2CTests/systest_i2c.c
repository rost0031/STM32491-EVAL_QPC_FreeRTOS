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
#include "stm324x9i_eval_ioe16.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Variables to define all the menu items in this submenu */
treeNode_t menuItem_runI2CEEPROMReadTest;
char *const menuSysTest_runI2CEEPROMReadTest_Txt = "Run I2C EEPROM Read test.";
char *const menuSysTest_runI2CEEPROMReadTest_SelectKey = "EER";

treeNode_t menuItem_runI2CSNReadTest;
char *const menuSysTest_runI2CSNReadTest_Txt = "Run I2C Serial Number Read test.";
char *const menuSysTest_runI2CSNReadTest_SelectKey = "SNR";

treeNode_t menuItem_runI2CEUI64ReadTest;
char *const menuSysTest_runI2CEUI64ReadTest_Txt = "Run I2C EUI64 Read test.";
char *const menuSysTest_runI2CEUI64ReadTest_SelectKey = "UIR";

treeNode_t menuItem_runI2CEEPROMWriteTest;
char *const menuSysTest_runI2CEEPROMWriteTest_Txt = "Run I2C EEPROM Write test.";
char *const menuSysTest_runI2CEEPROMWriteTest_SelectKey = "EEW";

treeNode_t menuItem_runI2CIOExpRegReadTest;
char *const menuSysTest_runI2CIOExpRegReadTest_Txt = "Run I2C IO Expander Register Read test.";
char *const menuSysTest_runI2CIOExpRegReadTest_SelectKey = "IRR";

treeNode_t menuItem_runI2CIOExpRegWriteTest;
char *const menuSysTest_runI2CIOExpRegWriteTest_Txt = "Run I2C IO Expander Register Write test.";
char *const menuSysTest_runI2CIOExpRegWriteTest_SelectKey = "IRW";

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

/******************************************************************************/
void MENU_i2cEEPROMWriteTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   uint16_t memAddr = 0x00;
   uint8_t bytes = 16;
   uint8_t tmp[bytes];
   memset(tmp, 0, sizeof(tmp));
   for( uint8_t i=0; i< bytes; i++ ) {
      tmp[i] = i+5; // Write some numbers to the buffer.
   }
   MENU_printf(dst, "Running an EEPROM write test. Writing %d bytes to 0x%02x\n", bytes, memAddr);
   /* Publish event to start an EEPROM read */
   I2CEEPROMWriteReqEvt *i2cEERPOMWriteReqEvt = Q_NEW(I2CEEPROMWriteReqEvt, EEPROM_RAW_MEM_WRITE_SIG);
   i2cEERPOMWriteReqEvt->addr = memAddr;
   i2cEERPOMWriteReqEvt->bytes  = bytes;
   MEMCPY(
         i2cEERPOMWriteReqEvt->dataBuf,
         tmp,
         bytes
   );

   QF_PUBLISH((QEvent *)i2cEERPOMWriteReqEvt, AO_DbgMgr);
}

/******************************************************************************/
void MENU_i2cIOExpRegReadTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   uint16_t memAddr = 0x00;
   MENU_printf(dst, "Running IO Expander Register Read test. Reading from register 0x%02x\n", memAddr);

//   /* Publish event to start an register read */
//   I2CDevRegReadReqEvt *i2cReqEvt = Q_NEW(I2CDevRegReadReqEvt, IOEXP_REG_READ_SIG);
//   i2cReqEvt->regAddr = IOE16_REG_CHP_ID_LSB;
//   QF_PUBLISH((QEvent *)i2cReqEvt, AO_DbgMgr);

//   /* Publish event to start an register read */
//   I2CDevRegReadReqEvt *i2cReqEvt = Q_NEW(I2CDevRegReadReqEvt, IOEXP_REG_READ_SIG);
//   i2cReqEvt->regAddr = IOE16_REG_ISGPIOR_LSB;
//   QF_PUBLISH((QEvent *)i2cReqEvt, AO_DbgMgr);

//   __IO uint16_t tmpsr = IOE16_GetITStatus();
//   DBG_printf("Read 0x%04x from IT reg on IOE\n", tmpsr);

   /* Read all the registers on the damn thing */
   uint16_t tmp = 0x0000;

   /* Read the device ID  */
   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_CHP_ID_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_CHP_ID_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_CHP_ID:   (0x%02x) register\n", tmp, IOE16_REG_CHP_ID_LSB);
   tmp = 0x0000;

   tmp = (uint16_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_ID_VER);
   DBG_printf("Read 0x%04x from IOE16_REG_ID:       (0x%02x) register\n", tmp, IOE16_REG_ID_VER);
   tmp = 0x0000;

   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_SYS_CTRL);
   DBG_printf("Read 0x%04x from IOE16_REG_SYS_CTRL: (0x%02x) register\n", tmp, IOE16_REG_SYS_CTRL);
   tmp = 0x0000;

   /* Read the interrupt control register */
   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_IEGPIOR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_IEGPIOR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_IEGPIOR:  (0x%02x) register\n", tmp, IOE16_REG_IEGPIOR_LSB);
   tmp = 0x0000;

   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_ISGPIOR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_ISGPIOR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_ISGPIOR:  (0x%02x) register\n", tmp, IOE16_REG_ISGPIOR_LSB);
   tmp = 0x0000;

   /* Read GPMR register */
   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPMR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPMR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_GPMR:     (0x%02x) register\n", tmp, IOE16_REG_GPMR_LSB);
   tmp = 0x0000;

   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPSR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPSR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_GPSR:     (0x%02x) register\n", tmp, IOE16_REG_GPSR_LSB);
   tmp = 0x0000;

   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPDR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPDR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_GPDR:     (0x%02x) register\n", tmp, IOE16_REG_GPDR_LSB);
   tmp = 0x0000;

   tmp = IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPPIR_MSB);
   tmp = (uint32_t)(tmp << 8);
   tmp |= (uint32_t)IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPPIR_LSB);
   DBG_printf("Read 0x%04x from IOE16_REG_GPPIR:    (0x%02x) register\n", tmp, IOE16_REG_GPPIR_LSB);
   tmp = 0x0000;

}

/******************************************************************************/
void MENU_i2cIOExpRegWriteTestAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
)
{
   CB_UNUSED_ARG(dataBuf);
   CB_UNUSED_ARG(dataLen);
   uint16_t memAddr = IOE16_REG_SYS_CTRL;
   uint8_t regValue = IOE16_IT_EN;
   MENU_printf(dst, "Running IO Expander Register Write test. Writing 0x%02x to register 0x%02x\n", regValue, memAddr);

   /* Publish event to start an register read */
   I2CDevRegWriteReqEvt *i2cReqEvt = Q_NEW(I2CDevRegWriteReqEvt, IOEXP_REG_WRITE_SIG);
   i2cReqEvt->regAddr = memAddr;
   i2cReqEvt->regValue = regValue;
   QF_PUBLISH((QEvent *)i2cReqEvt, AO_DbgMgr);
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
