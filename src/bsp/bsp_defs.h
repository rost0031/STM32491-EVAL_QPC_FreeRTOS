/**
 * @file bsp.h
 * @brief  This file contains the Board Support Package functions for Redwood_H1 Board
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 *
 * @addtogroup groupBSP
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_DEFS_H_
#define BSP_DEFS_H_

/* Includes ------------------------------------------------------------------*/
#include "Shared.h"                                   /*  Common Declarations */
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
/* Exported defines ----------------------------------------------------------*/
/**
 * @brief   How many times per second that the RTOS should attempt to handle its
 * events.  Tells QPC how often to run with respect to system clock.
 */
#define BSP_TICKS_PER_SEC                                                  10000

/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   This macro converts milliseconds from BSP ticks.
 */
#define BSP_TICKS_PER_MS  (BSP_TICKS_PER_SEC / 1000)

/**
 * @brief   Convert seconds to BSP ticks used by timers in QPC.
 *
 * @param [in] seconds: number of seconds that is desired
 * @return  ticks: number of ticks to actually arm the QP timer on the board.
 */
#define SEC_TO_TICKS( seconds )   (uint32_t)( (1.0 * seconds) * BSP_TICKS_PER_SEC )

/**
 * @brief   Convert milliseconds to BSP ticks used by timers in QPC.
 *
 * @param [in] ms: number of seconds that is desired
 * @return  ticks: number of ticks to actually arm the QP timer on the board.
 */
#define MS_TO_TICKS( ms )   (uint32_t)( (1.0 * ms) * BSP_TICKS_PER_MS )

/* Exported types ------------------------------------------------------------*/

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
* macros or any other QF/QK  services. These ISRs are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF/QK services. In particular they
* can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs {                                        /* see NOTE00 */
   RTC_WKUP_PRIO = 0,
   TIM5_PRIO,
   /* ... */
   MAX_KERNEL_UNAWARE_CMSIS_PRI                          /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

typedef enum KernelAwareISRs {   /* ISR priorities starting from the highest urgency */
	SYSTICK_PRIO = QF_AWARE_ISR_CMSIS_PRI,                         /* see NOTE00 */
	DMA2_Stream7_PRIO,
	DMA1_Stream6_PRIO,
	DMA1_Stream0_PRIO,
	USART1_PRIO,
   I2C1_ER_PRIO,
	I2C1_EV_PRIO,
	ETH_PRIO,                      /* Ethernet should take the lowest priority */
	ETH_LINK_PRIO,
	/* ... */
	MAX_KERNEL_AWARE_CMSIS_PRI                             /* keep always last */
} ISR_Priority;

/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/**
 * @} end addtogroup groupBSP
 */

#endif                                                         /* BSP_DEFS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
