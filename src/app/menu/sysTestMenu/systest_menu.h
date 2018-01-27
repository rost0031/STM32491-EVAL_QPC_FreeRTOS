/**
 * @file    systest_menu.h
 * @brief   Handles the system tests menu section of the menu application.
 *
 * @date    10/02/2014
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYSTEST_MENU_H_
#define SYSTEST_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "Shared.h"
#include "ktree.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern treeNode_t menuSysTest;
extern char *const menuSysTest_TitleTxt;
extern char *const menuSysTest_SelectKey;

extern treeNode_t menuSysTest_I2C;
extern char *const menuSysTest_I2C_TitleTxt;
extern char *const menuSysTest_I2C_SelectKey;

/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupMenu
 */

#ifdef __cplusplus
}
#endif

#endif                                                     /* SYSTEST_MENU_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
