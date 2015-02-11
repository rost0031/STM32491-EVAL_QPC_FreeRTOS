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
#include "bsp_defs.h"
#include "stm32f4xx.h"                                     /* STM32F4 support */
#include "stm32f4xx_conf.h"
#include "stm32f4x7_eth_bsp.h"                     /* DP83848 ETH PHY support */
#include "stm32f4x7_eth.h"                         /* STM32F4 ETH MAC support */
#include "qp_port.h"                                            /* QP support */
#include "project_includes.h"        /* application events and active objects */
#include "time.h"                              /* processor date/time support */
#include "i2c.h"                                               /* I2C support */
#include "serial.h"
#include "nor.h"                               /* M29WV128G NOR Flash support */
#include "sdram.h"                          /* MT48LC2M3B2B5-7E SDRAM support */
#include "projdefs.h"                          /* FreeRTOS base types support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_GENERAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static uint32_t l_nTicks;

__IO uint32_t TS_Pressed;

#ifdef Q_SPY
static uint8_t  l_SysTick_Handler;
QSTimeCtr QS_tickTime_;
QSTimeCtr QS_tickPeriod_;


#define QS_BUF_SIZE   (2*1024)
#define QS_BAUD_RATE  115200

#endif

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Initializes the IO Expander registers.
  * @param  None
  * @retval - 0: if all initializations are OK.
*/
uint32_t BSP_TSC_Init( void );

/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void BSP_init( void )
{
   /* Enable syscfg clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   /* Enable the CRC Module */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

   /* 1. Initialize the Serial for printfs to the serial port */
   Serial_Init( SERIAL_UART1 );

   /* 2. Initialize the RTC for getting time stamps. */
   TIME_Init();


   RCC_ClocksTypeDef RCC_Clocks;
   RCC_GetClocksFreq(&RCC_Clocks);
   dbg_slow_printf("Clock speed: %d\n", RCC_Clocks.SYSCLK_Frequency);

   /* 3. Initialize Ethernet */
   ETH_BSP_Config();

   /* 4. Initialize the I2C devices and associated busses */
   I2C_BusInit( I2CBus1 );

   /* 5. Initialize the NOR flash */
   NOR_Init();

   /* NOR IDs structure */
   NOR_IDTypeDef pNOR_ID;
   /* Initialize the ID structure */
   pNOR_ID.Manufacturer_Code = (uint16_t)0x00;
   pNOR_ID.Device_Code1 = (uint16_t)0x00;
   pNOR_ID.Device_Code2 = (uint16_t)0x00;
   pNOR_ID.Device_Code3 = (uint16_t)0x00;

   /* Read the NOR memory ID */
   NOR_ReadID(&pNOR_ID);
   dbg_slow_printf("NOR ID: MfgCode  : 0x%04x\n", pNOR_ID.Manufacturer_Code);
   dbg_slow_printf("NOR ID: DevCode1 : 0x%04x\n", pNOR_ID.Device_Code1);
   dbg_slow_printf("NOR ID: DevCode2 : 0x%04x\n", pNOR_ID.Device_Code2);
   dbg_slow_printf("NOR ID: DevCode3 : 0x%04x\n", pNOR_ID.Device_Code3);

   /* 6. Initialize the SDRAM  - this is already init in low_level startup code
   SDRAM_Init();
    */

   /* 7. Initialize the touchscreen */
//   dbg_slow_printf("Starting initializing touch screen\n");
//   BSP_TSC_Init();
//   dbg_slow_printf("Finished initializing touch screen\n");
}

