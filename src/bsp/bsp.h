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

#define BSP_TICKS_PER_SEC 10000 // Tells QPC how often to run with respect to system clock

typedef enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
	EXTI0_PRIO,                                         /* highest priority */
	EXTI1_PRIO,
	EXTI2_PRIO,
	EXTI3_PRIO,
	EXTI4_PRIO,
	EXTI9_5_PRIO,
	EXTI15_10_PRIO,
	TIM1_BRK_TIM9_PRIO,
	TIM8_BRK_TIM12_PRIO,
	TIM3_PRIO,
	TIM5_PRIO,
	TIM1_UP_TIM10_PRIO,
	TIM8_UP_TIM13_PRIO,
	DMA2_Stream3_PRIO,
	DMA2_Stream2_PRIO,
	DMA1_Stream4_PRIO,
	DMA1_Stream2_PRIO,
	DMA1_Stream3_PRIO,
	DMA1_Stream0_PRIO,
	DMA1_Stream1_PRIO,
	DMA1_Stream5_PRIO,
	DMA1_Stream6_PRIO,
	SYSTICK_PRIO,
	USART3_PRIO,
	ETH_PRIO,			/* Ethernet should take the lowest priority */
	/* ... */
} ISR_Priority;

void BSP_init(void);
void NVIC_Config(uint8_t irq, uint8_t priority);

#ifndef NOT_NEEDED_IN_TEST_PROJ
void BSP_busyDelay(void);               // to artificially extend RTC processing
void BSP_Delay(uint32_t nCount);	          // Delay used for init of hardware
#endif

#endif                                                                // bsp_h
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
