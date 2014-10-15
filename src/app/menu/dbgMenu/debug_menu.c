/**
 * @file    debug_menu.c
 * @brief   Handles the debug menu section of the menu application.
 *
 * @date    09/29/2014
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
#include "debug_menu.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

treeNode_t menuDbg;
char *const menuDbg_TitleTxt = "Debug Menu";
char *const menuDbg_SelectKey = "DBG";

//treeNode_t menuDbgItem_toggleSerialDebug;
//char *const menuDbgItem_toggleSerialDebugTxt = "Toggle debugging over serial port ON/OFF";
//char *const menuDbgItem_toggleSerialDebugSelectKey = "S";
//
//treeNode_t menuDbgItem_toggleEthDebug;
//char *const menuDbgItem_toggleEthDebugTxt = "Toggle debugging over ethernet port ON/OFF";
//char *const menuDbgItem_toggleEthDebugSelectKey = "E";

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

///******************************************************************************/
//void MENU_toggleSerialDebugAction( void )
//{
//   DBG_printf("Toggle Serial Debug Action test\n");
//}
//
///******************************************************************************/
//void MENU_toggleEthDebugAction( void )
//{
//   DBG_printf("Toggle Ethernet Debug Action test\n");
//}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