/******************************************************************************/
void NVIC_Config(uint8_t irq, uint8_t priority)
{
   NVIC_InitTypeDef nvic_init;

   nvic_init.NVIC_IRQChannel                   = irq;
   nvic_init.NVIC_IRQChannelPreemptionPriority = 0x0;
   nvic_init.NVIC_IRQChannelSubPriority        = 0x0;
   nvic_init.NVIC_IRQChannelCmd                = ENABLE;
   NVIC_Init( &nvic_init );/* enables the device and sets interrupt priority */
   NVIC_ClearPendingIRQ( irq );
   NVIC_SetPriority( irq, priority );
}

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

   DBG_printf("Enabled ETH IRQ\n");
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

   if ((USART1->SR & USART_FLAG_TXE) != 0) {              /* is TXE empty? */
      uint16_t b;

      QF_INT_DISABLE();
      b = QS_getByte();
      QF_INT_ENABLE();

      if (b != QS_EOD) {                              /* not End-Of-Data? */
         USART1->DR = (b & 0xFF);             /* put into the DR register */
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
void assert_failed(uint8_t *file, uint32_t line)
{

   Q_onAssert((const char *)file, line);
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
   printf("ASSERT FAILED in %s at line %d\n", file, line);
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
   QF_INT_DISABLE();
   while ((b = QS_getByte()) != QS_EOD) {      /* while not End-Of-Data... */
      QF_INT_ENABLE();
      while ((USART1->SR & USART_FLAG_TXE) == 0) { /* while TXE not empty */
      }
      USART1->DR = (b & 0xFF);                /* put into the DR register */
      QF_INT_DISABLE();
   }
   QF_INT_ENABLE();
}
#endif                                                             /* Q_SPY */
/*--------------------------------------------------------------------------*/

///******************************************************************************/
//inline void BSP_SysTickCallback( void )
//{
//   QF_CRIT_STAT_TYPE intStat;
//   BaseType_t lHigherPriorityTaskWoken = pdFALSE;
//
//   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */
//
//#ifdef Q_SPY
//   uint32_t dummy = SysTick->CTRL;           /* clear NVIC_ST_CTRL_COUNT flag */
//   QS_tickTime_ += QS_tickPeriod_;          /* account for the clock rollover */
//#endif
//
////   QF_TICK(&l_SysTick_Handler);              /* process all armed time events */
//
//   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */
//
//   /* yield only when needed... */
//   if (lHigherPriorityTaskWoken != pdFALSE) {
//      vTaskMissedYield();
//   }
//}

/******************************************************************************/
void vApplicationTickHook(void)
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

#ifdef Q_SPY
   {
      uint32_t dummy = SysTick->CTRL; /* clear SysTick_CTRL_COUNTFLAG */
      QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
   }
#endif

   QF_TICK_X(0U, &l_SysTick_Handler);  /* process all armed time events */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken); /* <=== ISR exit */

   /* yield only when needed... */
   if (lHigherPriorityTaskWoken != pdFALSE) {
      vTaskMissedYield();
   }
}

/******************************************************************************/
/**
 * @brief  FreeRTOS callback that is called whenever the system is idle.
 *
 * This function is a callback for FreeRTOS that replaces the QK_onIdle() used
 * by QK kernel. It is called whenever QPC/FreeRTOS runs out of things to do and
 * is idle.  It is used by QSPY (if compiled in) to send data out to prevent
 * interfering with the system as much as possible.
 *
 * This function can also be used to visualize idle activity.
 *
 * @param   None
 * @return  None
 */
void vApplicationIdleHook( void )
{
#ifdef Q_SPY

   if ((USART1->SR & USART_FLAG_TXE) != 0) {              /* is TXE empty? */
      uint16_t b;

      QF_INT_DISABLE();
      b = QS_getByte();
      QF_INT_ENABLE();

      if (b != QS_EOD) {                              /* not End-Of-Data? */
         USART1->DR = (b & 0xFF);             /* put into the DR register */
      }
   }

#elif defined NDEBUG
   __WFI();                                          /* wait for interrupt */
#endif
}

/******************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
   (void)xTask;
   (void)pcTaskName;
   printf("ERROR: Stack overflow in stack %s\n", pcTaskName);
   Q_ERROR();
}
/**
 * @} end addtogroup groupBSP
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
