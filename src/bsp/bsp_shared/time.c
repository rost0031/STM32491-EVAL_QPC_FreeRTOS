/**
 * @file   time.c
 * @brief  This file contains the definitions for functions to control on the
 * board time and timestamps.
 *
 * @date   06/06/2014
 * @author Harry Rostovtsev
 * @email  Harry_Rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupTime
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "time.h"
#include "Shared.h"
#include "bsp.h"
#include "bsp_defs.h"
#include "project_includes.h"
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "stm32f4xx_rtc.h"                           /* For STM32 RTC support */
#include "stm32f4xx_rcc.h"                           /* For STM32 clk support */
#include "stm32f4xx_tim.h"                         /* For STM32 Timer support */
#include "stm32f4xx_exti.h"                         /* For STM32 EXTI support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;   /**< Init structure for RTC, must be accessible by local functions to get the synch prediv used to set up RTC */

__IO uint32_t   uwLsiFreq = 0;       /**< Measured LSI frequency */
__IO uint32_t   uwCaptureNumber = 0; /**< Counter to keep track of captures on TIM5 used to measure LSI frequency, shared with ISR. */
__IO uint32_t   uwPeriodValue = 0;   /**< Calculated period value of LSI output, shared with ISR. */

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Configures TIM5 to measure the LSI oscillator frequency.
 * This function configures TIM5 to measure the frequency of the LSI oscillator.
 * The reason to do this is because LSI is quite drifty and can cause the RTC to
 * be off by quite a bit.  This function measures the "real" frequency instead
 * of depending on the supposed 32.67 kHz frequency.
 *
 * @note 1: This function sets up TIM5 but after returning, TIM5 can be used for
 * other purposes.
 * @note 2: This function should only be called by the TIME_Init() function.
 *
 * @param  None
 * @retval LSI Frequency
 */
static uint32_t TIME_getLSIFrequency( void );

/* Private functions ---------------------------------------------------------*/

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
   while( RESET == RCC_GetFlagStatus( RCC_FLAG_LSIRDY ) ) {
   }

   /* Select the RTC Clock Source */
   RCC_RTCCLKConfig( RCC_RTCCLKSource_LSI );

   /* Enable the RTC Clock */
   RCC_RTCCLKCmd( ENABLE );

   /* Wait for RTC APB registers synchronisation */
   RTC_WaitForSynchro();

   /* Calendar Configuration with LSI supposed at 32KHz */
   RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
   RTC_InitStructure.RTC_SynchPrediv  = 0xFF; /* (32KHz / 128) - 1 = 0xFF*/
   RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   RTC_Init(&RTC_InitStructure);

   /**TODO: This doesn't seem to be necessary.  Take out after verifying. HR */
//    /* EXTI configuration ******************************************************/
//   EXTI_ClearITPendingBit(EXTI_Line22);
//   EXTI_InitTypeDef EXTI_InitStructure;
//   EXTI_InitStructure.EXTI_Line = EXTI_Line22;
//   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//   EXTI_Init(&EXTI_InitStructure);
//
//   /* Set up Interrupt controller to handle RTC Wakeup interrupt */
//   NVIC_Config( RTC_WKUP_IRQn, RTC_WKUP_PRIO );

   /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
   RTC_WakeUpClockConfig( RTC_WakeUpClock_CK_SPRE_16bits );
   RTC_SetWakeUpCounter( 0x0 );

   /* Enable the RTC Wakeup Interrupt */
   RTC_ITConfig( RTC_IT_WUT, ENABLE );

   /* Enable Wakeup Counter */
   RTC_WakeUpCmd( ENABLE );

   /* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
   uwLsiFreq = TIME_getLSIFrequency();

   /* Calendar Configuration */
   RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
   RTC_InitStructure.RTC_SynchPrediv =  (uwLsiFreq/128) - 1;
   RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   /* Check on RTC init */
   if ( ERROR == RTC_Init( &RTC_InitStructure ) ) {
      err_slow_printf("!!RTC Prescaler Config failed!!\n");
   }

   /* Initialize the clock to zero */
   RTC_TimeTypeDef RTC_TimeStructure;
   RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
   RTC_TimeStructure.RTC_Hours   = 0;
   RTC_TimeStructure.RTC_Minutes = 0;
   RTC_TimeStructure.RTC_Seconds = 0;

   /* Configure the RTC time register */
   if( ERROR == RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) ) {
      err_slow_printf("!! RTC Set Time failed. !!\n");
   }

   /* Enable the subsecond timer */
