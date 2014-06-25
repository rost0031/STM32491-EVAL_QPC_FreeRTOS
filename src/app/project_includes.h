/**
 * @file   project_includes.h
 * @brief  Project specific includes.
 * This file contains some common includes needed by multiple files.  This file
 * is project specific and should not be shared by multiple modules.  Each
 * project, whether embedded app, bootloader, or simulator, should have its own
 * implementation of this file.
 *
 * @date   06/19/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PROJECT_INCLUDES_H_
#define PROJECT_INCLUDES_H_

/**
 * \mainpage Coupler Board Bootloader Firmware
 *
 * <b>Introduction</b>
 *
 * This user manual describes the Coupler Board Bootloader FW.
 *
 * The FW is divided into a number of modules each covering a specific category:
 * - Application
 *    - TODO: Add application submodules and descriptions here.
 * - BSP - Bootloader specific BSP that pulls in the shared BSP
 *    - Shared BSP
 *       - CMSIS
 *       - STM32F2xx device drivers
 *       - Serial/UART
 *       - Time/RTC
 *       - Ethernet (KSZ8863 MAC-MAC switch) driver
 *    - Flash driver
 *    - ISR implementations
 *    - Linker script
 * - System - RTOS, network stacks, encryption stacks, etc.
 *    - QPC RTOS/Framework
 *    - LWIP network stack
 *    - Shared System
 *       - QPC implementation of the LWIP network driver interface
 *       - STM32 specific init and optimizations.
 *
 * <b>Processor and board support</b>
 * TODO: Update this when the final board is released.
 * The code is completely written in C and is fully CMSIS compliant.  All BSP
 * code is STM32F217 compatible using an external oscillator.  The code will not
 * run without minor modifications on the STM3220G-Eval board.
 *
 * <b>Toolchain Support</b>
 *
 * The code has been developed and tested with CodeSourcery Lite before being
 * upgraded to gcc-arm-none-eabi-4_6-2012q4.  Newer version of gcc-arm should
 * not be used due to a bug in the newer versions of the compiler.
 *
 *
 * <b>Building the Bootloader</b>
 *
 * The project contains a Makefile which allows building of the entire bootloader
 * as well as any libraries and shared code used by it.
 *
 * make cleanall - Completely wipes out all objects, binaries, and dependency
 * files for all libraries and the top level project.
 * make clean - Wipes out just the binaries used by the top level project, leaving
 * shared code and library prebuilt binaries alone.
 *
 * make all - Builds a debug version.  Leaves certain logging over UART enabled.
 * make CONF=rel - Builds a release version.  This disables DBG and LOG level
 * UART debugging statements.
 *
 * <b>Copyright Notice</b>
 *
 * Copyright (C) 2014 Datacard. All rights reserved.
 */


/**
 * @defgroup groupApp Bootloader Application
 */

/**
 * @defgroup groupBSP Bootloader BSP
 */

   /**
    * @defgroup groupSharedBSP Shared BSP
    * @ingroup groupBSP
    */

      /**
       * @defgroup CMSIS CMSIS
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup STM32F2xx_StdPeriph_Driver STM32F2XX Std Peripheral Drivers
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupSerial UART/Serial
       * @ingroup groupSharedBSP
       */

         /**
          * @defgroup groupSerialFast Fast DMA Serial output functions
          * @ingroup groupSerial
          */

         /**
          * @defgroup groupSerialSlow Slow blocking Serial output functions
          * @ingroup groupSerial
          */

      /**
       * @defgroup groupTime Time/RTC
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupKSZ8863 Ethernet (KSZ8863 MAC-MAC switch) driver.
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupLWIP_QPC_Eth QPC implementation of the LWIP network driver interface
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupSTM32runtime STM32 init, runtime, and optimizations.
       * @ingroup groupSharedBSP
       */

   /**
    * @defgroup groupFlash Flash Support
    * @ingroup groupBSP
    */

   /**
    * @defgroup groupISR ISR implementations
    * @ingroup groupBSP
    */

   /**
    * @defgroup groupLinker Linker script
    * @ingroup groupBSP
    */

/**
 * @defgroup groupSYS Bootloader System
 */

   /**
    * @defgroup groupQPC QPC RTOS/Framework
    * @ingroup groupSYS
    */

   /**
    * @defgroup groupLWIP LWIP network stack
    * @ingroup groupSYS
    */


/* Includes ------------------------------------------------------------------*/
#include "mem_datacopy.h"      /* Very fast STM32 specific MEMCPY declaration */
#include "CBSignals.h"                                /* Signal declarations. */
#include "CBTimeouts.h"                             /* Timeouts declarations. */
#include "console_output.h"         /* For debug/console output declarations. */
#include "time.h"                                   /* For time functionality */

/* Exported defines ----------------------------------------------------------*/
/**
 * \def This define allows functions to be placed into RAM by the linker and
 * startup code.
 */
#define RAMFUNC __attribute__ ((long_call, section (".ramfunctions")))

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#endif                                                 /* PROJECT_INCLUDES_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
