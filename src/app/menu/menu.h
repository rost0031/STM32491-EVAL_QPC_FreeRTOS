/**
 * @file    menu.h
 * @brief   Implementation of the munu application using a statically allocated
 * k-ary tree structure.
 *
 * @date    09/29/2014
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MENU_H_
#define MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "ktree.h"
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
#define MENU_MAX_DEPTH                       20

/* Exported types ------------------------------------------------------------*/
/**
 * @brief   Structure to keep track of where we are in the menu tree and how
 * to get to the topmost node.
 */
typedef struct menuNavigationTag {
   /**< Array of pointers to treeNode_t types that will hold the path from
    * the currently selected node to the topmost parent via the fakeParentNode
    * pointers. This array is arranged from the child to the top so has to be
    * traversed backwards to get back to the child. */
   treeNode_t* pathToTop[MENU_MAX_DEPTH];
   /**< Index into the pathToTop array. */
   uint8_t pathToTopIndex;
} menuNav_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Prints out the menu expanded around the current node.
 *
 * This function prints the all the parent nodes of the currently selected menu
 * or menu item, the selected menu, and all the siblings.  For example, if the
 * menu tree looks like this:
 *
 * Root of Menu
 *    Menu 1
 *       Menu 1.1
 *          Menu Item 1.1.1
 *          Menu Item 1.1.2
 *          Menu Item 1.1.3
 *       Menu 1.2
 *          Menu Item 1.2.1
 *          Menu Item 1.2.2
 *          Menu Item 1.2.3
 *       Menu 1.3
 *          Menu Item 1.3.1
 *          Menu Item 1.3.2
 *          Menu Item 1.3.3
 *    Menu 2
 *       Menu 2.1
 *          Menu Item 2.1.1
 *          Menu Item 2.1.2
 *          Menu Item 2.1.3
 *       Menu 2.2
 *          Menu Item 2.2.1
 *          Menu Item 2.2.2
 *          Menu Item 2.2.3
 *       Menu 2.3
 *          Menu Item 2.3.1
 *          Menu Item 2.3.2
 *          Menu Item 2.3.3
 *
 * and menu 1.2 is currently selected, this function will print
 *
 * Root of Menu
 *    Menu 1
 *       Menu 1.1
 *       Menu 1.2
 *          Menu Item 1.2.1
 *          Menu Item 1.2.2
 *          Menu Item 1.2.3
 *       Menu 1.3
 *    Menu 2
 *
 * @param [in] msgSrc: MsgSrc var that specifies where to print out to.
 * @return  None
 */
void MENU_printMenuExpandedAtCurrNode( treeNode_t* node, MsgSrc msgSrc );

/**
 * @brief   Initializes the top level of the menu.
 * @param [in|out] *node: treeNode_t pointer to the node that will become the root node.
 * @param [in] *menuTxt: const char pointer to the storage of the text that will
 * be pointed to by the node being initialized.
 * @param [in] *menuSelectTxt: const char pointer to the storage of the selector
 * text that will be pointed by the node being initialized.
 * @return  treeNode_t*: pointer to the top level node of the menu.
 */
treeNode_t* MENU_initTopLevel(
      treeNode_t* node,
      const char* menuTxt,
      const char* menuSelectTxt
);

/**
 * @brief   Parse the raw data of the command coming to the menu only for the
 * currently selected menu.
 *
 * This function is called by the initial MENU_parse() function after it has
 * determined that the commands are not universal (apply to the entire menu) and
 * should be parsed only by the level of the menu that is currently selected. If
 * the select key passed in matches a sub-menu or a menu item at the current
 * level, that item will be selected and its action executed, if exists.  If the
 * node being selected has no children (it's a leaf node), the currently
 * selected node stays the same since it makes no sense to descend into a leaf
 * node.  Otherwise, the returned node is the child node of the current node.
 *
 * @param [in|out] *node: treeNode_t pointer to the submenu currently pointed to
 * @param [in] pBuffer: const char*  pointer to the storage of the buffer where the
 * command data resides
 * @param [in] bufferLen: uint16_t length of the data in the command buffer.
 * @param [in] msgSrc: MsgSrc var that indicates where the command came from.
 * @return  treeNode_t*: pointer to the submenu where the system is currently
 * pointing to.
 * @note: this can be different than the passed in node.
 */
treeNode_t* MENU_parseCurrLevelMenuItems(
      treeNode_t* node,
      const char* pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
);

/**
 * @brief   Add a sub-menu to an existing menu node.
 *
 * This function should be called by the user to add a new menu to the existing
 * menu tree. It takes in a menu node to add as well as a parent node to which
 * the new menu will be attached to as a "child" node.
 *
 * @note 1: This function is just a wrapper around MENU_addChild() which does the
 * actual manipulation of the tree.
 *
 * @note 2: "child" is in quotes because the menu implementation is actually a
 * k-tree that is implemented as a binary tree.  See notes at the top of ktree.h
 * header file for details.
 *
 * @param [in|out] *node: treeNode_t pointer to the submenu that is to be added.
 * @param [in|out] *parent: treeNode_t pointer to the menu where the new submenu
 * will be added.
 * @param [in] *menuTxt: const char pointer to the storage of the text that will
 * be pointed to by the submenu being added.
 * @param [in] *menuSelectTxt: const char pointer to the storage of the selector
 * text that will be pointed by the node being added.
 * @param [in] pMenuFunction function pointer to the action function that will be
 * pointed by the node being added.  This action is what will execute when that
 * submenu is selected.  Can be NULL if no action is to be taken.
 * @return  treeNode_t*: pointer to the added submenu.
 */
treeNode_t* MENU_addSubMenu(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
);

/**
 * @brief   Add a menu item to an existing sub-menu node.
 *
 * This function should be called by the user to add a new menu item to the
 * existing menu tree. It takes in a menu item node to add as well as a parent
 * node (the sub-menu where the menu item will live) to which the new menu item
 * will be attached to as a "child" node.
 *
 * @note 1: This function is just a wrapper around MENU_addChild() which does the
 * actual manipulation of the tree.
 *
 * @note 2: "child" is in quotes because the menu implementation is actually a
 * k-tree that is implemented as a binary tree.  See notes at the top of ktree.h
 * header file for details.
 *
 * @param [in|out] *node: treeNode_t pointer to the menu item that is to be added.
 * @param [in|out] *parent: treeNode_t pointer to the menu where the new menu
 * item will be added.
 * @param [in] *menuTxt: const char pointer to the storage of the text that will
 * be pointed to by the menu item being added.
 * @param [in] *menuSelectTxt: const char pointer to the storage of the selector
 * text that will be pointed by the menu item being added.
 * @param [in] pMenuFunction function pointer to the action function that will be
 * pointed by the node being added.  This action is what will execute when that
 * submenu is selected.  Can be NULL if no action is to be taken though this
 * probaly makes no sense for a menu item.
 * @return  treeNode_t*: pointer to the added menu item.
 */
treeNode_t* MENU_addMenuItem(
      treeNode_t* node,
      treeNode_t* parent,
      const char* menuTxt,
      const char* menuSelectTxt,
      pMenuFunction action
);

/**
 * @brief   Prints the entire expanded menu tree.
 *
 * @param [in] node: a treeNode_t* pointer to the node from which to start the
 * printing from.
 * @param [in] msgSrc: MsgSrc var that specifies where to print to.
 *
 * @return: None
 */
void MENU_printMenuTree( treeNode_t* node, MsgSrc msgSrc );

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                             /* MENU_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
