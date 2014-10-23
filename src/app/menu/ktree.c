/**
 * @file    ktree.c
 * @brief   Implementation of a statically allocated k-ary tree structure that
 * is used by the menu.
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
#include "ktree.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static treeNode_t Menu1;
char *const Menu1Txt = "Menu 1";

   static treeNode_t MenuItem1;
   char *const MenuItem1_1Txt = "Menu Item 1.1";

   static treeNode_t MenuItem2;
   char *const MenuItem1_2Txt = "Menu Item 1.2";

   static treeNode_t MenuItem3;
   char *const MenuItem1_3Txt = "Menu Item 1.3";

   static treeNode_t MenuItem4;
   char *const MenuItem1_4Txt = "Menu Item 1.4";

static treeNode_t Menu2;
char *const Menu2Txt = "Menu 2";

   static treeNode_t MenuItem2_1;
   char *const MenuItem2_1Txt = "Menu Item 2.1";

   static treeNode_t MenuItem2_2;
   char *const MenuItem2_2Txt = "Menu Item 2.2";

   static treeNode_t MenuItem2_3;
   char *const MenuItem2_3Txt = "Menu Item 2.3";

   static treeNode_t MenuItem2_4;
   char *const MenuItem2_4Txt = "Menu Item 2.4";

static treeNode_t Menu3;
char *const Menu3Txt = "Menu 3";

   static treeNode_t Menu3_1;
   char *const Menu3_1Txt = "Menu 3_1";

      static treeNode_t MenuItem3_1_1;
      char *const MenuItem3_1_1Txt = "Menu Item 3.1.1";

      static treeNode_t MenuItem3_1_2;
      char *const MenuItem3_1_2Txt = "Menu Item 3.1.2";

      static treeNode_t MenuItem3_1_3;
      char *const MenuItem3_1_3Txt = "Menu Item 3.1.3";

      static treeNode_t MenuItem3_1_4;
      char *const MenuItem3_1_4Txt = "Menu Item 3.1.4";

   static treeNode_t Menu3_2;
   char *const Menu3_2Txt = "Menu 3_2";

      static treeNode_t MenuItem3_2_1;
      char *const MenuItem3_2_1Txt = "Menu Item 3.2.1";

      static treeNode_t MenuItem3_2_2;
      char *const MenuItem3_2_2Txt = "Menu Item 3.2.2";

      static treeNode_t MenuItem3_2_3;
      char *const MenuItem3_2_3Txt = "Menu Item 3.2.3";

      static treeNode_t MenuItem3_2_4;
      char *const MenuItem3_2_4Txt = "Menu Item 3.2.4";

   static treeNode_t Menu3_3;
   char *const Menu3_3Txt = "Menu 3_3";

      static treeNode_t Menu3_3_1;
      char *const Menu3_3_1Txt = "Menu 3_3_1";

         static treeNode_t MenuItem3_3_1_1;
         char *const MenuItem3_3_1_1Txt = "Menu Item 3.3.1.1";

         static treeNode_t MenuItem3_3_1_2;
         char *const MenuItem3_3_1_2Txt = "Menu Item 3.3.1.2";

         static treeNode_t MenuItem3_3_1_3;
         char *const MenuItem3_3_1_3Txt = "Menu Item 3.3.1.3";

      static treeNode_t Menu3_3_2;
      char *const Menu3_3_2Txt = "Menu 3_3_2";

         static treeNode_t MenuItem3_3_2_1;
         char *const MenuItem3_3_2_1Txt = "Menu Item 3.3.2.1";

         static treeNode_t MenuItem3_3_2_2;
         char *const MenuItem3_3_2_2Txt = "Menu Item 3.3.2.2";

         static treeNode_t MenuItem3_3_2_3;
         char *const MenuItem3_3_2_3Txt = "Menu Item 3.3.2.3";

      static treeNode_t Menu3_3_3;
      char *const Menu3_3_3Txt = "Menu 3_3_3";

         static treeNode_t MenuItem3_3_3_1;
         char *const MenuItem3_3_3_1Txt = "Menu Item 3.3.3.1";

         static treeNode_t MenuItem3_3_3_2;
         char *const MenuItem3_3_3_2Txt = "Menu Item 3.3.3.2";

         static treeNode_t MenuItem3_3_3_3;
         char *const MenuItem3_3_3_3Txt = "Menu Item 3.3.3.3";

      static treeNode_t Menu3_3_4;
      char *const Menu3_3_4Txt = "Menu 3_3_4";

         static treeNode_t MenuItem3_3_4_1;
         char *const MenuItem3_3_4_1Txt = "Menu Item 3.3.4.1";

         static treeNode_t MenuItem3_3_4_2;
         char *const MenuItem3_3_4_2Txt = "Menu Item 3.3.4.2";

         static treeNode_t MenuItem3_3_4_3;
         char *const MenuItem3_3_4_3Txt = "Menu Item 3.3.4.3";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void KTREE_nodeCtor( treeNode_t *node )
{
   /* Initialize the root node */
   node->firstChildNode = NULL;
   node->firstSiblingNode = NULL;
   node->fakeParentNode = NULL;
   node->trueParentNode = NULL;
}

