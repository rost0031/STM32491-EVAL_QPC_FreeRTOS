/**
 * @file 	dbg_mod_cntrl.h
 * @brief   Handles the debug module control menu items of the menu application.
 * Allows user to enable/disable which modules provide their debugging information.
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
#ifndef DBG_MOD_CNTRL_H_
#define DBG_MOD_CNTRL_H_

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

extern treeNode_t menuDbgModCntrl;
extern char *const menuDbgModCntrl_TitleTxt;
extern char *const menuDbgModCntrl_SelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModGen;
extern char *const menuDbgModCntrlItem_toggleModGenTxt;
extern char *const menuDbgModCntrlItem_toggleModGenSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModSer;
extern char *const menuDbgModCntrlItem_toggleModSerTxt;
extern char *const menuDbgModCntrlItem_toggleModSerSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModTime;
extern char *const menuDbgModCntrlItem_toggleModTimeTxt;
extern char *const menuDbgModCntrlItem_toggleModTimeSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModEth;
extern char *const menuDbgModCntrlItem_toggleModEthTxt;
extern char *const menuDbgModCntrlItem_toggleModEthSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModI2C;
extern char *const menuDbgModCntrlItem_toggleModI2CTxt;
extern char *const menuDbgModCntrlItem_toggleModI2CSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModNOR;
extern char *const menuDbgModCntrlItem_toggleModNORTxt;
extern char *const menuDbgModCntrlItem_toggleModNORSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModSDR;
extern char *const menuDbgModCntrlItem_toggleModSDRTxt;
extern char *const menuDbgModCntrlItem_toggleModSDRSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModDBG;
extern char *const menuDbgModCntrlItem_toggleModDBGTxt;
extern char *const menuDbgModCntrlItem_toggleModDBGSelectKey;

extern treeNode_t menuDbgModCntrlItem_toggleModCOMM;
extern char *const menuDbgModCntrlItem_toggleModCOMMTxt;
extern char *const menuDbgModCntrlItem_toggleModCOMMSelectKey;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Called by the menu item to toggle general module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModGeneralAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle serial module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModSerialAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle time module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModTimeAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle ethernet module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModEthAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle I2C module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModI2CAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle NOR module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModNORAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle SDRAM module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModSDRAMAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle DEBUG module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModDBGAction(
      const char* dataBuf,
      uint16_t dataLen,
      MsgSrc dst
);

/**
 * @brief Called by the menu item to toggle COMM module debugging.
 * @param [in] dataBuf: const char* pointer to the data passed in by the user at
 * cmd line
 * @param [in] dataLen: uint16_t length of data in the dataBuf.
 * @param [in] dst: MsgSrc destination so MENU_printf() knows were to direct the
 * output.
 * @return: None
 */
void MENU_toggleDbgModCOMMAction(
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

#endif                                                    /* DBG_MOD_CNTRL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