//   TIME_subSecondTimer_Init();
}

/******************************************************************************/
static uint32_t TIME_getLSIFrequency( void )
{
   /* Enable the LSI oscillator ************************************************/
   RCC_LSICmd( ENABLE );

   /* TIM5 configuration *******************************************************/
   RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );

   /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
   TIM_RemapConfig( TIM5, TIM5_LSI );

   /* Configure TIM5 presclaer */
   TIM_PrescalerConfig( TIM5, 0, TIM_PSCReloadMode_Immediate );

   /* TIM5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM5 CH4
     The Rising edge is used as active edge,
     The TIM5 CCR4 is used to compute the frequency value
  ------------------------------------------------------------ */
   TIM_ICInitTypeDef  TIM_ICInitStructure;
   TIM_ICInitStructure.TIM_Channel     = TIM_Channel_4;
   TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
   TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
   TIM_ICInitStructure.TIM_ICFilter    = 0;
   TIM_ICInit( TIM5, &TIM_ICInitStructure );

   /* Enable TIM5 Interrupt channel */
   NVIC_Config( TIM5_IRQn, TIM5_PRIO );

   /* Enable TIM5 counter */
   TIM_Cmd( TIM5, ENABLE );

   /* Reset the flags */
   TIM5->SR = 0;

   /* Enable the CC4 Interrupt Request */
   TIM_ITConfig( TIM5, TIM_IT_CC4, ENABLE );

   /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in
    stm32f4xx_it.c file) ******************************************************/
   while(uwCaptureNumber != 2) {
   }
   /* Deinitialize the TIM5 peripheral registers to their default reset values.
    * It is not used again after this by the TIME module. */
   TIM_DeInit( TIM5 );

   /* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
   /* Get SYSCLK, HCLK and PCLKx frequency */
   RCC_ClocksTypeDef  RCC_ClockFreq;
   RCC_GetClocksFreq(&RCC_ClockFreq);

   /* Get PCLK1 prescaler */
   if (0 == (RCC->CFGR & RCC_CFGR_PPRE1) ) {
      /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
      return ((RCC_ClockFreq.PCLK1_Frequency / uwPeriodValue) * 8);
   } else {
      /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
      return (((2 * RCC_ClockFreq.PCLK1_Frequency) / uwPeriodValue) * 8) ;
   }
}

/******************************************************************************/
time_T TIME_getTime( void )
{
   time_T time;
   RTC_TimeTypeDef RTC_TimeStructure;

   /* Get the current Time and Date */
   RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

   /* Unfreeze the RTC DR Register.  Why this isn't done by the STM32 library
    * for this function but IS for the RTC_GetSubSecond(), I'll never know.
    * Also, this register is automatically "refrozen" as soon as your read it */
   (void)RTC->DR;

   /* Hours, minutes, and seconds are stored in the exact same format as in the
    * structure so we can use direct assignment. */
   time.hour_min_sec = RTC_TimeStructure;

   /* Sub-second depends on the frequency used by the clock and the fracSec
    * holds the down-counted timer, which then needs to be normalized using the
    * synch frequency prediv scalar.  The formula for this is:
    *
    * Second fraction = ( PREDIV_S - SS ) / ( PREDIV_S + 1 )
    */
   uint32_t fracSec = RTC_GetSubSecond();
   time.sub_sec = (uint32_t)( 1000 * (
         (float)(RTC_InitStructure.RTC_SynchPrediv  - fracSec) /
         (float)(RTC_InitStructure.RTC_SynchPrediv + 1))
   );

   return (time);
}

/**
 * @}
 * end addtogroup groupTime
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
