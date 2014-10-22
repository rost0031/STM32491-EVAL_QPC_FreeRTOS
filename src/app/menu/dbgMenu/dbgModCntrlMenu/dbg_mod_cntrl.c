/**
 * @file    dbg_out_cntrl.c
 * @brief   Handles the debug output control menu items of the menu application.
 *
 * @date    10/13/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "dbg_mod_cntrl.h"
#include "dbg_cntrl.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

treeNode_t menuDbgModCntrl;
char *const menuDbgModCntrl_TitleTxt = "Debug Module Control Menu";
char *const menuDbgModCntrl_SelectKey = "MOD";

treeNode_t menuDbgModCntrlItem_toggleModGen;
char *const menuDbgModCntrlItem_toggleModGenTxt =
      "Toggle GENERAL module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModGenSelectKey = "GEN";

treeNode_t menuDbgModCntrlItem_toggleModSer;
char *const menuDbgModCntrlItem_toggleModSerTxt =
      "Toggle SERIAL module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModSerSelectKey = "SER";

treeNode_t menuDbgModCntrlItem_toggleModTime;
char *const menuDbgModCntrlItem_toggleModTimeTxt =
      "Toggle TIME module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModTimeSelectKey = "TIM";

treeNode_t menuDbgModCntrlItem_toggleModEth;
char *const menuDbgModCntrlItem_toggleModEthTxt =
      "Toggle ETHERNET module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModEthSelectKey = "ETH";

treeNode_t menuDbgModCntrlItem_toggleModI2C;
char *const menuDbgModCntrlItem_toggleModI2CTxt =
      "Toggle I2C module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModI2CSelectKey = "I2C";

treeNode_t menuDbgModCntrlItem_toggleModNOR;
char *const menuDbgModCntrlItem_toggleModNORTxt =
      "Toggle NOR module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModNORSelectKey = "NOR";

treeNode_t menuDbgModCntrlItem_toggleModSDR;
char *const menuDbgModCntrlItem_toggleModSDRTxt =
      "Toggle SDRAM module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModSDRSelectKey = "SDR";

treeNode_t menuDbgModCntrlItem_toggleModDBG;
char *const menuDbgModCntrlItem_toggleModDBGTxt =
      "Toggle DBG module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModDBGSelectKey = "DBG";

treeNode_t menuDbgModCntrlItem_toggleModCOMM;
char *const menuDbgModCntrlItem_toggleModCOMMTxt =
      "Toggle COMM module debugging ON/OFF";
char *const menuDbgModCntrlItem_toggleModCOMMSelectKey = "COM";

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void MENU_toggleDbgModGeneralAction( void )
{
   DBG_printf("Debugging for GENERAL module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_GENERAL) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_GENERAL);
   LOG_printf("Debugging for GENERAL module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_GENERAL) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModSerialAction( void )
{
   DBG_printf("Debugging for SERIAL module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_SERIAL) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_SERIAL);
   LOG_printf("Debugging for SERIAL module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_SERIAL) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModTimeAction( void )
{
   DBG_printf("Debugging for TIME module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_TIME) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_TIME);
   LOG_printf("Debugging for TIME module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_TIME) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModEthAction( void )
{
   DBG_printf("Debugging for ETHERNET module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_ETH) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_ETH);
   LOG_printf("Debugging for ETHERNET module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_ETH) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModI2CAction( void )
{
   DBG_printf("Debugging for I2C module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_I2C) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_I2C);
   LOG_printf("Debugging for I2C module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_I2C) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModNORAction( void )
{
   DBG_printf("Debugging for NOR module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_NOR) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_NOR);
   LOG_printf("Debugging for NOR module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_NOR) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModSDRAMAction( void )
{
   DBG_printf("Debugging for SDRAM module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_SDRAM) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_SDRAM);
   LOG_printf("Debugging for SDRAM module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_SDRAM) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModDBGAction( void )
{
   DBG_printf("Debugging for DBG module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_DBG) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_DBG);
   LOG_printf("Debugging for DBG module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_DBG) ? "ON" : "OFF" );
}

/******************************************************************************/
void MENU_toggleDbgModCOMMAction( void )
{
   DBG_printf("Debugging for COMM module was %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_COMM) ? "ON" : "OFF" );
   DBG_TOGGLE_DEBUG_FOR_MODULE(DBG_MODL_COMM);
   LOG_printf("Debugging for COMM module is now %s\n", DBG_CHECK_DEBUG_FOR_MODULE(DBG_MODL_COMM) ? "ON" : "OFF" );
}


/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
