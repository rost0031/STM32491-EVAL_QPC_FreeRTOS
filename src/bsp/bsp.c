/**
 * @file bsp.c
 * @brief  This file contains the Board Support Package functions for board
 *
 * @date   06/03/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupBSP
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

#include "stm32f4xx.h"                                 /* For STM32F4 support */

#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"        /* application events and active objects */
#include "time.h"                          /* for processor date/time support */
#include "i2c.h"                                           /* For I2C support */
#include "serial.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static uint32_t l_nTicks;

#ifdef Q_SPY
static uint8_t  l_SysTick_Handler;
QSTimeCtr QS_tickTime_;
QSTimeCtr QS_tickPeriod_;


#define QS_BUF_SIZE   (2*1024)
#define QS_BAUD_RATE  115200

#endif

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void BSP_init( void )
{
   Serial_Init( SERIAL_SYS );

   /* ENABLE GPIO clocks */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
//
//   /* DMA clock enable */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 , ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);
//
//   /* Enable syscfg clock */
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//
//   /* Enable TIM clocks on APB2 bus */
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
//
//   /* Enable TIM clocks on APB1 bus */
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
//
//   /* Enable CRC clock */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
//
//   /* Initialize the Serial for printfs to the serial port */
//   Serial_Init( SYSTEM_SERIAL );

//   /* Initialize the I2C devices and associated busses */
//   I2C_BusInit( I2CBus1 );

   /* Start Ethernet configuration */
   /* Assert a reset on the ETH_RST line.  This should only be released
    * after all the local MAC and DMA configuration is done. */
//   GPIO_ResetBits(GPIOA, GPIO_Pin_8);
//
//   /* initialize the ETH GPIO */
//   ETH_GPIO_Config();
//
//   /* initialize the ETH MACDMA */
//   ETH_MACDMA_Config();
//
//   /* Release the reset on the ETH_RST line now that the local MAC and DMA
//    * configuration is done. */
//   GPIO_SetBits(GPIOA, GPIO_Pin_8);
//   /* End Ethernet configuration */
//
//   /* Initialize the time (RTC and a subsecond timer). */
//   TIME_Init();
}

/******************************************************************************/
//void NVIC_Config(uint8_t irq, uint8_t priority)
//{
//   NVIC_InitTypeDef nvic_init;
//
//   nvic_init.NVIC_IRQChannel                   = irq;
//   nvic_init.NVIC_IRQChannelPreemptionPriority = 0x0;
//   nvic_init.NVIC_IRQChannelSubPriority        = 0x0;
//   nvic_init.NVIC_IRQChannelCmd                = ENABLE;
//   NVIC_Init(&nvic_init);/* enables the device and sets interrupt priority */
//   NVIC_ClearPendingIRQ(irq);
//   NVIC_SetPriority(irq,   		priority);
//}

/******************************************************************************/
void BSP_Delay(__IO uint32_t nCount)
{
   while(nCount--) {
   }
}

/* Externally referenced functions and callbacks -----------------------------*/

/******************************************************************************/
/**
 * @brief  Gets the current "time" for LWIP stack.
 *
 * This function is a callback defined by LWIP that is used to interface LWIP's
 * timers to the BSP.
 *
 * @param None
 * @return ticks: uint32_t ticks scaled by the BSP
 */
uint32_t sys_now(void)
{
   return (l_nTicks * (1000 / BSP_TICKS_PER_SEC));
}

/******************************************************************************/
/**
 * @brief  QF startup callback that initializes things necessary for QPC.
 *
 * This function is a callback implementation defined by QPC that is used to
 * startup some components necessary for the QPC RTOS.
 *
 * @param   None
 * @return  None
 */
void QF_onStartup( void )
{
   /* Set up and enable the SysTick timer.  It will be used as a reference
    * for delay loops in the interrupt handlers.  The SysTick timer period
    * will be set up for BSP_TICKS_PER_SEC.
    */
   SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

   /* assing all priority bits for preemption-prio. and none to sub-prio. */
   NVIC_SetPriorityGrouping(0U);

   /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
   NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
   NVIC_Config(ETH_IRQn, ETH_PRIO);
}

