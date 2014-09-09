/**
 * @file   console_output.h
 * @brief  Declarations for debug and output functions over DMA serial console.
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSerial
 * @{
 * <b> Introduction </b>
 *
 * The 2 sets of macros defined here should be used for all output to the serial
 * debug console.  The 2 categories of printf-like macros are:
 *
 *    -# XXX_printf()
 *    -# xxx_slow_printf().
 *
 * <b> XXX_printf macro description </b>
 *
 * The XXX_printf() macros should only be used after the BSP has been properly
 * initialized and the RTOS (QPC) running.  These macros are non-blocking and
 * return almost instantly, causing very little speed decrease in the running of
 * the actual application.  Instead of waiting until the data is done printing
 * over the serial port, these macros copy the data to an event which gets
 * published and received by SerialMgr Active Object (AO).  SerialMgr AO then
 * either:
 *    - Sets up a DMA write out to the serial console and the data then proceeds
 *    to get output.
 *    - Queues up the event if it's currently busy doing a previous DMA serial
 *    write and takes care of it when it's finished.
 *
 * @note 1: Since the buffer space in SerialMgr is finite, if you have too many
 * consecutive calls to a XXX_printf() macro, you can overrun the queue.  This
 * will cause an Q_assert.
 *
 * @note 2: SerialMgr AO must be up and the BSP must have finished configuring
 * DMA serial interface.  Before this point, only the xxx_slow_debug() macros
 * should be used.
 *
 * @note 3: Some XXX_printf macros are automatically compiled out when doing a
 * release build.
 *
 * <b> XXX_printf macro usage and examples </b>
 *
 * Use the following macros for printf style debugging.  Some of them will be
 * automatically disabled via macros when a "rel" build is done (as opposed to
 * a dbg or a spy build).
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * DBG_printf("Debug print test %d\n",i);
 * LOG_printf("Logging print test %d\n",i);
 * WRN_printf("Warning print test %d\n",i);
 * ERR_printf("Error print test %d\n",i);
 * MENU_printf("Console print test %d\n",i);
 *
 * will output:
 * DBG-00:04:09:00459-function_name():219: Debug print test 0
 * LOG-00:04:09:00459-function_name():219: Logging print test 0
 * WRN-00:04:09:00459-function_name():219: Warning print test 0
 * ERR-00:04:09:00459-function_name():219: Error print test 0
 * Console print test 0
 * @endcode
 *
 * <b> xxx_slow_printf macro description </b>
 *
 * These macros print printing directly to the serial console and are fairly
 * slow.  Use the DBG/LOG/WRN/ERR_printf macros instead.  These macros should
 * only in the initializations before the QPC RTOS is running and should
 * not be used after.
 *
 * <b> xxx_slow_printf macro usage and examples </b>
 *
 * Use the following functions for printf style debugging only before the AOs of
 * the QPC have been started.  These output directly to the serial console
 * without using DMA and will adversely affect the performance of the system.
 * These will be automatically disabled via macros when a "rel" build is done
 * (as opposed to a dbg or a spy build). xxx_slow_printf will print a lot more info
 * than isr_debug_printf
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * dbg_slow_printf("Debug print test\n");
 * dbg_slow_printf("Debug print test %d\n",i);
 * log_slow_printf("Logging print test %d\n",i);
 * wrn_slow_printf("Warning print test %d\n",i);
 * err_slow_printf("Error print test %d\n",i);
 *
 *
 * will output:
 * DBG-SLOW!-00:04:09:00459-function_name():219: Debug print test
 * DBG-SLOW!-00:04:09:00459-function_name():219: Debug print test 0
 * LOG-SLOW!-00:04:09:00459-function_name():219: Logging print test 0
 * WRN-SLOW!-00:04:09:00459-function_name():219: Warning print test 0
 * ERR-SLOW!-00:04:09:00459-function_name():219: Error print test 0
 * @endcode
 *
 *
 * @code
 * isr_debug_slow_printf("ISR Debug print test\n");
 * isr_debug_slow_printf("ISR Debug print test %d\n", i);
 *
 * will output:
 * D ISR Debug print test
 * D ISR Debug print test 0
 *
 * @endcode
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONSOLE_OUTPUT_H_
#define CONSOLE_OUTPUT_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "dbg_cntrl.h"                                   /* For debug control */

