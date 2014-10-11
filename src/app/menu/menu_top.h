/**
 * @file    menu_top.h
 * @brief   Top level of the menu.
 *
 * @date    10/10/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MENU_TOP_H_
#define MENU_TOP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "ktree.h"
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize and arrange the menu.
 * This function is used to compose all the sub-menus and menu items that exist
 * in the menu system.
 *
 * @param   None
 * @return  treeNode_t*: pointer to the root of the menu.
 */
treeNode_t* MENU_init( void );

/**
 * @brief   Parse the raw data of the command coming to the menu.
 *
 * This function parses the raw command data and determines whether:
 * 1. The input is valid.  If yes,
 * 2. The input is a top level command that applies to anywhere in the menu.
 * or
 * 3. The input needs to be parsed only at the level of the sub menu where the
 * system is pointing to currently.
 *
 * @param [in|out] node: treeNode_t* pointer to the submenu currently pointed to
 * @param [in] pBuffer: const char*  pointer to the storage of the buffer where the
 * command data resides
 * @param [in] bufferLen: uint16_t length of the data in the command buffer.
 * @param [in] msgSrc: MsgSrc var that indicates where the command came from.
 * @return  treeNode_t*: pointer to the submenu where the system is currently
 * pointing to.
 * @note: this can be different than the passed in node.
 */
treeNode_t* MENU_parse(
      treeNode_t *node,
      const char* pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
);

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                         /* MENU_TOP_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
