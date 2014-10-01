/**
 * @file 	menu.h
 * @brief   Handles the menu application.
 *
 * @date   	09/29/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
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
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

//typedef void (*pFunction)(void);
//
//typedef struct treeNode {
//   char *text;
//   struct treeNode *parent;
//   struct treeNode *firstChildNode;
//   struct treeNode *firstSiblingNode;
//   pFunction actionToTake;
//} treeNode_t;


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
//CBErrorCode MENU_init( void );

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                             /* MENU_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
