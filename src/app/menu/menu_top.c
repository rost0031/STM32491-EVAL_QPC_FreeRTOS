/**
 * @file    menu_top.c
 * @brief   Top level of the menu.
 *
 * @date    10/10/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "menu_top.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

#include "debug_menu.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_MENU );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief   Storage for the root of the menu.  MenuMgr has a pointer to this.
 */
treeNode_t menu;

/**
 * @brief   Text of the main menu.  Displayed when the menu gets printed.
 */
char *const menu_TitleTxt = "Coupler Board Menu";

/**
 * @brief   Selector key for the top of the menu.
 * This is the letter sequence that the user inputs to jump back to the top of
 * the menu.
 */
char *const menu_SelectKey = "T";

/* Private function prototypes -----------------------------------------------*/


/**
 * @brief   Prints out the help text for the entire menu system.
 * @param [in] msgSrc: MsgSrc var that specifies where to print out to.
 * @return  None
 */
static void MENU_printHelp( MsgSrc msgSrc );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
treeNode_t* MENU_init( void )
{
   /* 1. This has to get called first.  It initializes the top level of the menu */
   MENU_initTopLevel( &menu, menu_TitleTxt, menu_SelectKey );

   /* 2. At this point, submenus and their items can be added.  The nodes, text,
    * actions, etc have to be defined in their respective c files and the vars
    * declared "extern" so they can be accessed from here. The menus and menu
    * items will appear in the order in which they are added here. */

   /* Add a menu */
   MENU_addSubMenu(
         &menuDbg,                                      /**< Menu being added */
         &menu,                           /**< Parent of the menu being added */
         menuDbg_TitleTxt,                   /**< Menu being added title text */
         menuDbg_SelectKey,               /**< Menu being added selection key */
         NULL                         /**< Action taken when menu is selected */
   );

   /* Add menu items for this menu */
   MENU_addMenuItem(
         &menuDbgItem_toggleSerialDebug,           /**< Menu item being added */
         &menuDbg,                   /**< Parent of the menu item being added */
         menuDbgItem_toggleSerialDebugTxt,          /**< Menu item title text */
         menuDbgItem_toggleSerialDebugSelectKey, /**< Menu item selection key */
         MENU_toggleSerialDebugAction /**< Action taken when menu item is selected */
   );

   MENU_addMenuItem(
         &menuDbgItem_toggleEthDebug,              /**< Menu item being added */
         &menuDbg,                   /**< Parent of the menu item being added */
         menuDbgItem_toggleEthDebugTxt,             /**< Menu item title text */
         menuDbgItem_toggleEthDebugSelectKey,    /**< Menu item selection key */
         MENU_toggleEthDebugAction    /**< Action taken when menu item is selected */
   );

   return( &menu ); /* return a pointer to the top level of the menu tree */
}

/******************************************************************************/
treeNode_t* MENU_parse(
      treeNode_t* node,
      const char* pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
)
{
   treeNode_t *newNode = node;

   /* Sanity check. If the passed in node is null, set to the top of the menu
    * and inform user. This really shouldn't happen and if it does, it is a bug
    * in the menu handling application. */
   if ( NULL == node ) {
      WRN_printf("Passed in a null node.  Setting to top of menu\n");
      newNode = &menu;
      MENU_printMenuExpandedAtCurrNode( newNode, msgSrc );
      return( newNode );
   }

   /* Parse the universal commands first */
   if ( 0 == strncmp((const char *)pBuffer, "?", 1 ) ) {
      MENU_printHelp( msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "T", 1 ) ) {
      newNode = &menu;
      MENU_printMenuExpandedAtCurrNode( newNode, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "P", 1 ) ) {
      MENU_printMenuExpandedAtCurrNode(newNode, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "A", 1 ) ) {
      MENU_printMenuTree(newNode, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "U", 1 ) ) {
      if ( NULL != node->fakeParentNode ) {
         newNode = node->fakeParentNode;
      } else {
         LOG_printf("Already at the top of the menu\n");
      }
      MENU_printMenuExpandedAtCurrNode(newNode, msgSrc );
   } else {
      newNode = MENU_parseCurrLevelMenuItems(node, pBuffer, bufferLen, msgSrc);
   }
   return( newNode );
}

/******************************************************************************/
static void MENU_printHelp( MsgSrc msgSrc )
{
   MENU_printf("******************************************************************************\n");
   MENU_printf("*****                           Menu Help                                *****\n");
   MENU_printf("***** Type the following commands:                                       *****\n");
   MENU_printf("***** '?': menu help                                                     *****\n");
   MENU_printf("***** 'P': print currently selected menu                                 *****\n");
   MENU_printf("***** 'A': print entire expanded menu tree                               *****\n");
   MENU_printf("***** 'T': go to the top of the menu                                     *****\n");
   MENU_printf("***** 'U': go up one level from where you're currently at                *****\n");
   MENU_printf("***** any identifier to select the submenu or menu item on your level    *****\n");
   MENU_printf("******************************************************************************\n");
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
