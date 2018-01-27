/**
 * @file   stm32f4xx_it.c
 * @brief  This file contains most interrupt handler functions.
 *
 * @date   06/23/2014
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupISR
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "stm32f4xx_dma.h"                                 /* for DMA support */
#include "stm32f4xx_tim.h"                                 /* for TIM support */
#include "stm32f4xx_rtc.h"                                 /* for RTC support */
#include "stm32f4xx_exti.h"                               /* for EXTI support */
#include "stm32f4x7_eth_bsp.h"

#include <stdio.h>                                    /* for printf() support */
#include "project_includes.h"        /* application events and active objects */
#include "qp_port.h"                                        /* for QP support */
#include "Shared.h"                                   /*  Common Declarations */
#include "CBSignals.h"                            /*  For signal declarations */
#include "SerialMgr.h"                                 /* for SerialMgr types */
#include "time.h"                          /* for processor date/time support */
#include "i2c.h"                                /* For I2C callback functions */
#include "serial.h"                          /* For Serial callback functions */
#include "eth_driver.h"                    /* For Ethernet callback functions */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/******************************************************************************/
void NMI_Handler(void)
{
}

/******************************************************************************/
void hard_fault_handler_c(unsigned int *hardfault_args)
{
   unsigned int stacked_r0;
   unsigned int stacked_r1;
   unsigned int stacked_r2;
   unsigned int stacked_r3;
   unsigned int stacked_r12;
   unsigned int stacked_lr;
   unsigned int stacked_pc;
   unsigned int stacked_psr;

   stacked_r0 = ((unsigned long) hardfault_args[0]);
   stacked_r1 = ((unsigned long) hardfault_args[1]);
   stacked_r2 = ((unsigned long) hardfault_args[2]);
   stacked_r3 = ((unsigned long) hardfault_args[3]);

   stacked_r12 = ((unsigned long) hardfault_args[4]);
   stacked_lr = ((unsigned long) hardfault_args[5]);
   stacked_pc = ((unsigned long) hardfault_args[6]);
   stacked_psr = ((unsigned long) hardfault_args[7]);

   printf ("[Hard fault handler - all numbers in hex]\n");
   printf ("R0 = %x\n", stacked_r0);
   printf ("R1 = %x\n", stacked_r1);
   printf ("R2 = %x\n", stacked_r2);
   printf ("R3 = %x\n", stacked_r3);
   printf ("R12 = %x\n", stacked_r12);
   printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
   printf ("PC [R15] = %x  program counter\n", stacked_pc);
   printf ("PSR = %x\n", stacked_psr);
   printf ("BFAR = %lx\n", (*((volatile unsigned long *)(0xE000ED38))));
   printf ("CFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED28))));
   printf ("HFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED2C))));
   printf ("DFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED30))));
   printf ("AFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED3C))));
   printf ("SCB_SHCSR = %lx\n", SCB->SHCSR);

   /* Go to infinite loop when Hard Fault exception occurs */
   while (1){
   }
}

/******************************************************************************/
void MemManage_Handler(void)
{
   printf ("[MemManage_handler!]\n");
   /* Go to infinite loop when Memory Manage exception occurs */
   while (1)
   {
   }
}

/******************************************************************************/
void BusFault_Handler(void)
{
   printf ("[BusFault_Handler!]\n");
   /* Go to infinite loop when Bus Fault exception occurs */
   while (1)
   {
   }
}

/******************************************************************************/
void UsageFault_Handler(void)
{
   printf ("[UsageFault_Handler!]\n");
   /* Go to infinite loop when Usage Fault exception occurs */
   while (1)
   {
   }
}

/******************************************************************************/
void DebugMon_Handler(void)
{
   printf ("[DebugMon_Handler!]\n");
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/******************************************************************************/
void DMA1_Stream0_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   I2C1_DMAReadCallback(); /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void DMA1_Stream6_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   I2C1_DMAWriteCallback(); /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void DMA2_Stream7_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   Serial_DMASendCallback(); /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   /* the usual end of FreeRTOS ISR... */
   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void ETH_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   ETH_EventCallback();      /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void I2C1_ER_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   I2C1_ErrorEventCallback();/* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void I2C1_EV_IRQHandler( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   I2C1_EventCallback();     /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}

/******************************************************************************/
void RTC_WKUP_IRQHandler( void )
{
   /* No need to tell RTOSes about this ISR since it doesn't interface with them */
   if( RESET != RTC_GetITStatus(RTC_IT_WUT) ) {

      /* Clear the pending bits so the interrupt fires again next time. */
      RTC_ClearITPendingBit(RTC_IT_WUT);
      EXTI_ClearITPendingBit(EXTI_Line22);
   }
   /* No need to tell RTOSes about this ISR since it doesn't interface with them */
}

///******************************************************************************/
//void SysTick_Handler( void )
//{
//   /* Issue the callback function which does the actual work. */
//   BSP_SysTickCallback();
//}

/******************************************************************************/
extern __IO unsigned long uwPeriodValue;     /**< external reference to period value used by TIME_getLSIFrequency() */
extern __IO unsigned long uwCaptureNumber;   /**< external reference to capture number used by TIME_getLSIFrequency() */
uint16_t tmpCC4[2] = {0, 0};                 /**< Local storage for TIM5_IRQHandler() to temporarily store timer counts for frequency calculation */
void TIM5_IRQHandler( void )
{
   /* No need to tell RTOSes about this ISR since it doesn't interface with them */
   /* This is for measuring the LSI frequency for configuring the RTC */
   if ( RESET != TIM_GetITStatus( TIM5, TIM_IT_CC4 ) ) {
      /* Get the Input Capture value */
      tmpCC4[uwCaptureNumber++] = TIM_GetCapture4( TIM5 );

      /* Clear CC4 Interrupt pending bit */
      TIM_ClearITPendingBit( TIM5, TIM_IT_CC4 );

      if ( uwCaptureNumber >= 2 ) {
         /* Compute the period length */
         uwPeriodValue = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
      }
   }
   /* No need to tell RTOSes about this ISR since it doesn't interface with them */
}

/******************************************************************************/
void USART1_IRQHandler(void)
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   Serial_UART1Callback(); /* Issue the callback function which does the actual work. */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);/* the end of FreeRTOS ISR */
}
/**
 * @} end addtogroup groupISR
 */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
