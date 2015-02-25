/**
 * @file   dbg_cntrl.h
 * @brief  Declarations for debug and output functions over DMA serial console.
 * @date   09/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupDbgCntrl
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
#ifndef DBG_CNTRL_H_
#define DBG_CNTRL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Flag passed in by the makefile to determine whether to compile in the
 * subset of output functions which depend on this flag.  This, in turn sets or
 * unsets the @def DEBUG flag.  This flag determines whether some of the
 * XXX_printf() macros will be compiled in or not.
 */
#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

/**< For debugging weird problems.  Uncomment this to turn all debugging output
 * to the regular slow and blocking version.  This will also disable debugging
 * output to ethernet.
 */
//#define SLOW_PRINTF

/* Exported types ------------------------------------------------------------*/
/*! \enum DBG_LEVEL_T
 * These are the various levels of debug that are available on the system.
 */
typedef enum DBG_LEVEL {
   DBG = 0, /**< Lowest level of debugging.  Everything printed. */
   LOG,     /**< Basic logging. */
   WRN,     /**< Warnings. Always printed. */
   ERR,     /**< Critical errors.  Always printed. */
   CON,     /**< This is reserved for printing to the console as part of
                 regular operation and nothing will be prepended.  Also, the
                 DBG_LEVEL will be checked to issue a MENU signal instead of a
                 LOG signal */
   ISR,     /**< Use this with isr_debug_slow_printf to get smaller printout */
} DBG_LEVEL_T;

/*! \enum DEBUG_MOD_T
 * These are the various system modules for which debugging can be
 * enabled/disabled.
 *
 * These are limited to 32 bit and must be maskable.  These enum masks will be
 * used for both naming the various modules and for checking whether their
 * debug capabilities have been enabled.
 */
typedef enum DBG_MODULES {
   DBG_MODL_GENERAL  = 0x00000001, /**< General module debugging (main, bsp, etc) */
   DBG_MODL_SERIAL   = 0x00000002, /**< Serial module debugging. */
   DBG_MODL_TIME     = 0x00000004, /**< Time module debugging. */
   DBG_MODL_ETH      = 0x00000008, /**< Ethernet module debugging. */
   DBG_MODL_I2C      = 0x00000010, /**< I2C module debugging. */
   DBG_MODL_NOR      = 0x00000020, /**< NOR Flash module debugging. */
   DBG_MODL_SDRAM    = 0x00000040, /**< SDRAM module debugging. */
   DBG_MODL_DBG      = 0x00000080, /**< MENU module debugging. */
   DBG_MODL_COMM     = 0x00000100, /**< COMM module debugging. */
   DBG_MODL_CPLR     = 0x00000200, /**< Coupler module debugging. */
   DBG_MODL_DB       = 0x00000400, /**< Database module debugging. */
} DBG_MODL_T;

/* Exported variables --------------------------------------------------------*/
extern uint32_t  glbDbgConfig; /**< Allow global access to debug info */

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Defines a module for grouping debugging functionality.
 *
 * @description
 * Macro to be placed at the top of each C/C++ module to define the single
 * instance of the module name string to be used in printing of debugging
 * information.
 *
 * @param[in] @c name_: DBG_MODL_T enum representing the module.
 *
 * @note 1: This macro should __not__ be terminated by a semicolon.
 * @note 2: This macro MUST be present in the file if DBG_printf() or
 * LOG_printf() functions are called.  The code will not compile without this.
 */
#define DBG_DEFINE_THIS_MODULE( name_ ) \
      static DBG_MODL_T const Q_ROM DBG_this_module_ = name_;

/**
 * @brief   Enable debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_ENABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig |= name_;

/**
 * @brief   Disable debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_DISABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig &= ~name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_TOGGLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig ^= name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_CHECK_DEBUG_FOR_MODULE( name_ ) \
      ( glbDbgConfig & name_ )
/**
 * @brief   Disable debugging output for all modules.
 */
#define DBG_DISABLE_DEBUG_FOR_ALL_MODULES( ) \
      glbDbgConfig = 0x00000000;

/** @addtogroup groupDbgFast
 * @{
 */

