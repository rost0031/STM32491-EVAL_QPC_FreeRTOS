/**
 * @file 	CBTimeouts.h
 * @brief   Contains all the timeout values for the coupler board
 *
 * @date   	06/02/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBTIMEOUTS_H_
#define CBTIMEOUTS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

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
 * NAME: QUICK_HEATER_CHECK - this is the function of this timeout.  It is used
 * to make sure that the check of temperature will not take too long.
 * */

/** \name Coupler Board Timeouts and Times
 * These are the timeouts used by all the state machines in the coupler board.
 *@{*/

   /** \name Serial Timeouts
    * These are the timeouts used by the low level SerialMgr AO.
    *@{*/
   #define LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC                              1.0
   /*@} Serial Timeout Maximums*/

   /** \name I2C Timeouts
    * These are the timeouts used by the low level I2CMgr AO.
    *@{*/
   #define LL_MAX_TIMEOUT_I2C_BUS_BUSY_RETRY_SEC                           0.01
   #define LL_MAX_TIMEOUT_I2C_BUSY_SEC                                     3.0
   #define LL_MAX_TIMEOUT_I2C_READ_OP_SEC                                  3.0
   #define LL_MAX_TIMEOUT_I2C_WRITE_OP_SEC                                 3.0
   /*@} I2C Timeouts */


/*@} Coupler Board Timeout Maximums*/


/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */

#endif                                                       /* CBTIMEOUTS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
