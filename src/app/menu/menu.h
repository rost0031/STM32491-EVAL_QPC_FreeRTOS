/**
 * @file    menu.h
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
#define MENU_MAX_FAKE_DEPTH                       20
#define MENU_MAX_TRUE_DEPTH                       100
#define MENU_MAX_SIBLINGS                         50
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
   treeNode_t* pathToTopFake[MENU_MAX_FAKE_DEPTH];
   /**< Index into the pathToTopFake array. */
   uint8_t pathToTopFakeIndex;
   /**< Array of pointers to treeNode_t types that will hold the path from
    * the currently selected node to the topmost parent via the trueParentNode
    * pointers. This array is arranged from the child to the top so has to be
    * traversed backwards to get back to the child. */
   treeNode_t* pathToTopTrue[MENU_MAX_TRUE_DEPTH];
   /**< Index into the pathToTopTrue array. */
   uint8_t pathToTopTrueIndex;
} menuNav_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

treeNode_t* MENU_init( void );

void MENU_printHelp( MsgSrc whereToPrint );

treeNode_t* MENU_parse(
      treeNode_t *node,
      uint8_t *pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
);

treeNode_t* MENU_parseCurrLevelMenuItems(
      treeNode_t *node,
      uint8_t *pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
);

void MENU_printCurrMenu( treeNode_t *node, MsgSrc whereToPrint );

void MENU_printMenuExpandedAtCurrNode(
      treeNode_t *node,
      MsgSrc whereToPrint
);

void MENU_printMenuCurrLevel( treeNode_t *node, MsgSrc whereToPrint );

void MENU_printMenuTree( treeNode_t *node, MsgSrc whereToPrint );

void MENU_printNode( treeNode_t *node, MsgSrc whereToPrint );
/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                             /* MENU_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
