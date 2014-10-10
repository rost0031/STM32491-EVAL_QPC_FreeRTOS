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
char *const menu_SelectKey = "T";

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
static void MENU_clearAncestryPaths( void );
static void MENU_printRevAncestry( treeNode_t *node, MsgSrc whereToPrint );

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
      MENU_printCurrMenu( newNode, msgSrc );
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
               MENU_printMenuExpandedAtCurrNode( menuNode, msgSrc );
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
   }

   /* Node is valid.  Add it to the ancestry */
   menuNav.pathToTopFake[menuNav.pathToTopFakeIndex++] = node;

   /* If parent exists, recursively call ourselves again with the parent node */
   if ( NULL != node->fakeParentNode ) {
      MENU_findFakeAncestryPath( node->fakeParentNode );
   } else {
      /* No more parents.  Put index back one since that's the last spot we have
       * a pointer to a node.  Make sure to not underflow the index since passing
       * in the root node would do that. */
      if ( menuNav.pathToTopFakeIndex > 0 ) {
         menuNav.pathToTopFakeIndex--;
      }
      return;
   }
}

/******************************************************************************/
static void MENU_clearAncestryPaths( void )
{
   memset( menuNav.pathToTopFake, 0, sizeof(menuNav.pathToTopFake) );
   menuNav.pathToTopFakeIndex = 0;
}

/******************************************************************************/
void MENU_printMenuExpandedAtCurrNode(
      treeNode_t *node,
      MsgSrc whereToPrint
)
{
   /* Clear out the menu navigation structure */
   MENU_clearAncestryPaths();                    /* Clear the ancestry paths. */

   /* Find the path from the current child node all the way to the top root of
    * the menu tree */
   MENU_findFakeAncestryPath( node );

   /* Make sure the last node found in the ancestry is the root node.  Something
    * is terribly wrong if it's not. */
   if ( &menu != menuNav.pathToTopFake[menuNav.pathToTopFakeIndex] ) {
      ERR_printf("Top node in ancestry is not the root of the menu!\n");
      ERR_printf(
            "top node: %s and node is %s\n",
            menuNav.pathToTopFake[menuNav.pathToTopFakeIndex]->text,
            menu.text
      );
   }

   /* Use a recursive function to print the entire ancestry */
   MENU_printRevAncestry( &menu, whereToPrint );
}

/******************************************************************************/
static void MENU_printRevAncestry( treeNode_t *node, MsgSrc whereToPrint )
{
   /* Underflow check. */
   if ( menuNav.pathToTopFakeIndex > MENU_MAX_FAKE_DEPTH ) {
      ERR_printf(
            "Ancestry index underflowed: %d\n",
            menuNav.pathToTopFakeIndex
      );
      return;
   }

   /* Iterate through the immediate children of the top most node and print them */
   while( NULL != node ) {
      MENU_printNode( node, whereToPrint );

      if ( menuNav.pathToTopFake[ menuNav.pathToTopFakeIndex ] == node ) {
         /* If the ancestry node and the current node match, recurse one level
          * lower unless this is the last node in the ancestry.  In that case,
          * print its children and return */

         if ( NULL == node->firstChildNode ) {
            ERR_printf("No child node\n");
            return;
         }

         if ( 0 == menuNav.pathToTopFakeIndex ) {
            /* Last ancestry node found */
            MENU_printMenuCurrLevel(
                  node->firstChildNode,
                  whereToPrint
            );
            return;
         } else if (0 != menuNav.pathToTopFakeIndex ) {
//            DBG_printf("Ancestry index %d\n", menuNav.pathToTopFakeIndex);
            /* Matched an ancestry node but it's not the last one.  Recurse but
             * make sure to decrement the ancestry index */
            menuNav.pathToTopFakeIndex--;
            MENU_printRevAncestry( node->firstChildNode, whereToPrint );
            node = node->firstSiblingNode;
         }
      } else {
         node = node->firstSiblingNode;
      }
   }
}

/******************************************************************************/
void MENU_printCurrMenu( treeNode_t *node, MsgSrc whereToPrint )
{
   if (NULL == node ) {
      return;
   }

   MENU_printNode( node, whereToPrint );

   if (NULL != node->firstChildNode ) {
      MENU_printMenuCurrLevel( node->firstChildNode, whereToPrint );
   }
}


/******************************************************************************/
void MENU_printMenuCurrLevel( treeNode_t *node, MsgSrc whereToPrint )
{
   if ( NULL == node ) {
      return;
   } else {
      MENU_printNode( node, whereToPrint );
   }

   if ( NULL != node->firstSiblingNode ) {
      MENU_printMenuCurrLevel( node->firstSiblingNode, whereToPrint );
   }
}

/******************************************************************************/
void MENU_printMenuTree( treeNode_t *node, MsgSrc whereToPrint )
{
   if ( NULL == node ) {
      return;
   } else {
      MENU_printNode( node, whereToPrint );
   }

   if ( NULL != node->firstChildNode ) {
      //      dbg_slow_printf("Child exists, descending one level\n");
      MENU_printMenuTree( node->firstChildNode, whereToPrint );
   }

   if ( NULL != node->firstSiblingNode ) {
      //      dbg_slow_printf("Sibling exits, moving right\n");
      MENU_printMenuTree( node->firstSiblingNode, whereToPrint );
   }
}

/******************************************************************************/
void MENU_printNode( treeNode_t *node, MsgSrc whereToPrint )
{
   uint8_t level = KTREE_findDepth( node, 0);
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
