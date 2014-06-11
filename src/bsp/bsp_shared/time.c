/**
 * @file   time.c
 * @brief  This file contains the definitions for functions to control on the
 * board time and timestamps.
 *
 * @date   06/06/2014
 * @author Harry Rostovtsev
 * @email  Harry_Rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

#include "time.h"
#include "stm32f2xx.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_pwr.h"
#include "stm32f2xx_exti.h"
#include "Shared.h"
#include <stdio.h>
#include "bsp.h"
#include "project_includes.h"

/* Private defines -----------------------------------------------------------*/
#define DELAY_TIM_FREQUENCY 10000

/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
TIM_ICInitTypeDef  TIM_ICInitStructure;
RCC_ClocksTypeDef  RCC_ClockFreq;
__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Configures TIM5 to measure the LSI oscillator frequency.
 * @param  None
 * @retval LSI Frequency
 */
static uint32_t TIME_getLSIFrequency( void );


/******************************************************************************/
void TIME_Init( void )
{
   /* Enable the PWR clock */
   RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE );

   /* Allow access to RTC */
   PWR_BackupAccessCmd( ENABLE );

   /* Enable the LSI OSC */
   RCC_LSICmd( ENABLE );

   /* Wait till LSI is ready */
   while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
   {
   }

   /* Select the RTC Clock Source */
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

   /* Enable the RTC Clock */
   RCC_RTCCLKCmd( ENABLE );

   /* Wait for RTC APB registers synchronisation */
   RTC_WaitForSynchro();

   /* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
   LsiFreq = TIME_getLSIFrequency();

   /* Calendar Configuration */
   RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
   RTC_InitStructure.RTC_SynchPrediv =  (LsiFreq/128) - 1;
   RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   /* Check on RTC init */
   if (RTC_Init(&RTC_InitStructure) == ERROR) {
      err_slow_printf("!!RTC Prescaler Config failed!!\n");
   }

   /* Initialize the clock to zero */
   RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
   RTC_TimeStructure.RTC_Hours   = 0;
   RTC_TimeStructure.RTC_Minutes = 0;
   RTC_TimeStructure.RTC_Seconds = 0;

   /* Configure the RTC time register */
   if( ERROR == RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) ) {
      err_slow_printf("!! RTC Set Time failed. !!\n");
   }

   /* EXTI configuration ******************************************************/
   EXTI_ClearITPendingBit(EXTI_Line22);
   EXTI_InitStructure.EXTI_Line = EXTI_Line22;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   /* Set up Interrupt controller to handle RTC Wakeup interrupt */
   NVIC_Config( RTC_WKUP_IRQn, RTC_WKUP_PRIO );

   RTC_WakeUpCmd(DISABLE);

   /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
   RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
   RTC_SetWakeUpCounter(0x0);

   /* Enable the RTC Wakeup Interrupt */
   RTC_ITConfig(RTC_IT_WUT, ENABLE);

   /* Enable Wakeup Counter */
   RTC_WakeUpCmd(ENABLE);

   /* Enable the subsecond timer */
   TIME_subSecondTimer_Init();
}

/******************************************************************************/
uint32_t TIME_getLSIFrequency( void )
{
   /* TIM5 configuration *******************************************************/
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

   /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
   TIM_RemapConfig(TIM5, TIM5_LSI);

   /* Configure TIM5 presclaer */
   TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);

   /* TIM5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM5 CH4
     The Rising edge is used as active edge,
     The TIM5 CCR4 is used to compute the frequency value
  ------------------------------------------------------------ */
   TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
   TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
   TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
   TIM_ICInitStructure.TIM_ICFilter = 0;
   TIM_ICInit(TIM5, &TIM_ICInitStructure);

   /* Enable TIM5 Interrupt channel */
   NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /* Enable TIM5 counter */
   TIM_Cmd(TIM5, ENABLE);

   /* Reset the flags */
   TIM5->SR = 0;

   /* Enable the CC4 Interrupt Request */
   TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);

   /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in
    stm32f2xx_it.c file) ******************************************************/
   while(CaptureNumber != 2)
   {
   }
   /* Deinitialize the TIM5 peripheral registers to their default reset values */
   TIM_DeInit(TIM5);


   /* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
   /* Get SYSCLK, HCLK and PCLKx frequency */
   RCC_GetClocksFreq(&RCC_ClockFreq);

   /* Get PCLK1 prescaler */
   if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
   {
      /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
      return ((RCC_ClockFreq.PCLK1_Frequency / PeriodValue) * 8);
   }
   else
   { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
      return (((2 * RCC_ClockFreq.PCLK1_Frequency) / PeriodValue) * 8) ;
   }
}


/******************************************************************************/
void TIME_subSecondTimer_Init( void )
{
   /* Enable timer clock  - use TIMER7 */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

   /* Time base configuration - The magic numbers allow a ~10000 ticks/sec on
    * this timer. */
   TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
   TIM_TimeBaseStructure.TIM_Prescaler = 1191;
   TIM_TimeBaseStructure.TIM_Period = 10049;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

   /* Enable counter */
   TIM_Cmd(TIM7, ENABLE);
}

/******************************************************************************/
void TIME_printTime( void )
{
   /* Get the current Time and Date */
   t_Time time = TIME_getTime();
   printf("%02d:%02d:%02d:%d\n",
         time.hour_min_sec.RTC_Hours,
         time.hour_min_sec.RTC_Minutes,
         time.hour_min_sec.RTC_Seconds,
         (int)time.sub_sec
   );
}

/******************************************************************************/
t_Time TIME_getTime( void )
{
   t_Time time;

   /* Get the current Time and Date */
   RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

   /* Unfreeze the RTC DR Register */
   (void)RTC->DR;

   time.hour_min_sec = RTC_TimeStructure;
   time.sub_sec = TIM7->CNT;

   return (time);
}
