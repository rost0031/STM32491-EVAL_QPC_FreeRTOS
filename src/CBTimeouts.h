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
 * 3. Is it a timeout (TOUT) or just used to time something (TIME)?
 * 4. Units: seconds (SEC), minutes (MIN), or other.
 * 5. Name of the timeout and who/how/etc it's used.
 *
 * {HL/LL}_{MAX/MIN/AVE}_{TOUT/TIME}_{SEC/MIN/MS}_NAME_AND_USE
 *
 * Example:  HL_MAX_TOUT_SEC_QUICK_HEATER_CHECK
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

   /** \name Serial Timeouts and Times.
    * These are the timeouts used by the low level SerialMgr AO.
    *@{*/
   #define LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC                                 1.0
   /*@} Serial Timeout Maximums*/

   /** \name I2C Bus Timeouts and Times.
    * These are the timeouts used by the low level I2CBusMgr AO.
    *@{*/
   #define LL_MAX_TOUT_SEC_I2C_BASIC_OP                                       0.2
   #define LL_MAX_TOUT_SEC_I2C_BUS_RECOVERY     ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 6 )  /**< How long the recovery is expected to take */
   #define LL_MAX_TIME_SEC_I2C_BUS_SETTLE       ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 3 )  /**< How long bus settle should take */
   #define LL_MAX_TOUT_SEC_I2C_GLOBAL           ( LL_MAX_TOUT_SEC_I2C_BUS_RECOVERY * 3 )
   #define LL_MAX_TOUT_SEC_I2C_MEM_READ         ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 3 )
   #define LL_MAX_TOUT_SEC_I2C_MEM_WRITE        ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 3 )
   #define LL_MAX_TOUT_SEC_I2C_READ_OP          ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 4 )
   #define LL_MAX_TOUT_SEC_I2C_WRITE_OP         ( LL_MAX_TOUT_SEC_I2C_BASIC_OP * 4 )
   /*@} I2C Timeouts */

   /** \name I2C1Dev Timeouts and Times.
    * These are the timeouts used by the higher level I2C1DevMgr AO.
    * These should be based on the LL I2CBusMgr timeouts.
    *@{*/
   #define HL_MAX_TOUT_SEC_I2C_DEV_OP                                         0.3
   #define HL_MAX_TOUT_SEC_I2C_DEV_REQ                                        10.0
   #define HL_MAX_TOUT_SEC_I2C_BUS_CHECK        ( LL_MAX_TOUT_SEC_I2C_BUS_RECOVERY * 1.3 )
   #define HL_MAX_TOUT_SEC_I2C_EV5              ( HL_MAX_TOUT_SEC_I2C_DEV_OP * 1.3 )
   #define HL_MAX_TOUT_SEC_I2C_EV6              ( HL_MAX_TOUT_SEC_I2C_DEV_OP * 1.3 )
   #define HL_MAX_TOUT_SEC_I2C_EV8              ( HL_MAX_TOUT_SEC_I2C_DEV_OP * 1.3 )
   #define HL_MAX_TOUT_SEC_I2C_READ             ( HL_MAX_TOUT_SEC_I2C_DEV_OP * 1.3 )
   #define HL_MAX_TOUT_SEC_I2C_WRITE            ( HL_MAX_TOUT_SEC_I2C_DEV_OP * 1.3 )
   #define HL_MAX_TIME_MS_I2C_POST_WRITE                                      5.0 // 5ms post write wait on the I2C EEPROM.
   /*@} I2C1Dev Timeouts and Times. */

   /** \name ETH Timeouts and Times.
    * These are the timeouts used by the low level LWIPMgr AO.
    *@{*/
   #define LL_MAX_TIMEOUT_TCP_SEND_SEC                                        3.0
   /*@} ETH Timeout Maximums*/

   /** \name GUI Timeouts and Times.
    * These are the timeouts used by the GuiMgr AO.
    *@{*/
   #define LL_MAX_TIME_SEC_LCD_UPDATE                                         0.1
   /*@} GUI Timeouts and Times.*/

/*@} Coupler Board Timeout Maximums*/


/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */

#endif                                                       /* CBTIMEOUTS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
