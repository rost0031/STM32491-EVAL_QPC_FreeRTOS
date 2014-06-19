// $Id$
/**
 * @file bsp.h
 * @brief  This file contains the Board Support Package functions for Redwood_H1 Board
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

#ifndef bsp_h
#define bsp_h

#include "Shared.h"
#include "qf_port.h"
#include "stm32f2xx.h"

#define BSP_TICKS_PER_SEC 10000 // Tells QPC how often to run with respect to system clock
#define BSP_TICKS_PER_MS  (BSP_TICKS_PER_SEC / 1000)

/**
 * Convert seconds to BSP ticks used by timers in QP.
 *
 * @param  seconds: number of seconds that is desired
 * @retval ticks: number of ticks to actually arm the QP timer on the laminator.
 */
#define SEC_TO_TICKS( seconds )   (uint32_t)( (1.0 * seconds) * BSP_TICKS_PER_SEC )

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
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI                          /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

typedef enum KernelAwareISRs {   /* ISR priorities starting from the highest urgency */
	EXTI0_PRIO = QF_AWARE_ISR_CMSIS_PRI,                         /* see NOTE00 */
//	EXTI1_PRIO,
//	EXTI2_PRIO,
//	EXTI3_PRIO,
//	EXTI4_PRIO,
//	EXTI9_5_PRIO,
//	EXTI15_10_PRIO,
//	TIM1_BRK_TIM9_PRIO,
//	TIM8_BRK_TIM12_PRIO,
//	TIM3_PRIO,
//	TIM5_PRIO,
//	TIM1_UP_TIM10_PRIO,
//	TIM8_UP_TIM13_PRIO,
//	DMA2_Stream3_PRIO,
//	DMA2_Stream2_PRIO,
	DMA1_Stream4_PRIO,
//	DMA1_Stream2_PRIO,
//	DMA1_Stream3_PRIO,
//	DMA1_Stream0_PRIO,
//	DMA1_Stream1_PRIO,
//	DMA1_Stream5_PRIO,
//	DMA1_Stream6_PRIO,
	SYSTICK_PRIO,
//	USART3_PRIO,
	UART4_PRIO,
	ETH_PRIO,			             /* Ethernet should take the lowest priority */
	/* ... */
	MAX_KERNEL_AWARE_CMSIS_PRI                             /* keep always last */
} ISR_Priority;

/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

void BSP_init(void);
void NVIC_Config(uint8_t irq, uint8_t priority);
void BSP_busyDelay(void);               // to artificially extend RTC processing
void BSP_Delay(uint32_t nCount);	          // Delay used for init of hardware

#endif                                                                // bsp_h
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
