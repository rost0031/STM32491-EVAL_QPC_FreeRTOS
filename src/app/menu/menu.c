/**
 * @file    menu.h
 * @brief   Handles the menu application.
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
#include "menu.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

//#include "debug_menu.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_MENU );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
//char *arrMenuTopLevel[20];
//static treeNode_t menuTree;
//
//static treeNode_t debugSubMenu;
//const char debugMenuTitleTxt = "Debug Menu";
//
//static treeNode_t debugMenuItemTimeTest;
//const char debugMenuTimeTestTxt = "Run debug output time test";
//
//static treeNode_t debugMenuItemNORTest;
//const char debugMenuNORTestTxt = "Run destructive NOR Flash memory test";
//
//static treeNode_t debugMenuItemSDRAMTest;
//const char debugMenuSDRAMTestTxt = "Run destructive SDRAM test";
//
//static treeNode_t debugMenuItemSDRAM_NORTest;
//const char debugMenuSDRAM_NORTestTxt = "Run destructive SDRAM/NOR Flash interaction test";
//
//static treeNode_t someOtherSubMenu;
//const char someOtherMenuTitleTxt = "Some Other Menu";
//const char someOtherMenuTimeTestTxt1 = "Some Other Menu Item 1";
//const char someOtherMenuTimeTestTxt2 = "Some Other Menu Item 2";
//const char someOtherMenuTimeTestTxt3 = "Some Other Menu Item 3";
//const char someOtherMenuTimeTestTxt4 = "Some Other Menu Item 4";


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
//CBErrorCode MENU_init( void )
//{
//   CBErrorCode status = ERR_NONE; /* Clear initial error and set it if it happens */
//
//   /* Initialize the root node */
//   menuTree.actionToTake = NULL;
//   menuTree.firstChildNode = NULL;
//   menuTree.firstSiblingNode = NULL;
//   menuTree.parent = NULL;
//   menuTree.text = NULL;
//
//   MENU_addSubMenu(
//         &debugSubMenu,
//         MENU_getNextSubMenuSlot( &menuTree->firstChildNode ),
//         debugMenuTitleTxt
//   );
//
//   MENU_addItem( &debugMenuItemTimeTest, &debugSubMenu, debugMenuTimeTestTxt );
//   MENU_addItem( &debugMenuItemNORTest, &debugSubMenu, debugMenuTimeTestTxt );
//   MENU_addItem( &debugMenuItemSDRAMTest, &debugSubMenu, debugMenuTimeTestTxt );
//   MENU_addItem( &debugMenuItemSDRAM_NORTest, &debugSubMenu, debugMenuTimeTestTxt );
//
//   MENU_addSubMenu( &menuTree, &someOtherSubMenu );
//
//
//   return( status );
//}
//
///******************************************************************************/
//void MENU_addSubMenu( treeNode_t *subMenuToAdd, treeNode_t *parentMenuItem, const char *subMenuText)
//{
//   subMenuToAdd->text = subMenuText;
//   subMenuToAdd->parent = parentMenuItem;
//   subMenuToAdd->firstSiblingNode = NULL;
//   subMenuToAdd->firstChildNode = NULL;
//   subMenuToAdd->actionToTake = NULL;
//}
//
//void MENU_addItem( treeNode_t *menuItem, treeNode_t *menuItemParent, const char *menuItemText )
//{
//   menuItemParent->firstSiblingNode = menuItem;
//   menuItemParent->text = menuItemText;
//   menuItemParent->actionToTake = MENU_testAction();
//}
//
//void MENU_testAction( void )
//{
//   DBG_printf("Testing an action\n");
//}
//
//treeNode_t* MENU_getNextSubMenuSlot( treeNode_t *menu )
//{
//   if ( NULL != menu->firstSiblingNode ) {
//      return( MENU_getNextSubMenuSlot( &menuTree->firstChildNode ) );
//   } else {
//      return ( menu->firstSiblingNode );
//   }
//}
//
//treeNode_t* MENU_getNextSubMenuSlot( treeNode_t *menu )
//{
//   if ( NULL != menu->firstChildNode ) {
//      return( MENU_getNextSubMenuSlot( &menuTree->firstChildNode ) );
//   } else {
//      return ( menu->firstSiblingNode );
//   }
//}
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
