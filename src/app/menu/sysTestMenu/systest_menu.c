/**
 * @file    systest_menu.c
 * @brief   Handles the system tests menu section of the menu application.
 *
 * @date    10/02/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "ktree.h"
#include "systest_menu.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_MENU );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

static menuNode_t menuSysTest;
const char menuSysTest_TitleTxt = "System Test Menu";

static menuNode_t menuItem_runDbgSpeedTest;
const char menuSysTest_runDbgSpeedTest = "Run debug speed comparison test.";

static menuNode_t menuItem_runNORFlashTest;
const char menuSysTest_runNORFlashTest = "Run destructive NOR flash test.";

static menuNode_t menuItem_runSdramNorTest;
const char menuSysTest_runSdramNorTest = "Run SDRAM NOR interaction test.";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

///******************************************************************************/
//void MENU_debugMenuInit( treeNode_t *t )
//{
//
//
//}
//
///******************************************************************************/
//void MENU_addDebugMenu( treeNode_t *t )
//{
//   t->text = debugMenuTitleTxt;
//   t->parent = NULL;
//   t->firstSiblingNode = NULL;
//   t->firstChildNode = NULL;
//}
//
//void MENU_addItemToDebugMenu( treeNode_t *menuItem, treeNode_t *menuItemParent, const char *menuItemText )
//{
//   menuItemParent->parent = menuItem;
//   menuItemParent->text = menuItemText;
//}
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