/**
 * @brief Debug print function.
 *
 * This macro behaves just like a printf function but prepends "DBG:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * @note: This macro checks the DEBUG flag (compile time) and global debug
 * control to allow filtering.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * DBG_printf("Debug print test %d\n", i);
 *
 * will output:
 * DBG-00:04:09:459-function_name():219: Debug print test 0
 * @endcode
 *
 * @note 1: This macro is disabled in Rel builds.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define DBG_printf(fmt, ...) \
      do { \
         if (DEBUG) { \
            if ( glbDbgConfig & DBG_this_module_ ) { \
               CON_output(DBG, NA_SRC_DST, NA_SRC_DST, __func__, __LINE__, fmt, \
                  ##__VA_ARGS__); \
            } \
         } \
      } while (0)
#else
#define DBG_printf(fmt, ...) \
      do { \
         if (DEBUG) { \
            if ( glbDbgConfig & DBG_this_module_ ) { \
               CON_slow_output(DBG, __func__, __LINE__, fmt, \
                  ##__VA_ARGS__); \
            } \
         } \
      } while (0)

#endif
/**
 * @brief Logging print function.
 *
 * This macro behaves just like a printf function but prepends "LOG:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * LOG_printf("Logging print test %d\n", i);
 *
 * will output:
 * LOG-00:04:09:459-function_name():219: Logging print test 0
 * @endcode
 *
 * @note 1: This macro is disabled in Rel builds.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define LOG_printf(fmt, ...) \
      do { CON_output(LOG, NA_SRC_DST, NA_SRC_DST, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define LOG_printf(fmt, ...) \
      do { CON_slow_output(LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
/**
 * @brief Warning print function.
 *
 * This macro behaves just like a print function but prepends "WRN:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * WRN_printf("Warning print test %d\n", i);
 *
 * will output:
 * WRN-00:04:09:459-function_name():219: Warning print test 0
 * @endcode
 *
 * @note 1: This macro is enabled in Rel builds.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define WRN_printf(fmt, ...) \
      do { CON_output(WRN, NA_SRC_DST, NA_SRC_DST, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define WRN_printf(fmt, ...) \
      do { CON_slow_output(WRN, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
/**
 * @brief Error print function.
 *
 * This macro behaves just like a printf function but prepends "ERR:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * WRN_printf("Error print test %d\n", i);
 *
 * will output:
 * ERR-00:04:09:459-function_name():219: Error print test 0
 * @endcode
 *
 * @note 1: This macro is enabled in Rel builds.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define ERR_printf(fmt, ...) \
      do { CON_output(ERR, NA_SRC_DST, NA_SRC_DST, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define ERR_printf(fmt, ...) \
      do { CON_slow_output(ERR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif

/**
 * @brief Menu print function.
 *
 * This macro behaves just like a printf function.  It doesn't prepend
 * anything.
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
 * @note 1: This macro is always enabled.
 * @note 2: Use just as a regular printf with all the same flags.
 * @note 3: Don't use this for regular debugging since it will be difficult to
 * track down where this is being called since it prepends no location or
 * temporal data to help you.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#if 0
#define MENU_printf(dst, fmt, ...) \
      do {  DBG_printf("dst:%d\n", dst); \
            CON_output(CON, dst, dst, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
#else
#define MENU_printf(dst, fmt, ...) \
      do { CON_slow_output(CON, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
/**
 * @} end group groupDbgFast
 */

/** @addtogroup groupDbgSlow
 * @{
 */

/**
 * @brief Slow debug print function.
 *
 * This macro behaves just like a printf function but prepends "DBG-SLOW!", time,
 * function name, line number, and finally, the user passed in message.
 *
 * @note: This macro checks the DEBUG flag (compile time) and global debug
 * control to allow filtering.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * dbg_slow_printf("Slow debug print test %d\n", i);
 *
 * will output:
 * DBG-SLOW!-00:04:09:459-function_name():219: Slow debug print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def DBG_printf() macro instead.
 * @note 2. This macro is disabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define dbg_slow_printf(fmt, ...) \
      do { \
         if (DEBUG) { \
            if ( glbDbgConfig & DBG_this_module_ ) { \
               CON_slow_output(DBG, __func__, __LINE__, fmt, \
                     ##__VA_ARGS__); \
            } \
         } \
      } while (0)

/**
 * @brief Slow logging print function.
 *
 * This macro behaves just like a printf function but prepends "LOG-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * log_slow_printf("Slow logging print test %d\n", i);
 *
 * will output:
 * LOG-SLOW!-00:04:09:459-function_name():219: Slow logging print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def LOG_printf() macro instead.
 * @note 2. This macro is disabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define log_slow_printf(fmt, ...) \
      do { CON_slow_output(LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow warning print function.
 *
 * This macro behaves just like a printf function but prepends "WRN-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * log_slow_printf("Slow warning print test %d\n", i);
 *
 * will output:
 * WRN-SLOW!-00:04:09:459-function_name():219: Slow warning print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def WRN_printf() macro instead.
 * @note 2. This macro is enabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define wrn_slow_printf(fmt, ...) \
      do { CON_slow_output(WRN, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow error print function.
 *
 * This macro behaves just like a printf function but prepends "ERR-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * err_slow_printf("Slow error print test %d\n", i);
 *
 * will output:
 * ERR-SLOW!-00:04:09:459-function_name():219: Slow error print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def ERR_printf() macro instead.
 * @note 2. This macro is enabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define err_slow_printf(fmt, ...) \
      do { CON_slow_output(ERR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow isr debug print function.
 *
 * This macro behaves just like a printf function but prepends "D-ISR",
 * timestamp, and line number only in front of the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * isr_debug_slow_printf("Slow isr debug print test %d\n", i);
 *
 * will output:
 * D-ISR:-00:04:09:459-219:Slow isr debug print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def DBG_printf() macro instead.
 * @note 2. This macro is disabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define isr_dbg_slow_printf(fmt, ...) \
      do { if (DEBUG) CON_slow_output(ISR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

#ifdef __cplusplus
}
#endif

/**
 * @} end group groupDbgFast
 */
#endif                                                        /* DBG_CNTRL_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
