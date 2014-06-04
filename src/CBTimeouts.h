// $Id$
/**
 * @file 	CBTimeouts.h
 * Contains all the timeout values for the coupler board
 *
 * @date   	06/02/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */
// $Log$

#ifndef CBTIMEOUTS_H_
#define CBTIMEOUTS_H_

/**
 * Timeout naming conventions:
 * Timeouts should be named in the following fashion:
 *
 * 1. Is the timeout high level or low level?  Which AOs are using it
 * 2. Is it a maximum or a minimum?
 * 3. Is it a timeout or just used to time something?
 * 4. Units: seconds (SEC), minutes (MIN), or other.
 * 5. Name of the timeout and who/how/etc it's used.
 *
 * {HL/LL}_{MAX/MIN/AVE}_{TIMEOUT/TIME}_{SEC/MIN/MS}_NAME_AND_USE
 *
 * Example:  HL_MAX_TIMEOUT_SEC_QUICK_HEATER_CHECK
 *
 * HL: indicates that this is a high level timeout and is used by a top level
 * Active Object and is visible to the printer.
 * MAX: this is the maximum amount of time that is allowed for the action being
 * timed with this timeout.
 * TIMEOUT: this is a timeout and not simply used to time how long something
 * takes or a polling time.
 * SEC: this timeout is using units of "seconds"
 * NAME: QUCK_HEATER_CHECK - this is the function of this timeout.  It is used
 * to make sure that the check of temperature will not take too long.
 * */

/** \name Coupler Board Timeouts and Times
 * These are the timeouts used by all the state machines in the coupler board.
 *@{*/

   /** \name Serial Timeout Maximums
    * These are the timeouts used by the low level SerialMgr AO.
    *@{*/
   #define LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC                              1.0
   /*@} Serial Timeout Maximums*/


/*@} Coupler Board Timeout Maximums*/
#endif                                                       /* CBTIMEOUTS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
