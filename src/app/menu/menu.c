/**
 * @file    menu.c
 * @brief   Implementation of the munu application using a statically allocated
 * k-ary tree structure.
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

#include "debug_menu.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_MENU );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
treeNode_t menu;
char *const menu_TitleTxt = "Coupler Board Menu";
char *const menu_SelectKey = "top";

/* Private function prototypes -----------------------------------------------*/
treeNode_t* MENU_initTopLevel( void );

treeNode_t* MENU_addChild(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

treeNode_t* MENU_addSubMenu(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

treeNode_t* MENU_addMenuItem(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
treeNode_t* MENU_init( void )
{
   /* 1. This has to get called first.  It initializes the top level of the menu */
   MENU_initTopLevel();

   /* 2. At this point, submenus and their items can be added.  The nodes, text,
    * actions, etc have to be defined in their respective c files and the vars
    * declared "extern" so they can be accessed from here. */

   /* Add a menu */
   MENU_addSubMenu( &menuDbg, &menu, menuDbg_TitleTxt, menuDbg_SelectKey, NULL );

   /* Add menu items for this menu */
   MENU_addMenuItem(
         &menuDbgItem_toggleSerialDebug,
         &menuDbg,
         menuDbgItem_toggleSerialDebugTxt,
         menuDbgItem_toggleSerialDebugSelectKey,
         MENU_toggleSerialDebugAction
   );

   MENU_addMenuItem(
         &menuDbgItem_toggleEthDebug,
         &menuDbg,
         menuDbgItem_toggleEthDebugTxt,
         menuDbgItem_toggleEthDebugSelectKey,
         MENU_toggleEthDebugAction
   );

   return( &menu ); /* return a pointer to the top level of the menu tree */
}

/******************************************************************************/
treeNode_t* MENU_initTopLevel( void )
{
   return( MENU_addChild(&menu, NULL, menu_TitleTxt, menu_SelectKey, NULL ) );
}

/******************************************************************************/
treeNode_t* MENU_addSubMenu(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
)
{
   return( MENU_addChild(node, parent, menuTxt, menuSelectTxt, action ) );
}

/******************************************************************************/
treeNode_t* MENU_addMenuItem(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
)
{
   return( MENU_addChild(node, parent, menuTxt, menuSelectTxt, action ) );
}

/******************************************************************************/
treeNode_t* MENU_addChild(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
)
{
   /* 1. Initialize this submenu */
   KTREE_nodeCtor( node );                      /* Call the node constructor */
   node->actionToTake = action;      /* Set the action to occur when selected */

   /* Set the parents (fake and real) of this node */
   node->trueParentNode = parent;   /* Set the parent node where the node is actually attached */
   node->fakeParentNode = parent;   /* Set the parent node that this node will appear under in a true tree structure */

   node->text = menuTxt;                                      /* Display text */
   node->selector = menuSelectTxt;                          /* Selection text */

   if ( NULL != parent ) {
      KTREE_addChild( node, parent, parent );
   }

   return( node );
}

/******************************************************************************/
void MENU_printMenuTree( treeNode_t *node, uint8_t level, MsgSrc whereToPrint )
{
   if ( NULL == node ) {
      return;
   } else {
      MENU_printNode( node, level, whereToPrint );
   }

   if ( NULL != node->firstChildNode ) {
      //      dbg_slow_printf("Child exists, descending one level\n");
      MENU_printMenuTree( node->firstChildNode, level+1, whereToPrint );
   }

   if ( NULL != node->firstSiblingNode ) {
      //      dbg_slow_printf("Sibling exits, moving right\n");
      MENU_printMenuTree( node->firstSiblingNode, level, whereToPrint );
   }
}

/******************************************************************************/
void MENU_printNode( treeNode_t *node, uint8_t level, MsgSrc whereToPrint )
{
   for ( uint8_t i = 0; i < level; i++ ) {
      MENU_printf("*--");
   }

   MENU_printf("%s to select.  %s\n", node->selector, node->text );
//   for ( uint8_t i = 0; i < level; i++ ) {
//      MENU_printf("   ");
//   }
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
