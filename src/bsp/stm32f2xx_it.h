/**
 * @file   stm32f2xx_it.h
 * @brief  This file contains the declarations of the interrupt handlers.
 *
 * This file is derived from the original version distributed by ST Micro. THis
 * is not strictly necessary since the ISRs are "weak" linked to the assembly
 * code that calls them but this provides a good place to document the ISRs in
 * a header file.
 *
 * @date   06/23/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_IT_H
#define __STM32F2xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "Shared.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/**
 * This ISR function handles DMA1_Stream4 global interrupt requests.
 * @param     None
 * @retval    None
 */
void DMA1_Stream4_IRQHandler( void ) __attribute__((__interrupt__));


/**
 * This ISR function handles TIM5 global interrupt requests.
 * @param     None
 * @retval    None
 */
void TIM5_IRQHandler(void) __attribute__((__interrupt__));

/**
 * This ISR function handles RTC Wakeup global interrupt request.  Specifically,
 * it fires once a second and resets the timer counter that is used to track the
 * subsecond time.
 * @param  None
 * @retval None
 */
void RTC_WKUP_IRQHandler(void) __attribute__((__interrupt__));

#ifdef __cplusplus
}
#endif

#endif                                                    /* __STM32F2xx_IT_H */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
