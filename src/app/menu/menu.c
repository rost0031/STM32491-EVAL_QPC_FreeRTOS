/**
 * @file    menu.c
 * @brief   Implementation of the menu application using a statically allocated
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

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief   Storage for ancestry navigation through the menu.
 * Used to find ancestry for printing out the menu expanded at the selected node.
 */
menuNav_t menuNav;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief   Add a node to the menu.
 * This function is a wrapper around the KTREE functionality to add a node to
 * the existing tree of nodes.
 *
 * @param [in|out] node: treeNode_t* pointer to the node that is to be added.
 * @param [in|out] parent: treeNode_t* pointer to the node where the node will
 * be added.
 * @param [in] menuTxt: const char* pointer to the storage of the text that will
 * be pointed to by the node being added.
 * @param [in] menuSelectTxt: const char* pointer to the storage of the selector
 * text that will be pointed by the node being added.
 * @param [in] pMenuFunction function pointer to the action function that will be
 * pointed by the node being added.  This action is what will execute when that
 * menu item (or submenu) is selected.  Can be NULL if no action is to be taken.
 * @return: treeNode_t* pointer to the newly added node.
 */
static treeNode_t* MENU_addChild(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
);

/**
 * @brief   Find and record the path from the current node to the root of the
 * tree.
 *
 * This function is used to find the "ancestry" of the current node in order to
 * print only the relevant part of the tree expanded around the current node.
 * (See MENU_printMenuExpandedAtCurrNode() documentation).  It traverses the
 * "fakeParentNode" pointers of the current node all the way to the root of the
 * menu tree and records them in a static array.
 *
 * @param [in] node: a treeNode_t* pointer to the node from which to start the
 * search from.
 * @return: None
 */
static void MENU_findAncestryPath( treeNode_t* node );

/**
 * @brief   Clears the internal array used for storage of ancestry.
 *
 * @note: see MENU_findAncestryPath() documentation.
 *
 * @param   None
 * @return: None
 */
static void MENU_clearAncestryPaths( void );

/**
 * @brief   Prints all the relevant parents of the currently selected node.
 *
 * This function is used to print the "ancestry" of the current node in order to
 * print only the relevant part of the tree expanded around the current node.
 * (See MENU_printMenuExpandedAtCurrNode() documentation).
 *
 * @param [in] node: a treeNode_t* pointer to the node from which to start the
 * printing from.
 * @param [in] msgSrc: MsgSrc var that specifies where to print to.
 *
 * @return: None
 */
static void MENU_printRevAncestry( treeNode_t* node, MsgSrc msgSrc );

/**
 * @brief   Prints the currently selected menu and all the items immediately
 * below it.
 *
 * @param [in] node: a treeNode_t* pointer to the node from which to start the
 * printing from.
 * @param [in] msgSrc: MsgSrc var that specifies where to print to.
 *
 * @return: None
 */
static void MENU_printMenuCurrLevel( treeNode_t* node, MsgSrc msgSrc );

/**
 * @brief   Prints the passed in node.
 *
 * @param [in] node: a treeNode_t* pointer to the node to print.
 * @param [in] msgSrc: MsgSrc var that specifies where to print to.
 *
 * @return: None
 */
static void MENU_printNode( treeNode_t* node, MsgSrc msgSrc );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void MENU_printMenuExpandedAtCurrNode( treeNode_t* node, MsgSrc msgSrc )
{

   if ( NULL == node ) {
      ERR_printf("Node null. Something is probably wrong with the algorithm\n");
      return;
   }

   /* Clear out the menu navigation structure */
   MENU_clearAncestryPaths();                    /* Clear the ancestry paths. */

   /* Find the path from the current child node all the way to the top root of
    * the menu tree */
   MENU_findAncestryPath( node );

   /* Make sure the last node found in the ancestry is the root node.  Something
    * is terribly wrong if it's not. */
   if ( NULL == menuNav.pathToTop[menuNav.pathToTopIndex] ) {
      ERR_printf("Incorrect ancestry calculation. Probably a bug in the algorithm\n");
      return;
   } else if ( NULL != menuNav.pathToTop[menuNav.pathToTopIndex]->fakeParentNode ) {
      ERR_printf("Incorrect ancestry calculation. Probably a bug in the algorithm\n");
      return;
   }
   /* Use a recursive function to print the entire ancestry */
   MENU_printf(msgSrc, "******************************************************************************\n");
   MENU_printRevAncestry( menuNav.pathToTop[menuNav.pathToTopIndex], msgSrc );
   MENU_printf(msgSrc, "******************************************************************************\n\n");
}

