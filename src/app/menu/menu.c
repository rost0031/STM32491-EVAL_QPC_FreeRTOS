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

menuNav_t menuNav;


treeNode_t menu;
char *const menu_TitleTxt = "Coupler Board Menu";
char *const menu_SelectKey = "top";

/* Private function prototypes -----------------------------------------------*/
static treeNode_t* MENU_initTopLevel( void );

static treeNode_t* MENU_addChild(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

static treeNode_t* MENU_addSubMenu(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

static treeNode_t* MENU_addMenuItem(
      treeNode_t *node,
      treeNode_t *parent,
      const char *menuTxt,
      const char *menuSelectTxt,
      pFunction action
);

static void MENU_findFakeAncestryPath( treeNode_t *node );
static void MENU_findTrueAncestryPath( treeNode_t *node );
static void MENU_clearAncestryPaths( void );

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
treeNode_t* MENU_parse( treeNode_t *node, uint8_t *pBuffer, uint16_t bufferLen, MsgSrc msgSrc )
{
   treeNode_t *newNode = node;

   /* Sanity check. If the passed in node is null, set to the top of the menu
    * and inform user. This really shouldn't happen and if it does, it is a bug
    * in the menu handling application. */
   if ( NULL == node ) {
      WRN_printf("Passed in a null node.  Setting to top of menu\n");
      newNode = &menu;
      MENU_printCurrMenu( newNode, 0, msgSrc );
      return( newNode );
   }

   /* Parse the universal commands first */
   if ( 0 == strncmp((const char *)pBuffer, "?", 1 ) ) {
      MENU_printHelp( msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "T", 1 ) ) {
      newNode = &menu;
      MENU_printCurrMenu( node, 0, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "P", 1 ) ) {
      MENU_printCurrMenu( newNode, 0, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "U", 1 ) ) {
      if ( NULL != node->fakeParentNode ) {
         newNode = node->fakeParentNode;
      } else {
         LOG_printf("Already at the top of the menu\n");
      }
      MENU_printCurrMenu( newNode, 0, msgSrc );
   } else {
      newNode = MENU_parseCurrLevelMenuItems(node, pBuffer, bufferLen, msgSrc);
   }
   return( newNode );
}

/******************************************************************************/
treeNode_t* MENU_parseCurrLevelMenuItems(
      treeNode_t *node,
      uint8_t *pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
)
{
   treeNode_t *menuNode = node;
   treeNode_t *childNode = NULL;

   if ( NULL == node ) {
      menuNode = &menu;
   } else if ( NULL != node->firstChildNode ) {
      menuNode = node;
      childNode = node->firstChildNode;
      while ( NULL != childNode ) {
         DBG_printf("Checking if %s matches %s\n",childNode->selector, pBuffer);
         if (0 == strncmp(childNode->selector, pBuffer, bufferLen-1)) {
            DBG_printf("Selecting %s\n", childNode->text);
            if ( NULL != childNode->actionToTake) {
               childNode->actionToTake();
            }

            /* If this current node is another menu, make sure to set the node
             * pointer to it since that's how MenuMgr keeps track of where in
             * the menu it is. */
            if ( NULL != childNode->firstChildNode ) {
               DBG_printf("Setting node to %s\n", childNode->text);
               menuNode = childNode;
               MENU_printCurrMenu(
                     menuNode,
                     KTREE_findDepth(menuNode, 0),
                     msgSrc
               );
            }
            return( menuNode );
         } else {
            childNode = childNode->firstSiblingNode;
         }
      }

      WRN_printf("Requested cmd '%s' not found in menu %s\n", pBuffer, menuNode->text);
   }

   return( menuNode );
}

/******************************************************************************/
void MENU_printHelp( MsgSrc whereToPrint )
{
   MENU_printf("******************************************************************************\n");
   MENU_printf("*****                           Menu Help                                *****\n");
   MENU_printf("***** Type the following commands:                                       *****\n");
   MENU_printf("***** '?': menu help                                                     *****\n");
   MENU_printf("***** 'P': print currently selected menu                                 *****\n");
   MENU_printf("***** 'T': go to the top of the menu                                     *****\n");
   MENU_printf("***** 'U': go up one level from where you're currently at                *****\n");
   MENU_printf("***** any identifier to select the submenu or menu item on your level    *****\n");
   MENU_printf("******************************************************************************\n");
}


/******************************************************************************/
static treeNode_t* MENU_initTopLevel( void )
{
   return( MENU_addChild(&menu, NULL, menu_TitleTxt, menu_SelectKey, NULL ) );
}

/******************************************************************************/
static treeNode_t* MENU_addSubMenu(
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
static treeNode_t* MENU_addMenuItem(
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
static treeNode_t* MENU_addChild(
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
static void MENU_findFakeAncestryPath( treeNode_t *node )
{
   if ( NULL == node ) {
      /* Just to avoid crashes. */
      ERR_printf("Node null. Something is probably wrong with the algorithm\n");
      return;
   } else if ( NULL != node->fakeParentNode ) {
      menuNav.pathToTopFake[menuNav.pathToTopFakeIndex] = node->fakeParentNode;
      menuNav.pathToTopFakeIndex++;
      MENU_findFakeAncestryPath( node->fakeParentNode );
   } else {
      /* No more parents.  Put index back one since that's the last spot we have
       * a pointer to a node */
      menuNav.pathToTopFakeIndex--;
      return;
   }
}

/******************************************************************************/
static void MENU_findTrueAncestryPath( treeNode_t *node )
{
   if ( NULL == node ) {
      /* Just to avoid crashes. */
      ERR_printf("Node null. Something is probably wrong with the algorithm\n");
      return;
   } else if ( NULL != node->trueParentNode ) {
      menuNav.pathToTopTrue[menuNav.pathToTopTrueIndex] = node->trueParentNode;
      menuNav.pathToTopTrueIndex++;
      MENU_findTrueAncestryPath( node->trueParentNode );
   } else {
      /* No more parents.  Put index back one since that's the last spot we have
       * a pointer to a node */
      menuNav.pathToTopTrueIndex--;
      return;
   }
}

/******************************************************************************/
static void MENU_clearAncestryPaths( void )
{
   memset( menuNav.pathToTopTrue, 0, sizeof(menuNav.pathToTopTrue) );
   menuNav.pathToTopTrueIndex = 0;
   memset( menuNav.pathToTopFake, 0, sizeof(menuNav.pathToTopFake) );
   menuNav.pathToTopFakeIndex = 0;
}

/******************************************************************************/
void MENU_printMenuExpandedAtCurrNode(
      treeNode_t *node,
      uint8_t level,
      MsgSrc whereToPrint
)
{
   MENU_clearAncestryPaths();                    /* Clear the ancestry paths. */

   /* Find the path to the top that is connected via trueParentNode pointers.
    * The search for true nodes will start from the first fakeParentNode since
    * we have to print the entire current level anyway */
   if (NULL != node && NULL != node->fakeParentNode ) {
      MENU_findTrueAncestryPath( node );
   }

   /* Sanity checks to avoid really bad memory out of bounds errors */
   if ( 0 == menuNav.pathToTopTrueIndex || menuNav.pathToTopTrueIndex > MENU_MAX_TRUE_DEPTH ) {
      ERR_printf("pathToTopIndex: %d\n", menuNav.pathToTopTrueIndex );
      return;
   }

   if ( NULL != menuNav.pathToTopTrue[ menuNav.pathToTopTrueIndex ] ) {
      MENU_printNode( menuNav.pathToTopTrue[ menuNav.pathToTopTrueIndex ], level, whereToPrint );
   }

   treeNode_t *currNode = menuNav.pathToTopTrue[menuNav.pathToTopTrueIndex]->firstChildNode;
   if ( NULL != currNode ) {
      /* Iterate through all the children nodes */

   }

}

/******************************************************************************/
void MENU_printParentMenus( treeNode_t *node, uint8_t level, MsgSrc whereToPrint )
{
   /* Clear out the menu navigation structure */


   /* Find the path from the current child node all the way to the top root of
    * the menu tree */
   MENU_findFakeAncestryPath( node );

   /* Start printing the menu from the very top level. */
   MENU_printNode( &menu, level, whereToPrint );

   /* Iterate through the immediate children of the top most node and print them */
   if ( NULL == menu.firstChildNode ) {
      return;
   }
   treeNode_t *currNode = &menu.firstChildNode;
   ++level; /* We are now one level below the top */
//   while( NULL != currNode ) {
//      MENU_printNode( currNode, level, whereToPrint );
//      if ( menuNav.pathToTop[ menuNav.pathToTopIndex ] == currNode ) {
//
//      }
//   }

   MENU_printMenuCurrLevel( node->firstSiblingNode, level, whereToPrint );

//   MENU_printNode( node, level, whereToPrint );
//
//   if ( NULL != node->firstSiblingNode ) {
//      //      dbg_slow_printf("Sibling exits, moving right\n");
//      MENU_printMenuCurrLevel( node->firstSiblingNode, level, whereToPrint );
//   }
}


/******************************************************************************/
void MENU_printCurrMenu( treeNode_t *node, uint8_t level, MsgSrc whereToPrint )
{
   if (NULL == node ) {
      return;
   }

   MENU_printNode( node, level, whereToPrint );

   if (NULL != node->firstChildNode ) {
      MENU_printMenuCurrLevel( node->firstChildNode, level+1, whereToPrint );
   }
}


/******************************************************************************/
void MENU_printMenuCurrLevel( treeNode_t *node, uint8_t level, MsgSrc whereToPrint )
{
   if ( NULL == node ) {
      return;
   } else {
      MENU_printNode( node, level, whereToPrint );
   }

   if ( NULL != node->firstSiblingNode ) {
      //      dbg_slow_printf("Sibling exits, moving right\n");
      MENU_printMenuCurrLevel( node->firstSiblingNode, level, whereToPrint );
   }
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
      MENU_printf("   ");
   }
   MENU_printf("*--");
   MENU_printf("** %-3s **:  %-50s\n", node->selector, node->text );
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
