// $Id$
/**
 * @file   stm32f2xx_it.c
 * @brief  This file contains all interrupts for motors and associated sensors
 * 		   for the Redwood_H1 board. This file is derived from the original
 * 		   version distributed by ST Micro.
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

/* Includes ------------------------------------------------------------------*/

#include "stm32f2xx_it.h"
#include "stm32f2xx_dma.h"
#include "project_includes.h"        /* application events and active objects */
#include "qp_port.h"                                        /* for QP support */
#include "Shared.h"                                   /*  Common Declarations */
#include <stdio.h>
#include "CBSignals.h"

/** @addtogroup Template_Project
 * @{
 */

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void hard_fault_handler_c(unsigned int * hardfault_args) {
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

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
   /* Go to infinite loop when Memory Manage exception occurs */
   while (1) {
   }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
   /* Go to infinite loop when Bus Fault exception occurs */
   while (1) {
   }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
   /* Go to infinite loop when Usage Fault exception occurs */
   while (1) {
   }
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.S).                                               */
/******************************************************************************/

/******************************************************************************/
void DMA1_Stream4_IRQHandler(void) {
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) ) {
      /* Disable DMA so it doesn't keep outputting the buffer. */
      DMA_Cmd(DMA1_Stream4, DISABLE);

      /* Publish event stating that the count has been reached */
      QEvt *qEvt = Q_NEW(QEvt, UART_DMA_DONE_SIG);
      QF_PUBLISH((QEvent *)qEvt, AO_SerialMgr );

      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
   }

   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