/******************************************************************************/
treeNode_t* MENU_parseCurrLevelMenuItems(
      treeNode_t* node,
      const char* pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
)
{
   treeNode_t *menuNode = node;
   treeNode_t *childNode = NULL;

   if ( NULL == node ) {
      ERR_printf("Passed in null node\n");
      return( NULL );
   } else if ( NULL != node->firstChildNode ) {
      menuNode = node;
      childNode = node->firstChildNode;
      while ( NULL != childNode ) {
//         DBG_printf("Checking if %s matches %s\n",childNode->selector, pBuffer);
         if (0 == strncmp(childNode->selector, pBuffer, bufferLen-1)) {
//            DBG_printf("Selecting %s\n", childNode->text);
            if ( NULL != childNode->actionToTake) {
               childNode->actionToTake( pBuffer, bufferLen, msgSrc );
            }

            /* If this current node is another menu, make sure to set the node
             * pointer to it since that's how MenuMgr keeps track of where in
             * the menu it is. */
            if ( NULL != childNode->firstChildNode ) {
//               DBG_printf("Setting node to %s\n", childNode->text);
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
treeNode_t* MENU_initTopLevel(
      treeNode_t* node,
      const char* menuTxt,
      const char* menuSelectTxt
)
{
   return( MENU_addChild(node, NULL, menuTxt, menuSelectTxt, NULL ) );
}

/******************************************************************************/
treeNode_t* MENU_addSubMenu(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
)
{
   return( MENU_addChild(node, parent, menuTxt, menuSelectTxt, action ) );
}

/******************************************************************************/
treeNode_t* MENU_addMenuItem(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
)
{
   return( MENU_addChild(node, parent, menuTxt, menuSelectTxt, action ) );
}

/******************************************************************************/
static treeNode_t* MENU_addChild(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
)
{
   /* 1. Initialize this submenu */
   KTREE_nodeCtor( node );                       /* Call the node constructor */
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
static void MENU_findAncestryPath( treeNode_t* node )
{
   if ( NULL == node ) {
      /* Just to avoid crashes. */
      ERR_printf("Node null. Something is probably wrong with the algorithm\n");
      return;
   }

   /* Node is valid.  Add it to the ancestry */
   menuNav.pathToTop[menuNav.pathToTopIndex++] = node;

   /* If parent exists, recursively call ourselves again with the parent node */
   if ( NULL != node->fakeParentNode ) {
      MENU_findAncestryPath( node->fakeParentNode );
   } else {
      /* No more parents.  Put index back one since that's the last spot we have
       * a pointer to a node.  Make sure to not underflow the index since passing
       * in the root node would do that. */
      if ( menuNav.pathToTopIndex > 0 ) {
         menuNav.pathToTopIndex--;
      }
      return;
   }
}

/******************************************************************************/
static void MENU_clearAncestryPaths( void )
{
   memset( menuNav.pathToTop, 0, sizeof(menuNav.pathToTop) );
   menuNav.pathToTopIndex = 0;
}



/******************************************************************************/
static void MENU_printRevAncestry( treeNode_t* node, MsgSrc msgSrc )
{
   /* Underflow check. */
   if ( menuNav.pathToTopIndex > MENU_MAX_DEPTH ) {
      ERR_printf(
            "Ancestry index underflowed: %d\n",
            menuNav.pathToTopIndex
      );
      return;
   }

   /* Iterate through the children of the top most node and print them */
   while( NULL != node ) {
//      DBG_printf("About to print node %08x\n", (uint32_t) node);
      MENU_printNode( node, msgSrc );
//      DBG_printf("pathToTopIndex: %d, node: %08x, menuNavNode: %08x\n", menuNav.pathToTopIndex, (uint32_t)node,  menuNav.pathToTop[ menuNav.pathToTopIndex ]);
      if ( menuNav.pathToTop[ menuNav.pathToTopIndex ] == node ) {
         /* If the ancestry node and the current node match, recurse one level
          * lower unless this is the last node in the ancestry.  In that case,
          * print its children and return */

         if ( NULL == node->firstChildNode ) {
            ERR_printf("No child node\n");
            return;
         }

         if ( 0 == menuNav.pathToTopIndex ) {
            /* Last ancestry node found */
            MENU_printMenuCurrLevel(
                  node->firstChildNode,
                  msgSrc
            );
         } else if (0 != menuNav.pathToTopIndex ) {
//            DBG_printf("Ancestry index %d\n", menuNav.pathToTopIndex);
            /* Matched an ancestry node but it's not the last one.  Recurse but
             * make sure to decrement the ancestry index */
            menuNav.pathToTopIndex--;
            MENU_printRevAncestry( node->firstChildNode, msgSrc );
            node = node->firstSiblingNode;
         }
      }

      if ( NULL != node && NULL != node->firstSiblingNode ) {
//         DBG_printf("setting node %08x to first sibling node, sibling node is %08x\n", (uint32_t)node, (uint32_t)node->firstSiblingNode);
         node = node->firstSiblingNode;
//         DBG_printf("set node to first sibling node\n");
      } else {
         return;
      }
   }
}

/******************************************************************************/
void MENU_printMenuTree( treeNode_t* node, MsgSrc msgSrc )
{
   if ( NULL == node ) {
      return;
   } else {
      MENU_printNode( node, msgSrc );
   }

   if ( NULL != node->firstChildNode ) {
      MENU_printMenuTree( node->firstChildNode, msgSrc );
   }

   if ( NULL != node->firstSiblingNode ) {
      MENU_printMenuTree( node->firstSiblingNode, msgSrc );
   }
}

/******************************************************************************/
static void MENU_printMenuCurrLevel( treeNode_t* node, MsgSrc msgSrc )
{
   if ( NULL == node ) {
      return;
   }

   MENU_printNode( node, msgSrc );

   if ( NULL != node->firstSiblingNode ) {
      MENU_printMenuCurrLevel( node->firstSiblingNode, msgSrc );
   }
}


/******************************************************************************/
static void MENU_printNode( treeNode_t* node, MsgSrc msgSrc )
{
   uint8_t level = KTREE_findDepth( node, 0);
   for ( uint8_t i = 0; i < level; i++ ) {
      MENU_printf(msgSrc, "   ");
   }
   MENU_printf(msgSrc, "*--");
   MENU_printf(msgSrc, "** %-3s **: %s\n", node->selector, node->text );
}

/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
