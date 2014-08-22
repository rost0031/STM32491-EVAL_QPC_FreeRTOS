/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

#include <stdio.h>                                    /* for printf() support */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
 * @brief  This function gets called by Hard Fault exception handler to print
 * out the contents of the registers.
 * @param  [in] *hardfault_args: pointer to array containing values of registers.
 * @retval None
 */
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

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles External line 8 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

/**
  * @brief  This function handles Ethernet interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
//  ETHERNET_IRQHandler();
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
