/**
 * @file bsp.c
 * @brief  This file contains the Board Support Package functions for board
 *
 * @date   06/03/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

#include "bsp.h"
#include <stdio.h>
#include <string.h>

#include "stm32f2xx.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2x7_eth_bsp.h"

#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "time.h"                          /* for processor date/time support */

Q_DEFINE_THIS_FILE;

static uint32_t l_delay = 0UL; /* limit for the loop counter in busyDelay() */
static uint32_t l_nTicks;

#ifdef Q_SPY
static uint8_t  l_SysTick_Handler;
QSTimeCtr QS_tickTime_;
QSTimeCtr QS_tickPeriod_;


#define QS_BUF_SIZE   (2*1024)
#define QS_BAUD_RATE  115200

#endif

/**
  * Initializes the BSP for the board. Should be called only once and only in
  * the beginning.  Most clocks (not all), and any device hardware bringup is
  * done here.
  * @param  None
  * @retval None
  */
void BSP_init( void ) {

	SystemInit();         /* initialize STM32 system (clock, PLL and Flash) */

	/* ENABLE GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

	/* DMA clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);

	/* Enable syscfg clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Enable TIM clocks on APB2 bus */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);

	/* Enable TIM clocks on APB1 bus */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	/* Enable CRC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

	/* Initialize the Serial for printfs to the serial port */
	Serial_Init( SYSTEM_SERIAL );

	/* Start Ethernet configuration */
	/* Assert a reset on the ETH_RST line.  This should only be released
	 * after all the local MAC and DMA configuration is done. */
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);

	/* initialize the ETH GPIO */
	ETH_GPIO_Config();

	/* initialize the ETH MACDMA */
	ETH_MACDMA_Config();

	/* Release the reset on the ETH_RST line now that the local MAC and DMA
	 * configuration is done. */
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	/* End Ethernet configuration */

	/* Initialize the time (RTC and a subsecond timer). */
   TIME_Init();
   TIME_printTime();

}

/**
 * @brief  Initializes IRQ and priority in the NVIC with the provided IRQ and
 * priority.
 *
 * @param  irq: selects the irq to assign priority to
 * @param  priority: selects the priority to assign to the specified irq
 * @retval None
 */
void NVIC_Config(uint8_t irq, uint8_t priority) {

	NVIC_InitTypeDef nvic_init;

	nvic_init.NVIC_IRQChannel                   = irq;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0xF;
	nvic_init.NVIC_IRQChannelSubPriority        = 0xF;
	nvic_init.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&nvic_init);/* enables the device and sets interrupt priority */
	NVIC_ClearPendingIRQ(irq);
	NVIC_SetPriority(irq,   		priority);

}

#ifndef NOT_NEEDED_IN_TEST_PROJ
/*..........................................................................*/
void QF_onStartup(void) {

	/* Set up and enable the SysTick timer.  It will be used as a reference
	 * for delay loops in the interrupt handlers.  The SysTick timer period
	 * will be set up for BSP_TICKS_PER_SEC.
	 */
	SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

	NVIC_Config(ETH_IRQn, ETH_PRIO);


	/* TODO: There are still the DOOR open and Sense2 sensors which are not
	 * currently being configured since they'll be set up slightly different on
	 * the new board
	 */

}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QK_onIdle(void) {

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
/*..........................................................................*/
/* error routine that is called if the STM32 library encounters an error    */
void assert_failed(char const *file, int line) {
	Q_onAssert(file, line);
}
/*..........................................................................*/
void Q_onAssert(char const * const file, int line) {
	(void)file;                                   /* avoid compiler warning */
	(void)line;                                   /* avoid compiler warning */
	QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
	for (;;) {       /* NOTE: replace the loop with reset for final version */
	}
}

/*..........................................................................*/
void BSP_busyDelay(void) {
	uint32_t volatile i = l_delay;
	while (i-- > 0UL) {                                   /* busy-wait loop */
	}
}

/*--------------------------------------------------------------------------*/
#ifdef Q_SPY
/*..........................................................................*/
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
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) {            /* invoked with interrupts locked */
	if ((SysTick->CTRL & 0x00010000) == 0) {               /* COUNT no set? */
		return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
	}
	else {     /* the rollover occured, but the SysTick_ISR did not run yet */
		return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
	}
}
/*..........................................................................*/
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

/*..........................................................................*/
/* sys_now() is used in the lwIP stack
 */
uint32_t sys_now(void) {
	return (l_nTicks * (1000 / BSP_TICKS_PER_SEC));
}


/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void BSP_Delay(__IO uint32_t nCount) {
  while(nCount--) {
  }
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void) __attribute__((__interrupt__));
void SysTick_Handler(void) {
	QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
#ifdef Q_SPY
	uint32_t dummy = SysTick->CTRL;        /* clear NVIC_ST_CTRL_COUNT flag */
	QS_tickTime_ += QS_tickPeriod_;       /* account for the clock rollover */
#endif

	QF_TICK(&l_SysTick_Handler);           /* process all armed time events */

	QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}

#endif

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