/******************************************************************************/
/**
 * @brief  QF cleanup callback that cleans up after a QPC shutdown.
 *
 * This function is a callback implementation defined by QPC that is used to
 * clean up.
 *
 * @param   None
 * @return  None
 */
void QF_onCleanup( void )
{
}

/******************************************************************************/
/**
 * @brief  QK callback that is called whenever the QPC is idle.
 *
 * This function is a callback implementation defined by QPC that is called
 * whenever QPC runs out of things to do and is idle.  It is used by QSPY
 * (if compiled in) to send data out to prevent interfering with the system as
 * much as possible.
 *
 * @param   None
 * @return  None
 */
void QK_onIdle(void)
{

#ifdef Q_SPY

   if ((USART3->SR & USART_FLAG_TXE) != 0) {              /* is TXE empty? */
      uint16_t b;

      QF_INT_DISABLE();
      b = QS_getByte();
      QF_INT_ENABLE();

      if (b != QS_EOD) {                              /* not End-Of-Data? */
         USART3->DR = (b & 0xFF);             /* put into the DR register */
      }
   }

#elif defined NDEBUG
   __WFI();                                          /* wait for interrupt */
#endif
}

/******************************************************************************/
/**
 * @brief  A callback that is called whenever the STM32 StdPeriph library errors
 * out.
 *
 * This function is a callback implementation defined by STM32 StdPeriph library
 * that is called whenever an assert fails there.
 *
 * @param [in] *file: char const pointer to the file where the assert occurred.
 * @param [in] line: int indicating the line where the assert occurred.
 * @return  None
 */
void assert_failed(char const *file, int line)
{
   Q_onAssert(file, line);
}

/******************************************************************************/
/**
 * @brief  QPC callback that is called when QPC encounters an assert.
 *
 * This function is a callback implementation defined by QPC that is called
 * whenever QPC asserts.
 *
 * @param [in] *file: char const pointer to the file where the assert occurred.
 * @param [in] line: int indicating the line where the assert occurred.
 * @return  None
 */
void Q_onAssert(char const * const file, int line)
{
   (void)file;                                   /* avoid compiler warning */
   (void)line;                                   /* avoid compiler warning */
   QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
   for (;;) {       /* NOTE: replace the loop with reset for final version */
   }
}

/*--------------------------------------------------------------------------*/
#ifdef Q_SPY

/******************************************************************************/
/**
 * @brief  QS startup callback that initializes things necessary for QSPY.
 *
 * This function is a callback implementation defined by QSPY that is used to
 * startup components necessary for QSPY output.
 *
 * @note 1: this function only exists on QSPY builds.
 *
 * @param [in] *arg: const void pointer to a passed in argument.  This may or
 * may not be used.
 * @retval  1: on success
 *          0: on failure
 */
