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

void MENU_toggleDbgModGeneralAction( void );
void MENU_toggleDbgModSerialAction( void );
void MENU_toggleDbgModTimeAction( void );
void MENU_toggleDbgModEthAction( void );
void MENU_toggleDbgModI2CAction( void );
void MENU_toggleDbgModNORAction( void );
void MENU_toggleDbgModSDRAMAction( void );
void MENU_toggleDbgModDBGAction( void );
void MENU_toggleDbgModCOMMAction( void );


/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                    /* DBG_MOD_CNTRL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
