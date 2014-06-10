/**
 * @file   console_output.h
 * This file contains the declarations for debug and output functions over the
 * serial dma console.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

#ifndef CONSOLE_OUTPUT_H_
#define CONSOLE_OUTPUT_H_

#include <stdint.h>
#include <stdarg.h>

/* These two macros are handy for debugging and will be disabled if NDEBUG is not set */
/* This is the regular debug print function which includes more info */
#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

/*! \enum DEBUG_LEVEL_T
 * These are the various levels of debug that are available on the system.
 */
typedef enum DEBUG_LEVEL {
   DBG = 0, /*!< Lowest level of debugging.  Everything printed. */
   LOG,     /*!< Basic logging. */
   WRN,     /*!< Warnings */
   ERR,     /*!< Critical errors.  Always printed. */
   CON      /*!< This is reserved for printing to the console as part of regular
                 operation and nothing will be prepended */
} DEBUG_LEVEL_T;

/**
  * Basic console output function which should be called by the various macro
  * functions to do the actual output to serial.  Takes in parameters that allow
  * easy logging level specification, file, function name, line number, etc.
  * These are prepended in front of the data that was actually sent in to be
  * printed.
  *
  * Note: when printing to a @CON debug level, no data is prepended to the front
  * of the buffer.  This should be used to do menu output to the console.
  *
  * Note: Instead of directly printing to the serial console, it creates a
  * SerDataEvt and sends the data to be output to serial via DMA.  This prevents
  * slow downs due to regular printfs to serial.
  *
  * @param  [in] dbgLvl: a DEBUG_LEVEL_T variable that specifies the logging
  * level to use.
  *   @arg DBG: Lowest level of debugging.  Everything above this level is
  *   printed.  Disabled in Release builds.
  *   @arg LOG: Basic logging. Everything above this level is printed.
  *   Disabled in Release builds.
  *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
  *   Release builds.
  *   @arg ERR: Errors. Enabled in all builds.
  *
  * @param [in] *pFilePath: pointer to the file and path from the macro where
  * this is called.  Since this currently includes the path and filename, it is
  * not printed due to being too long.
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
      DEBUG_LEVEL_T dbgLvl,
      char *pFilePath,
      char *pFuncName,
      uint16_t wLineNumber,
      const char *fmt,
      ...
);

/**
 * Use the following macros for printf style debugging.  Some of them will be
 * automatically disabled via macros when a "rel" build is done (as opposed to
 * a dbg or a spy build).
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * DBG_printf("Debug print test\n");
 * DBG_printf("Debug print test %d\n",i);
 * LOG_printf("Logging print test %d\n",i);
 * WRN_printf("Warning print test %d\n",i);
 * ERR_printf("Error print test %d\n",i);
 * MENU_printf("Console print test %d\n",i);
 * @endcode
 *
 * will output:
 * DBG-00:04:09:00459-LWIPMgr_Running():219: Debug print test
 * DBG-00:04:09:00459-LWIPMgr_Running():219: Debug print test 0
 * LOG-00:04:09:00459-LWIPMgr_Running():219: Logging print test 0
 * WRN-00:04:09:00459-LWIPMgr_Running():219: Warning print test 0
 * ERR-00:04:09:00459-LWIPMgr_Running():219: Error print test 0
 * Console print test 0
 *
 */

/**
 * Debug print function.  Turned off in Rel builds.  Use just as a regular
 * printf with all the same flags.  Output will prepend "DBG:", time, function
 * name, line number, and finally, the user specified data.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
#define DBG_printf(fmt, ...) \
      do { if (DEBUG) CON_output(DBG, __FILE__, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * Logging print function.  Turned off in Rel builds.  Use just as a regular
 * printf with all the same flags.  Output will prepend "LOG:", time, function
 * name, line number, and finally, the user specified data.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
#define LOG_printf(fmt, ...) \
      do { if (DEBUG) CON_output(LOG, __FILE__, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * Warning print function.  Use just as a regular printf with all the same flags.
 * Output will prepend "WRN:", time, function name, line number, and finally,
 * the user specified data.
 *
 * Note: this should only be used on non-critical errors. (Buffer limits, etc)
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
#define WRN_printf(fmt, ...) \
      do { CON_output(WRN, __FILE__, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * Error print function.  Use just as a regular printf with all the same flags.
 * Output will prepend "ERR:", time, function name, line number, and finally,
 * the user specified data.
 *
 * Note: This should only be used on critical error paths since it stays enabled.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
#define ERR_printf(fmt, ...) \
      do { CON_output(ERR, __FILE__, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * Menu print function.  Use just as a regular printf with all the same flags.
 * Output will not prepend anything and just output what the user specifies.
 * This function should be used to print menu items and other user interface
 * stuff.
 *
 * Note: Don't use this for regular debugging since it will be difficult to track
 * down where this is being called since it prepends no data to help you.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
#define MENU_printf(fmt, ...) \
      do { CON_output(CON, __FILE__, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

#endif                                                   /* CONSOLE_OUTPUT_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
