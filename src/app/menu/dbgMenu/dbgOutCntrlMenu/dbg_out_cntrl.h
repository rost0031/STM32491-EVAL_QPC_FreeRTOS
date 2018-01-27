/**
 * @file 	dbg_out_cntrl.h
 * @brief   Handles the debug output control menu items of the menu application.
 *
 * @date   	10/13/2014
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
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
 * @brief Called by the menu item to toggle debugging over serial.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleSerialDebugAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle debugging over ethernet.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleEthDebugAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                    /* DBG_OUT_CNTRL_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
