/**
 * @file bsp.h
 * @brief  This file contains the Board Support Package functions for Redwood_H1 Board
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2012 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupBSP
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_H_
#define BSP_H_

/* Includes ------------------------------------------------------------------*/
#include "Shared.h"                                   /*  Common Declarations */
#include "qp_port.h"                                        /* for QP support */
#include "bsp_defs.h"                               /* Common BSP definitions */

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief   Initializes the BSP for the board.
 *
 * This function initializes the board support package (BSP).  It initializes:
 *    - All clocks either directly or by calling BSP modules' functions.
 *    - All GPIO and EXTI lines
 *    - All NVIC and associated ISRs.
 *
 * @note 1: Should be called only once and only in the beginning.
 * @note 2: Most clocks (not all), and any device hardware bringup is done here.
 * @param  None
 * @return None
 */
void BSP_init( void );

/**
 * @brief  Initializes IRQ and priority in the NVIC with the provided IRQ and
 * priority.
 *
 * This function is a utility wrapper around initializing NVIC and assigning
 * IRQs priorities.  It is called by any function that wishes to initialize an
 * interrupt.
 *
 * @param [in] irq: selects the irq to assign priority to
 * @param [in] priority: selects the priority to assign to the specified irq
 * @return None
 */
void NVIC_Config( uint8_t irq, uint8_t priority );

/**
 * @brief  Delay Function.
 *
 * This function should only be used in BSP startup and not after the BSP is
 * running.  It simply implements a manual loop that spins down to zero.
 *
 * @param [in] nCount: uint32_t that specifies the delay time length.
 * @return None
 */
void BSP_Delay(uint32_t nCount);

/**
 * @brief   Systick callback function
 *
 * This function should only be called from the ISR that handles the SysTick ISR
 *
 * @note: this function is defined as "inline" but not declared as such.  This
 * is so it can be called externally (by the file that contains the actual ISRs)
 * and they can still be inlined so as not incur any function call overhead.
 *
 * @param   None
 * @return: None
 */
void BSP_SysTickCallback( void );

/**
 * @} end addtogroup groupBSP
 */

#endif                                                              /* BSP_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
