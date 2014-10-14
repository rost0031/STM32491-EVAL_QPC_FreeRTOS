/**
 * @file    dbg_cntrl.c
 * @brief   Module level debugging control.
 *
 * This file contains the definitions for debug and output functions over the
 * serial DMA console.
 *
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupDbgCntrl
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "dbg_cntrl.h"
#include "qp_port.h"                                        /* for QP support */
#include "CBSignals.h"
#include "CBErrors.h"
#include "Shared.h"                                   /*  Common Declarations */
#include "time.h"
#include "SerialMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**< This "global" variable is used to keep track of all the modules which have
 * debugging enabled throughout the system.
 * @note 1: this var should not be accessed directly by the developer.
 * @note 2: this var should be set/cleared/checked by
 * #DBG_ENABLE_DEBUG_FOR_MODULE(), #DBG_DISABLE_DEBUG_FOR_MODULE(),
 * #DBG_TOGGLE_DEBUG_FOR_MODULE(), #DBG_CHECK_DEBUG_FOR_MODULE() macros only!
 */
uint32_t  glbDbgConfig = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @} end addtogroup groupDbgCntrl
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
