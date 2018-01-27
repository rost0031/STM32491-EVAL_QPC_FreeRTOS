/**
 * @file    systest_menu.c
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
/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "systest_menu.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Variables to define this submenu */
treeNode_t menuSysTest;
char *const menuSysTest_TitleTxt = "System Test Menu";
char *const menuSysTest_SelectKey = "SYS";

/* All the submenus available in the above submenu */
treeNode_t menuSysTest_I2C;
char *const menuSysTest_I2C_TitleTxt = "I2C Test Menu";
char *const menuSysTest_I2C_SelectKey = "I2C";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
