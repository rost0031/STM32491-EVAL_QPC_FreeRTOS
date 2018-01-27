/**
 * @file    ktree.h
 * @brief   Implementation of a statically allocated k-ary tree structure that
 * is used by the menu.
 *
 * @date    09/29/2014
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * # Design of the KTREE algorithm
 * The Menu is built around a k-ary tree (k-tree) which is implemented as a
 * binary tree (b-tree). Unlike a regular k-tree where a node can have multiple
 * children which would make sense for a menu system, since the system is memory
 * and resource constrained, a binary tree makes more since.  It allows for
 * statically allocated nodes without using fixed sized arrays to store child
 * nodes. This implementation is a little confusing at first but is actually
 * used in several resource sensitive systems such as the Linux kernel.
 *
 * A regular k-tree is represented as:
 * struct t {
 *    int n;
 *    int numKids;
 *    struct t **kids;
 * }
 *
 * and the resulting tree struture looks like:
 *
 *  TOP
 *  |
 *  [ list of kids ]
 *  |     |      |
 *  k1    k2     k3
 *  |
 *  |
 *  [list of kids]
 *  |     |     |
 *  k1.1  k1.2  k1.3
 *
 * This representation works if you have things like malloc but on an embedded
 * system where using malloc is anathema, if you want to add another child, use
 * a static array of limited length in addition to having to record the number
 * of kids in the array in the structure.
 *
 * Instead, the following structure is used:
 * struct t {
 *    int n;
 *    struct t *firstChildNode;
 *    struct t *firstSiblingNode;
 *    struct t *trueParentNode;
 *    struct t *fakeParentNode;
 * };
 *
 * and the tree representation looks like:
 *
 * ___________
 * [TOP NODE ] <-
 * -----------   \
 * ^|  ^          \
 * ||  |           \ fP
 * ||  |tP, fP      \
 * ||  |_______  fS  \__________     fS
 * ||  [NODE 2]------>[NODE 2.1]---------->NULL
 * ||  ------- <------ ---------
 * ||       |      tP      |
 * ||       |fC            |fC
 * ||fC     V              V
 * ||      NULL          NULL
 * ||
 * |tP, fP
 * ||
 * |V_____
 * [NODE1]
 * -------
 *    ^
 *    |tP, fp
 *    |_________   fS
 *    [NODE 1.1] -----> NULL
 *        |
 *        |fC
 *        |
 *        V
 *      NULL
 *
 * Where:
 * fP = fakeParentNode - this pointer is used to figure out where the actual
 * parent as it would appear in a classical k-ary tree.
 * tP = trueParentNode - this pointer is used to show where this node is actually
 * attached since it could be either a child node of another node or a sibling.
 * fC = firstChildNode - pointer to the first child node.
 * fS = firstSiblingNode - pointer to the first sibling node.
 *
 * From any node, you can only access the first child or the first sibling,
 * making this a classical b-tree. From that first child, you can access it's
 * children and siblings.  Likewise for the first sibling pointer. This way all
 * the nodes are stored in a list which can be comprised of statically allocated
 * structures.  The true and fake Parent pointers are for easily tracing the
 * ancestry of each child node.  The fake parent pointer is used to map a
 * classical k-tree structure over this b-tree.
 *
 * There are several advantages to this seemingly confusing approach:
 * - Each node is a fixed size and can be statically allocated.
 * - No arrays are needed for representing child nodes.
 * - Many algorithms can be easily expressed since it's just a b-tree.
 *
 * This implementation of the menu is limited in that the functionality to remove
 * nodes is not present since this doesn't seem to be a necessary feature for a
 * menu.  To add a menu or menu item is very simple.  See example in menu_top.c
 *
 * This design was inspired by
 * https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KTREE_H_
#define KTREE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef void (*pMenuFunction) (
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

typedef struct treeNode {
   struct treeNode *fakeParentNode;
   struct treeNode *trueParentNode;
   struct treeNode *firstChildNode;
   struct treeNode *firstSiblingNode;
   char *text;
   char *selector;
   pMenuFunction actionToTake;
} treeNode_t;


/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/**
 * @brief   Initialize the menu memory space with the contents of the menu
 *
 * This function initializes the menu application.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to initialize
 *    @arg
 * @return: None
 */
void KTREE_nodeCtor( treeNode_t *node );

/**
 * @brief   Counts the depth (via fakeParentNode pointer) of the passed in
 * pointer to a node.
 *
 * Recursive function that counts it's own depth by traversing the tree up via
 * the fakeParentNode pointer and counting.
 *
 * @param [in]  *node: treeNode_t pointer to the node
 * @parem [in] currDepth: uint8_t value of the current depth.  Unless you want
 * to for some reason offset where you start counting from, pass in 0 here.
 * @return: uint8_t depth of the current node.
 */
uint8_t KTREE_findDepth( treeNode_t *node, uint8_t currDepth );

/**
 * @brief   Adds a child node to a passed in parent node.
 *
 * This function adds a child node to a passed in parent node after doing some
 * initial checking of both nodes.
 *
 * @param [in] childNode: treeNode_t* pointer to the node to be added as a child
 * @parem [in] trueParentNone: treeNode_t* pointer to the node that the child
 * node will be directly attached to because it may appear as a sibling node or
 * an actual child node.
 * @parem [in] fakeParentNone: treeNode_t* pointer to the node that indicates
 * the "regular" tree depth of this child node.
 * @return: None
 */
void KTREE_addChild(
      treeNode_t *childToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
);

/**
 * @brief   Adds a sibling node to a passed in parent node.
 *
 * This function adds a sibling node to a passed in parent node by checking if
 * the child node of the trueParentNode is used up and if it is, it recursively
 * calls itself with the used node as a trueParentNode parameter until it finds
 * the last sibling.  It then adds the sibling node there and sets the
 * fakeParentNode.
 *
 * @param [in] childNode: treeNode_t* pointer to the node to be added as a child
 * @parem [in] trueParentNone: treeNode_t* pointer to the node that the child
 * node will be directly attached to because it may appear as a sibling node or
 * an actual child node.
 * @parem [in] fakeParentNone: treeNode_t* pointer to the node that indicates
 * the "regular" tree depth of this child node.
 * @return: None
 */
void KTREE_addNextSibling(
      treeNode_t *siblingToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
);

void KTREE_printTree( treeNode_t *node, uint8_t level );

void KTREE_printNode( treeNode_t *node, uint8_t level );

void KTREE_fakeMenuTest( void );

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                            /* KTREE_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