/* Exported defines ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Function that gets called by the XXX_printf() macros to output a
 * dbg/log/wrn/err to DMA serial console.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 *
 * Function performs the following steps:
 *    -# Gets the timestamp.  This timestamp represents when the call was
 *    actually made since by the time it's output, time can/will have passed.
 *    -# Constructs a new msg event pointer and allocates storage in the QP
 *    event pool.
 *    -# Decides the output format based on which macro was called DBG_printf(),
 *    LOG_printf(), WRN_printf(), ERR_printf(), or CON_printf() and writes it to
 *    the event pointer msg buffer and sets the length in the event pointer.
 *    -# Pass the va args list to get output to a buffer, making sure to not
 *    overwrite the prepended data.
 *    -# Append the actual user supplied data to the buffer and set the length.
 *    -# Publish the event and return.  The event will be handled (queued or
 *     executed) by SerialMgr AO when it is able to do.  See @SerialMgr
 *     documentation for details.
 *
 * @note 1: Do not call this function directly.  Instead, call on of the
 * DBG/LOG/WRN/ERR/CON_printf() macros.  Printout from these looks something
 * like:
 * DBG_LEVEL-HH:MM:SS:XXX-SomeFunctionName():fileLineNumber: User message here
 *
 * @note 2: when printing to a @CON debug level, no data is prepended to the front
 * of the buffer.  This should be used to do menu output to the console.
 *
 * @note 3: Instead of directly printing to the serial console, it creates a
 * SerDataEvt and sends the data to be output to serial via DMA.  This prevents
 * slow downs due to regular printf() to serial.
 *
 * @param  [in] dbgLvl: a DBG_LEVEL_T variable that specifies the logging
 * level to use.
 *   @arg DBG: Lowest level of debugging.  Everything above this level is
 *   printed.  Disabled in Release builds.  Prints "DBG" in place of "DBG_LEVEL".
 *   @arg LOG: Basic logging. Everything above this level is printed.
 *   Disabled in Release builds. Prints "LOG" in place of "DBG_LEVEL".
 *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
 *   Release builds. Prints "WRN" in place of "DBG_LEVEL".
 *   @arg ERR: Errors. Enabled in all builds. Prints "ERR" in place of "DBG_LEVEL".
 *   @arg CON: Regular output to the console without prepending anything.
 *   Enabled in all builds. Just the "User message here" will be printed.  This
 *   is meant to output serial menu items.
 *
 * @param [in] *pFuncName: pointer to the function name where the macro was
 * called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_output(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
);

/**
 * @brief Function called by the slow_xxx_printf() macros.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 * 1. Gets the timestamp.  This timestamp represents when the call was actually
 * made since by the time it's output, time can/will have passed.
 * 2. Decides the output format based on which macro was called
 * slow_dbg_printf(), slow_log_printf(), slow_wrn_printf(), slow_err_printf(),
 * or isr_slow_debug_printf() and writes it to a temporary buffer.
 * 3. Pass the va args list to get output to a buffer, making sure to not
 * overwrite the prepended data.
 * 4. Performs a blocking write of the buffer to the serial port without using
 * DMA and returns.
 *
 * @note 1: when printing to a @CON debug level, nothing is output.
 *
 * @note 2: This function prints directly printing to the serial console and is
 * fairly slow.  Use the xxx_slow_printf macros instead.  This function is only
 * called by the slow macros which have to be called in the initializations
 * before the QPC RTOS is running and should not be used after.
 *
 * @param  [in] dbgLvl: a DBG_LEVEL_T variable that specifies the logging
 * level to use.
 *   @arg DBG: Lowest level of debugging.  Everything above this level is
 *   printed.  Disabled in Release builds.
 *   @arg LOG: Basic logging. Everything above this level is printed.
 *   Disabled in Release builds.
 *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
 *   Release builds.
 *   @arg ERR: Errors. Enabled in all builds.
 *
 * @param [in] *pFuncName: pointer to the function name where the macro was
 * called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_slow_output(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
);

/**
 * @} end addtogroup groupSerial
 */

#endif                                                   /* CONSOLE_OUTPUT_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
