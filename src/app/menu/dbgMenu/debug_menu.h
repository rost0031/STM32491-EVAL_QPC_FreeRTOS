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

/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                       /* DEBUG_MENU_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
