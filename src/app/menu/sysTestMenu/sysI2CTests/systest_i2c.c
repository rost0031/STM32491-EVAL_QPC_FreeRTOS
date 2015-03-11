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
#include "DbgMgr.h"
#include "i2c_dev.h"

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

   /* Subscribe to the signal of the event that is expected to be returned. This
    * is not subscribed to by default to avoid printing out every single I2C
    * read request sent to the I2C1DevMgr.  As soon as the signal handler for
    * this event gets triggered in DbgMgr AO, it will un-subscribe from it. */
   QActive_subscribe(AO_DbgMgr, I2C1_DEV_READ_DONE_SIG);

   MENU_printf(
         dst,
         "--- Test Start --- Running an EEPROM read test. Reading %d bytes from 0x%02x\n",
         bytes,
         memAddr
   );

   /* Do a read from the EEPROM on the I2C Bus */
   CBErrorCode status = I2C_readDevMemEVT(
      EEPROM,                                         // I2C_Dev_t iDev,
      memAddr,                                        // uint16_t offset,
      bytes,                                          // uint16_t bytesToRead,
      ACCESS_QPC,                                     // AccessType_t accType,
      AO_DbgMgr                                       // QActive* callingAO
   );

   /* Print error if exists */
   ERR_COND_OUTPUT(
         status,
         ACCESS_QPC,
         "Error 0x%08x running EEPROM READ test\n",
         status
   );
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
   uint16_t memAddr = 0x00;
   uint8_t bytes = 16;
   /* Subscribe to the signal of the event that is expected to be returned. This
    * is not subscribed to by default to avoid printing out every single I2C
    * read request sent to the I2C1DevMgr.  As soon as the signal handler for
    * this event gets triggered in DbgMgr AO, it will un-subscribe from it. */
   QActive_subscribe(AO_DbgMgr, I2C1_DEV_READ_DONE_SIG);

   MENU_printf(
         dst,
         "--- Test Start --- Running an SN_ROM read test. Reading %d bytes from 0x%02x\n",
         bytes,
         memAddr
   );

      /* Do a read from the EEPROM on the I2C Bus */
   CBErrorCode status = I2C_readDevMemEVT(
      SN_ROM,                                         // I2C_Dev_t iDev,
      memAddr,                                        // uint16_t offset,
      bytes,                                          // uint16_t bytesToRead,
      ACCESS_QPC,                                     // AccessType_t accType,
      AO_DbgMgr                                       // QActive* callingAO
   );

   /* Print error if exists */
   ERR_COND_OUTPUT(
         status,
         ACCESS_QPC,
         "Error 0x%08x running SN_ROM READ test\n",
         status
   );
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

   uint16_t memAddr = 0x00;
   uint8_t bytes = 8;

   /* Subscribe to the signal of the event that is expected to be returned. This
    * is not subscribed to by default to avoid printing out every single I2C
    * read request sent to the I2C1DevMgr.  As soon as the signal handler for
    * this event gets triggered in DbgMgr AO, it will un-subscribe from it. */
   QActive_subscribe(AO_DbgMgr, I2C1_DEV_READ_DONE_SIG);

   MENU_printf(
         dst,
         "--- Test Start --- Running an EUI_ROM read test. Reading %d bytes from 0x%02x\n",
         bytes,
         memAddr
   );

   /* Do a read from the EEPROM on the I2C Bus */
   CBErrorCode status = I2C_readDevMemEVT(
         EUI_ROM,                                        // I2C_Dev_t iDev,
         memAddr,                                        // uint16_t offset,
         bytes,                                          // uint16_t bytesToRead,
         ACCESS_QPC,                                     // AccessType_t accType,
         AO_DbgMgr                                       // QActive* callingAO
   );

   /* Print error if exists */
   ERR_COND_OUTPUT(
         status,
         ACCESS_QPC,
         "Error 0x%08x running EUI_ROM READ test\n",
         status
   );
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

   /* Subscribe to the signal of the event that is expected to be returned. This
    * is not subscribed to by default to avoid printing out every single I2C
    * read request sent to the I2C1DevMgr.  As soon as the signal handler for
    * this event gets triggered in DbgMgr AO, it will un-subscribe from it. */
   QActive_subscribe(AO_DbgMgr, I2C1_DEV_WRITE_DONE_SIG);

   uint16_t memAddr = 0x00;
   uint8_t bytes = 16;
   uint8_t tmp[bytes];
   memset(tmp, 0, sizeof(tmp));
   for( uint8_t i=0; i< bytes; i++ ) {
      tmp[i] = i+5; // Write some numbers to the buffer.
   }

   MENU_printf(
         dst,
         "--- Test Start --- Running an EEPROM write test. Writing %d bytes to 0x%02x\n",
         bytes,
         memAddr
   );

   /* Do a write to the EEPROM on the I2C Bus */
   CBErrorCode status = I2C_writeDevMemEVT(
      EEPROM,                                      // I2C_Dev_t iDev,
      memAddr,                                     // uint16_t offset,
      bytes,                                       // uint16_t bytesToWrite,
      ACCESS_QPC,                                  // AccessType_t accType,
      AO_DbgMgr,                                   // QActive* callingAO
      tmp                                          // uint8_t *pBuffer,
   );

   /* Print error if exists */
   ERR_COND_OUTPUT(
         status,
         ACCESS_QPC,
         "Error 0x%08x running EUI_ROM READ test\n",
         status
   );
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
