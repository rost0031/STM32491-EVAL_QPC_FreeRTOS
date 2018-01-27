/**
 * @file   rtc.h
 * @brief  This file contains the definitions for functions to control on the
 * Redwood L1 board time and timestamps.
 *
 * @date   10/22/2013
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupTime
 * @{
 *
 * This module is responsible for setting up the Real-time clock (RTC) for time
 * keeping along with a subsecond timer that measures approximately 10000 ticks
 * per second ( 1/10 of a millisecond ).
 *
 * This module also calibrates the prescalar to the RTC by measuring the RTC
 * clock frequency and adjusting for any drift.  TIM5 is used for this and is
 * disabled immediately after so it could be used later if desired.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TIME_H__
#define TIME_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * \struct time_T
 * Basic time structure that contains the STM32 struct for hours, min, and
 * seconds and adds a subsecond counter.
 */
typedef struct
{
   RTC_TimeTypeDef   hour_min_sec;   /**< STM32 Time struct with h, m, and s. */
   uint32_t          sub_sec;    /**< uint32_t subsecond timer from 0 - 1000. */
}time_T;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Initializes the RTC and a subsecond timer.
 *
 * This function:
 *   -# initializes the RTC.
 *   -# calibrates the RTC using TIM5 to account for clock drift.
 *   -# initializes a subsecond timer using TIM7.
 *
 * @note 1: This function should be called only once and only in the beginning.
 *
 * @param  None
 * @return None
 */
void TIME_Init( void );

/**
 * @brief   Return a structure containing the current time.
 * @param  None
 * @return time: a time_T structure containing the current time.
 */
time_T TIME_getTime( void );

/**
 * @}
 * end addtogroup groupTime
 */

#endif                                                            /* TIME_H__ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