uint8_t QS_onStartup(void const *arg) {
   static uint8_t qsBuf[QS_BUF_SIZE];            /* buffer for Quantum Spy */
   QS_initBuf(qsBuf, sizeof(qsBuf));

   /* USART3 and GPIOA/AFIO clocks are already enabled by this point */


   QS_tickPeriod_ = (QSTimeCtr)(SystemCoreClock / BSP_TICKS_PER_SEC);
   QS_tickTime_ = QS_tickPeriod_;        /* to start the timestamp at zero */

   /* setup the QS filters... */
   QS_FILTER_ON(QS_ALL_RECORDS);

   //    QS_FILTER_OFF(QS_QEP_STATE_EMPTY);
   //    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
   //    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
   //    QS_FILTER_OFF(QS_QEP_STATE_INIT);
   //    QS_FILTER_OFF(QS_QEP_INIT_TRAN);
   //    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
   //    QS_FILTER_OFF(QS_QEP_TRAN);
   //    QS_FILTER_OFF(QS_QEP_IGNORED);

   QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
   QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
   QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
   QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
   QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
   QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
   QS_FILTER_OFF(QS_QF_ACTIVE_GET);
   QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
   QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
   QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
   QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
   QS_FILTER_OFF(QS_QF_EQUEUE_GET);
   QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
   QS_FILTER_OFF(QS_QF_MPOOL_INIT);
   QS_FILTER_OFF(QS_QF_MPOOL_GET);
   QS_FILTER_OFF(QS_QF_MPOOL_PUT);
   QS_FILTER_OFF(QS_QF_PUBLISH);
   QS_FILTER_OFF(QS_QF_NEW);
   QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
   QS_FILTER_OFF(QS_QF_GC);
   //    QS_FILTER_OFF(QS_QF_TICK);
   QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
   QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
   QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
   QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
   QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
   QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
   QS_FILTER_OFF(QS_QF_CRIT_ENTRY);
   QS_FILTER_OFF(QS_QF_CRIT_EXIT);
   QS_FILTER_OFF(QS_QF_ISR_ENTRY);
   QS_FILTER_OFF(QS_QF_ISR_EXIT);

   //    QS_FILTER_OFF(QS_QK_MUTEX_LOCK);
   //    QS_FILTER_OFF(QS_QK_MUTEX_UNLOCK);
   QS_FILTER_OFF(QS_QK_SCHEDULE);

   return (uint8_t)1;                                    /* return success */
}

/******************************************************************************/
/**
 * @brief  QS shutdown callback that cleans up things for QSPY.
 *
 * This function is a callback implementation defined by QSPY that is used to
 * clean up after QSPY is shutdown.
 *
 * @note 1: this function only exists on QSPY builds.
 *
 * @param   None
 * @return  None
 */
void QS_onCleanup(void) {
}

/******************************************************************************/
/**
 * @brief  QS callback that gets called when a QSPY timer expires.
 *
 * This function is a callback implementation defined by QSPY that is used to
 * get the current BSP QPC time.
 *
 * @note 1: this function only exists on QSPY builds.
 *
 * @param   None
 * @return  tickTime: QSTimeCtr time structure (in BSP ticks).
 */
QSTimeCtr QS_onGetTime(void) {            /* invoked with interrupts locked */
   if ((SysTick->CTRL & 0x00010000) == 0) {               /* COUNT no set? */
      return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
   }
   else {     /* the rollover occured, but the SysTick_ISR did not run yet */
      return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
   }
}

/******************************************************************************/
/**
 * @brief  QS callback that flushes the serial buffer.
 *
 * This function is a callback implementation defined by QSPY that is used to
 * flush the serial buffer after it's filled up.
 *
 * @note 1: this function only exists on QSPY builds.
 *
 * @param   None
 * @return  None
 */
void QS_onFlush(void) {
   uint16_t b;
   while ((b = QS_getByte()) != QS_EOD) {      /* while not End-Of-Data... */
      while ((USART3->SR & USART_FLAG_TXE) == 0) { /* while TXE not empty */
      }
      USART3->DR = (b & 0xFF);                /* put into the DR register */
   }
}
#endif                                                             /* Q_SPY */
/*--------------------------------------------------------------------------*/

/**
 * @} end addtogroup groupBSP
 */

/**
 * @addtogroup groupISR
 * @{
 */

/******************************************************************************/
void SysTick_Handler( void )
{
   QK_ISR_ENTRY();                          /* inform QK-nano about ISR entry */
#ifdef Q_SPY
   uint32_t dummy = SysTick->CTRL;           /* clear NVIC_ST_CTRL_COUNT flag */
   QS_tickTime_ += QS_tickPeriod_;          /* account for the clock rollover */
#endif

   QF_TICK(&l_SysTick_Handler);              /* process all armed time events */

   QK_ISR_EXIT();                            /* inform QK-nano about ISR exit */
}

/**
 * @} end addtogroup groupISR
 */



/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
