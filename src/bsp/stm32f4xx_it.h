/**
 * @file   stm32f4xx_it.h
 * @brief  This file contains the declarations of the interrupt handlers.
 *
 * This file is derived from the original version distributed by ST Micro. This
 * is not strictly necessary since the ISRs are "weak" linked to the assembly
 * code that calls them but this provides a good place to document the ISRs in
 * a header file.
 *
 * @date   06/23/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupISR
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  This function handles Non-maskable interrupt (NMI) exception.
 * @param  None
 * @return None
 */
void NMI_Handler( void ) __attribute__((__interrupt__));


/**
 * @brief  This function gets called by Hard Fault exception handler to print
 * out the contents of the registers.
 * @param  [in] *hardfault_args: pointer to array containing values of registers.
 * @return None
 */
void hard_fault_handler_c( unsigned int *hardfault_args );

/**
 * @brief  This function handles Hard Fault exception.
 *
 * This ISR has been modified to print out the registers to the serial port if
 * it occurs.
 *
 * @param  None
 * @return None
 */
void HardFault_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles Memory Management exception.
 *
 * @param  None
 * @return None
 */
void MemManage_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @return None
 */
void BusFault_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @return None
 */
void UsageFault_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles SVC exception.
 * @param  None
 * @return None
 */
void SVC_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @return None
 */
void DebugMon_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles PendSV exception.
 * @param  None
 * @return None
 */
void PendSV_Handler( void ) __attribute__((__interrupt__));


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/


/**
 * @brief   This ISR function handles DMA1 Stream0 interrupt requests.
 * @param  None
 * @retval None
 */
void DMA1_Stream0_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles DMA1 Stream6 interrupt requests.
 * @param  None
 * @retval None
 */
void DMA1_Stream6_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles DMA2_Stream7 global interrupt requests.
 *
 * This ISR function turns off the DMA used by UART1 upon completion of TX.
 * @param     None
 * @retval    None
 */
void DMA2_Stream7_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles Ethernet global interrupt request.
 *
 * @param  None
 * @retval None
 */
void ETH_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles I2C1 bus event interrupt requests.
 * See i2c.c for implementation.
 * @param  None
 * @retval None
 */
void I2C1_EV_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles I2C1 bus error interrupt requests.
 * See i2c.c for implementation.
 * @param  None
 * @retval None
 */
void I2C1_ER_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles RTC Wakeup global interrupt request.
 *
 * This ISR fires every second change of the RTC and resets TIM7 which is used
 * for subsecond timer functionality.
 *
 * @note 1: Since this is a (QF) kernel unaware ISR, this handler may not access
 * any QPC functionality.
 *
 * @param  None
 * @retval None
 */
void RTC_WKUP_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles the SysTick global interrupt request.
 *
 * This ISR function mostly just processes all the QPC events that are ready to
 * be executed.  If QSPY is compiled in, it also handles its timing and clock
 * rollover.  The SysTick interrupt is fired @def BSP_TICKS_PER_SEC / second.
 *
 * @param     None
 * @retval    None
 */
void SysTick_Handler( void ) __attribute__((__interrupt__));

/**
 * @brief   This ISR function handles TIM5 global interrupt requests.
 *
 * This ISR is used to sample and time the clock rate of the RTC clock and is
 * part of the (QF) kernel unaware interrupts.
 *
 * @note 1: Since this is a (QF) kernel unaware ISR, this handler may not access
 * any QPC functionality.
 *
 * @param     None
 * @retval    None
 */
void TIM5_IRQHandler( void ) __attribute__((__interrupt__));

/**
 * @brief  This function handles LTDC global interrupt request.
 *
 * This ISR should use the callback function which should be defined wherever
 * LTDC communication is handled.
 *
 * @param  None
 * @retval None
 */
void LTDC_IRQHandler(void) __attribute__((__interrupt__));

/**
 * @brief  This function handles DMA2D global interrupt request.
 *
 * This ISR should use the callback function which should be defined wherever
 * DMA2D is handled.
 *
 * @param  None
 * @retval None
 */
void DMA2D_IRQHandler(void) __attribute__((__interrupt__));

/**
 * @brief   ISR that handles incoming data on UART1 (debug serial) port.
 *
 * This ISR should use the callback function which should be defined wherever
 * serial communications is handled.
 *
 * @param   None
 * @retval  None
 */
void USART1_IRQHandler(void) __attribute__((__interrupt__));

#ifdef __cplusplus
}
#endif

/**
 * @} end addtogroup groupISR
 */
#endif                                                    /* __STM32F4xx_IT_H */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
