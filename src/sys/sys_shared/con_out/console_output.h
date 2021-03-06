/**
 * @file    console_output.h
 * @brief   Serial output to the UART.
 *
 * This file contains the declarations for debug and output functions over the
 * serial DMA console and over regular serial blocking.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupConOut
 * @{
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONSOLE_OUTPUT_H_
#define CONSOLE_OUTPUT_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "Shared.h"
#include "dbg_cntrl.h"                                   /* For debug control */

/* Exported defines ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Menu printf function to output menu items to serial or ethernet.
 *
 * This function is used to output menu printing out to either serial or
 * ethernet log port, depending on the origin of the menu request. The output
 * destination is specified with the @a dst variable. This function behaves just
 * like a printf function.  It doesn't prepend anything.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * MENU_printf("Menu print test %d\n", i);
 *
 * will output:
 * Menu print test 0
 * @endcode
 *
 * @note 1: This function is always enabled.
 * @note 2: Use just as a regular printf with all the same flags.
 * @note 3: Don't use this for regular debugging since it will be difficult to
 * track down where this is being called since it prepends no location or
 * temporal data to help you.
 * @note 4: don't manually set dst variable.  Instead, it should be set by the
 * requester of the menu (serial port or ethernet port).
 *
 * @param [in] dst: MsgSrc var that determines where the output will go
 *    @arg SERIAL_CON: output to serial port.
 *    @arg ETH_LOG_PORT: output to the ethernet port.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
void MENU_printf(
      volatile MsgSrc dst,
      char *fmt,
      ...
);

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
 * @param [in] src: MsgSrc var specifying the source of the data.
 *    @arg NA_SRC_DST: no src/dst
 *    @arg SERIAL_CON: data is to or from the serial console
 *    @arg ETH_PORT_SYS: data is to or from the system TCP port
 *    @arg ETH_PORT_LOG: data is to or from the logging TCP port
 *
 * @param [in] dst: MsgSrc var specifying the destination of the data.
 *    @arg NA_SRC_DST: no src/dst
 *    @arg SERIAL_CON: data is to or from the serial console
 *    @arg ETH_PORT_SYS: data is to or from the system TCP port
 *    @arg ETH_PORT_LOG: data is to or from the logging TCP port
 *
 * @param [in] pFuncName: const char* pointer to the function name where the
 * macro was called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] fmt: const char* pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_output(
      DBG_LEVEL_T dbgLvl,
      volatile MsgSrc src,
      volatile MsgSrc dst,
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
 * @brief   Convert a uint8_t hex array to a string array.
 *
 * @note: passed in buffer for string output should be at least 5x the size of
 * the original data.
 *
 * @param [in] hexData: const char* pointer to the buffer that contains the hex
 * data to convert.
 * @param [in] hexDataLen: uint16_t length of data in the hexData buffer.
 * @param [in,out] strDataBuffer: char* pointer to the buffer where to write the
 * result.  This should be allocated by the caller.
 * @param [in] strDataBufferSize: uint16_t max length of the strDataBuffer.
 * @param [in,out] strDataLen: uint16_t* pointer to the size of the data in the
 * strDataBuffer after it has been written.
 * @param [in] outputNColumns: uint8_t number of columns to break up the
 * resulting string into with newlines. If set to zero, a single long row is
 * returned without any newlines.
 * @param [in] sep: char that will be used to separate all the printed hex numbers.
 * @param [in] bPrintX: bool that specifies whether to print 0x in front of each
 * hex number.
 * @return CBErrorCode: ERR_NONE if OK.
 */
CBErrorCode CON_hexToStr(
      const uint8_t* hexData,
      uint16_t hexDataLen,
      char* strDataBuffer,
      uint16_t strDataBufferSize,
      uint16_t* strDataLen,
      uint8_t outputNColumns,
      const char sep,
      const bool bPrintX
);

/**
 * @} end group groupConOut
 */

#endif                                                   /* CONSOLE_OUTPUT_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
