/**
 * @file 	dbg_out_cntrl.h
 * @brief   Handles the debug output control menu items of the menu application.
 *
 * @date   	10/13/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DBG_OUT_CNTRL_H_
#define DBG_OUT_CNTRL_H_

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

extern treeNode_t menuDbgOutCntrl;
extern char *const menuDbgOutCntrl_TitleTxt;
extern char *const menuDbgOutCntrl_SelectKey;

extern treeNode_t menuDbgOutCntrlItem_toggleSerialDebug;
extern char *const menuDbgOutCntrlItem_toggleSerialDebugTxt;
extern char *const menuDbgOutCntrlItem_toggleSerialDebugSelectKey;

extern treeNode_t menuDbgOutCntrlItem_toggleEthDebug;
extern char *const menuDbgOutCntrlItem_toggleEthDebugTxt;
extern char *const menuDbgOutCntrlItem_toggleEthDebugSelectKey;

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

#endif                                                    /* DBG_OUT_CNTRL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
