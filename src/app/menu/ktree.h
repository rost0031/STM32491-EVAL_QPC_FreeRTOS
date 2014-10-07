/**
 * @file    ktree.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KTREE_H_
#define KTREE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef void (*pFunction)(void);

typedef struct treeNode {
   struct treeNode *fakeParentNode;
   struct treeNode *trueParentNode;
   struct treeNode *firstChildNode;
   struct treeNode *firstSiblingNode;
   char *text;
   char *selector;
   pFunction actionToTake;
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


void KTREE_addChild(
      treeNode_t *childToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
);

void KTREE_addSibling(
      treeNode_t *siblingToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
);

void KTREE_addNextSibling(
      treeNode_t *siblingToAdd,
      treeNode_t *trueParentNode,
      treeNode_t *fakeParentNode
);

treeNode_t* KTREE_getLastSibling( treeNode_t *node );

treeNode_t* KTREE_getLastChild( treeNode_t *node );

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
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
