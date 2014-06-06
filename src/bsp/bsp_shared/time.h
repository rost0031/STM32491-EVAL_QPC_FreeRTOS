// $Id$
/**
 * @file   rtc.h
 * @brief  This file contains the definitions for functions to control on the
 * Redwood L1 board time and timestamps.
 *
 * @date   10/22/2013
 * @author Harry Rostovtsev
 * @email  Harry_Rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$

#ifndef TIME_H__
#define TIME_H__

#include "stm32f2xx_rtc.h"

/* Basic time structure that contains the STM32 struct for hours, min, and seconds
 * and adds a subsecond counter. */
typedef struct
{
   RTC_TimeTypeDef   hour_min_sec;
   uint32_t          sub_sec;
}t_Time;

/**
  * Initializes the RTC and a subsecond timer. Should be called only once and
  * only in the beginning.  This init function sets up the realtime clock to be
  * used for timestamping.
  * @param  None
  * @return None
  */
void TIME_Init( void );

/**
  * Initializes the subsecond timer (TIM7). Should be called only once and
  * only in the beginning by TIME_Init().  This init function sets up TIM7 to
  * be used as a microsecond timer
  * @param  None
  * @return None
  */
void TIME_subSecondTimer_Init( void );

/**
  * Print the current time. (hours, minutes, seconds, subseconds).
  * @param  None
  * @retval None
  */
void TIME_printTime( void );

/**
  * Return a structure containing the current time.
  * @param  None
  * @retval time: a t_Time structure containing the current time.
  */
t_Time TIME_getTime( void );

#endif                                                            /* TIME_H__ */