/******************************************************************************/
uint8_t KTREE_findDepth( treeNode_t *node, uint8_t currDepth )
{
   /* Make sure the node is not null.  If this happens, it's a bug. */
   if ( NULL == node ) {
      ERR_printf("!!!Node is null\n");
      return( 0 );
   }

   if ( NULL == node->fakeParentNode ) {
      /* If no fake parents exist, this is the top.  Return the calculated
       * depth */
      return( currDepth );
   } else {
      /* Parent exists; recurse after incrementing the depth */
      return (KTREE_findDepth( node->fakeParentNode, ++currDepth ));
   }
}

/******************************************************************************/
void KTREE_addChild(
      treeNode_t *childToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
)
{
   /* Find the parent node */
   if ( NULL == trueParentNode ) {
      childToAdd->trueParentNode = NULL;
      childToAdd->fakeParentNode = NULL;
      return;
   } else if ( NULL == trueParentNode->firstChildNode ) {

      /* If the node where we want to add a child currently has no children,
       * add the child node here to the childNode field. */
      trueParentNode->firstChildNode = childToAdd;
//      dbg_slow_printf("whereToAddChild=0x%08x\n", (uint32_t )whereToAddChild);
   } else {
      /* Otherwise, add a sibling to the child of this node */
      KTREE_addNextSibling( childToAdd, trueParentNode->firstChildNode, fakeParentNode);
//      dbg_slow_printf("Adding a sibling to the child of node '%s'\n", trueParentNode->text);
   }
}

/******************************************************************************/
void KTREE_addNextSibling(
      treeNode_t *siblingToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
)
{
   if( NULL == trueParentNode->firstSiblingNode ) {
      /* In the node being added, set its parents. */
      siblingToAdd->fakeParentNode = fakeParentNode;
      siblingToAdd->trueParentNode = trueParentNode;

      /* Set the empty firstSiblingNode pointer to new node being added. */
      trueParentNode->firstSiblingNode = siblingToAdd;
//      dbg_slow_printf("Added node '%s' as a sibling to node '%s'\n", siblingToAdd->text, trueParentNode->text);
   } else {
//      dbg_slow_printf("Sibling '%s' already exists here.  Trying next one\n", trueParentNode->text);
      KTREE_addNextSibling(siblingToAdd, trueParentNode->firstSiblingNode, fakeParentNode);
   }
}

/******************************************************************************/
void KTREE_printTree( treeNode_t *node, uint8_t level )
{
   if ( NULL == node ) {
//      dbg_slow_printf("Node at level %d is null, not printing\n", level);
      return;
   } else {
      KTREE_printNode( node, level );
   }

   if ( NULL != node->firstChildNode ) {
//      dbg_slow_printf("Child exists, descending one level\n");
      KTREE_printTree( node->firstChildNode, level+1 );
   }

   if ( NULL != node->firstSiblingNode ) {
//      dbg_slow_printf("Sibling exits, moving right\n");
      KTREE_printTree( node->firstSiblingNode, level );
   }

}

/******************************************************************************/
void KTREE_printNode( treeNode_t *node, uint8_t level )
{
   for ( uint8_t i = 0; i < level; i++ ) {
      printf("*--");
   }
   printf("NodeText: %s\n", node->text);
   for ( uint8_t i = 0; i < level; i++ ) {
      printf("*--");
   }
   printf("True Parent   pointer: 0x%08x\n", (uint32_t)node->trueParentNode);
   for ( uint8_t i = 0; i < level; i++ ) {
      printf("*--");
   }
   printf("Fake Parent   pointer: 0x%08x\n", (uint32_t)node->fakeParentNode);
   for ( uint8_t i = 0; i < level; i++ ) {
      printf("*--");
   }
   printf("First Sibling pointer: 0x%08x\n", (uint32_t)node->firstSiblingNode);
   for ( uint8_t i = 0; i < level; i++ ) {
      printf("*--");
   }
   printf("First Child   pointer: 0x%08x\n", (uint32_t)node->firstChildNode);
}

