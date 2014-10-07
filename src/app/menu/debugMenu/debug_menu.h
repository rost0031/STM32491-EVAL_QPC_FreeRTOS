/**
 * @file 	debug_menu.h
 * @brief   Handles the debug menu section of the menu application.
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
#ifndef DEBUG_MENU_H_
#define DEBUG_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ktree.h"                                  /* For treeNode_t typedef */

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern treeNode_t menuDbg;
extern char *const menuDbg_TitleTxt;
extern char *const menuDbg_SelectKey;

extern treeNode_t menuDbgItem_toggleSerialDebug;
extern char *const menuDbgItem_toggleSerialDebugTxt;
extern char *const menuDbgItem_toggleSerialDebugSelectKey;

extern treeNode_t menuDbgItem_toggleEthDebug;
extern char *const menuDbgItem_toggleEthDebugTxt;
extern char *const menuDbgItem_toggleEthDebugSelectKey;

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
void MENU_toggleSerialDebugAction( void );

void MENU_toggleEthDebugAction( void );

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                       /* DEBUG_MENU_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