/******************************************************************************/
void KTREE_fakeMenuTest( void )
{
   /* First Node */
   KTREE_nodeCtor( &Menu1 );
   Menu1.text = Menu1Txt;

      /* Add a child node */
      KTREE_nodeCtor( &MenuItem1 );
      MenuItem1.text = MenuItem1_1Txt;
      KTREE_addChild( &MenuItem1, &Menu1, &Menu1 );

      /* Add siblings to that child node */
      KTREE_nodeCtor( &MenuItem2 );
      MenuItem2.text = MenuItem1_2Txt;
      KTREE_addChild( &MenuItem2, &Menu1, &Menu1 );

      KTREE_nodeCtor( &MenuItem3 );
      MenuItem3.text = MenuItem1_3Txt;
      KTREE_addChild( &MenuItem3, &Menu1, &Menu1 );

      KTREE_nodeCtor( &MenuItem4 );
      MenuItem4.text = MenuItem1_4Txt;
      KTREE_addChild( &MenuItem4, &Menu1, &Menu1 );

   /* Add another node at top level - it gets added as a sibling to the first node */
   KTREE_nodeCtor( &Menu2 );
   Menu2.text = Menu2Txt;
   KTREE_addNextSibling( &Menu2, &Menu1, NULL );

      /* Add menu items to it as children  */
      KTREE_nodeCtor( &MenuItem2_1 );
      MenuItem2_1.text = MenuItem1_1Txt;
      KTREE_addChild( &MenuItem2_1, &Menu2, &Menu2 );

      /* Add siblings to that child node */
      KTREE_nodeCtor( &MenuItem2_2 );
      MenuItem2_2.text = MenuItem2_2Txt;
      KTREE_addChild( &MenuItem2_2, &Menu2, &Menu2 );

      KTREE_nodeCtor( &MenuItem2_3 );
      MenuItem2_3.text = MenuItem2_3Txt;
      KTREE_addChild( &MenuItem2_3, &Menu2, &Menu2 );

      KTREE_nodeCtor( &MenuItem2_4 );
      MenuItem2_4.text = MenuItem2_4Txt;
      KTREE_addChild( &MenuItem2_4, &Menu2, &Menu2 );

   /* Add another node at top level - it gets added as a sibling to the first node */
   KTREE_nodeCtor( &Menu3 );
   Menu3.text = Menu3Txt;
   KTREE_addNextSibling( &Menu3, &Menu1, NULL );

      /* Add submenu nodes - it gets added as a sibling to the first node */
      KTREE_nodeCtor( &Menu3_1 );
      Menu3_1.text = Menu3_1Txt;
      KTREE_addChild( &Menu3_1, &Menu3, &Menu3 );

         /* Add menu items to it as children  */
         KTREE_nodeCtor( &MenuItem3_1_1 );
         MenuItem3_1_1.text = MenuItem3_1_1Txt;
         KTREE_addChild( &MenuItem3_1_1, &Menu3_1, &Menu3_1 );

         KTREE_nodeCtor( &MenuItem3_1_2 );
         MenuItem3_1_2.text = MenuItem3_1_2Txt;
         KTREE_addChild( &MenuItem3_1_2, &Menu3_1, &Menu3_1 );

         KTREE_nodeCtor( &MenuItem3_1_3 );
         MenuItem3_1_3.text = MenuItem3_1_3Txt;
         KTREE_addChild( &MenuItem3_1_3, &Menu3_1, &Menu3_1 );

         KTREE_nodeCtor( &MenuItem3_1_4 );
         MenuItem3_1_4.text = MenuItem3_1_4Txt;
         KTREE_addChild( &MenuItem3_1_4, &Menu3_1, &Menu3_1 );

      KTREE_nodeCtor( &Menu3_2 );
      Menu3_2.text = Menu3_2Txt;
      KTREE_addChild( &Menu3_2, &Menu3, &Menu3 );

         /* Add menu items to it as children  */
         KTREE_nodeCtor( &MenuItem3_2_1 );
         MenuItem3_2_1.text = MenuItem3_2_1Txt;
         KTREE_addChild( &MenuItem3_2_1, &Menu3_2, &Menu3_2 );

         KTREE_nodeCtor( &MenuItem3_2_2 );
         MenuItem3_2_2.text = MenuItem3_2_2Txt;
         KTREE_addChild( &MenuItem3_2_2, &Menu3_2, &Menu3_2 );

         KTREE_nodeCtor( &MenuItem3_2_3 );
         MenuItem3_2_3.text = MenuItem3_2_3Txt;
         KTREE_addChild( &MenuItem3_2_3, &Menu3_2, &Menu3_2 );

         KTREE_nodeCtor( &MenuItem3_2_4 );
         MenuItem3_2_4.text = MenuItem3_2_4Txt;
         KTREE_addChild( &MenuItem3_2_4, &Menu3_2, &Menu3_2 );

      KTREE_nodeCtor( &Menu3_3 );
      Menu3_3.text = Menu3_3Txt;
      KTREE_addChild( &Menu3_3, &Menu3, &Menu3 );

         KTREE_nodeCtor( &Menu3_3_1 );
         Menu3_3_1.text = Menu3_3_1Txt;
         KTREE_addChild( &Menu3_3_1, &Menu3_3, &Menu3_3 );

            /* Add menu items to it as children  */
            KTREE_nodeCtor( &MenuItem3_3_1_1 );
            MenuItem3_3_1_1.text = MenuItem3_3_1_1Txt;
            KTREE_addChild( &MenuItem3_3_1_1, &Menu3_3_1, &Menu3_3_1 );

            KTREE_nodeCtor( &MenuItem3_3_1_2 );
            MenuItem3_3_1_2.text = MenuItem3_3_1_2Txt;
            KTREE_addChild( &MenuItem3_3_1_2, &Menu3_3_1, &Menu3_3_1 );

            KTREE_nodeCtor( &MenuItem3_3_1_3 );
            MenuItem3_3_1_3.text = MenuItem3_3_1_3Txt;
            KTREE_addChild( &MenuItem3_3_1_3, &Menu3_3_1, &Menu3_3_1 );

         KTREE_nodeCtor( &Menu3_3_2 );
         Menu3_3_2.text = Menu3_3_2Txt;
         KTREE_addChild( &Menu3_3_2, &Menu3_3, &Menu3_3 );

            /* Add menu items to it as children  */
            KTREE_nodeCtor( &MenuItem3_3_2_1 );
            MenuItem3_3_2_1.text = MenuItem3_3_2_1Txt;
            KTREE_addChild( &MenuItem3_3_2_1, &Menu3_3_2, &Menu3_3_2 );

            KTREE_nodeCtor( &MenuItem3_3_2_2 );
            MenuItem3_3_2_2.text = MenuItem3_3_2_2Txt;
            KTREE_addChild( &MenuItem3_3_2_2, &Menu3_3_2, &Menu3_3_2 );

            KTREE_nodeCtor( &MenuItem3_3_2_3 );
            MenuItem3_3_2_3.text = MenuItem3_3_2_3Txt;
            KTREE_addChild( &MenuItem3_3_2_3, &Menu3_3_2, &Menu3_3_2 );

         KTREE_nodeCtor( &Menu3_3_3 );
         Menu3_3_3.text = Menu3_3_3Txt;
         KTREE_addChild( &Menu3_3_3, &Menu3_3, &Menu3_3 );

            /* Add menu items to it as children  */
            KTREE_nodeCtor( &MenuItem3_3_3_1 );
            MenuItem3_3_3_1.text = MenuItem3_3_3_1Txt;
            KTREE_addChild( &MenuItem3_3_3_1, &Menu3_3_3, &Menu3_3_3 );

            KTREE_nodeCtor( &MenuItem3_3_3_2 );
            MenuItem3_3_3_2.text = MenuItem3_3_3_2Txt;
            KTREE_addChild( &MenuItem3_3_3_2, &Menu3_3_3, &Menu3_3_3 );

            KTREE_nodeCtor( &MenuItem3_3_3_3 );
            MenuItem3_3_3_3.text = MenuItem3_3_3_3Txt;
            KTREE_addChild( &MenuItem3_3_3_3, &Menu3_3_3, &Menu3_3_3 );

         KTREE_nodeCtor( &Menu3_3_4 );
         Menu3_3_4.text = Menu3_3_4Txt;
         KTREE_addChild( &Menu3_3_4, &Menu3_3, &Menu3_3 );

            /* Add menu items to it as children  */
            KTREE_nodeCtor( &MenuItem3_3_4_1 );
            MenuItem3_3_4_1.text = MenuItem3_3_4_1Txt;
            KTREE_addChild( &MenuItem3_3_4_1, &Menu3_3_4, &Menu3_3_4 );

            KTREE_nodeCtor( &MenuItem3_3_4_2 );
            MenuItem3_3_4_2.text = MenuItem3_3_4_2Txt;
            KTREE_addChild( &MenuItem3_3_4_2, &Menu3_3_4, &Menu3_3_4 );

            KTREE_nodeCtor( &MenuItem3_3_4_3 );
            MenuItem3_3_4_3.text = MenuItem3_3_4_3Txt;
            KTREE_addChild( &MenuItem3_3_4_3, &Menu3_3_4, &Menu3_3_4 );

   KTREE_printTree( &Menu1, 0 );
}


/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
