/**
 * @file   project_includes.h
 * @brief  Doxygen main page and project specific includes.
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
 * \mainpage STM324x9I-EVAL2 QPC port with LWIP.
 *
 * <b>Introduction</b>
 *
 * This user manual describes the FW for the STM324x9I-EVAL2 developement kit 
 * from ST Micro.
 *
 * The FW is divided into a number of modules each covering a specific category:
 * - Application
 *    - TODO: Add application submodules and descriptions here.
 * - BSP - Bootloader specific BSP that pulls in the shared BSP
 *    - Shared BSP
 *       - CMSIS
 *       - STM32F4xx device drivers
 *       - Serial/UART
 *       - Time/RTC
 *       - Ethernet (STM32 MAC) driver
 *       - QPC implementation of the LWIP network driver interface
 *       - STM32 specific init and optimizations.
 *    - Flash driver
 *    - DP83848 Ethernet PHY driver
 *    - ISR implementations
 *    - Linker script
 * - System - RTOS, network stacks, encryption stacks, console debugging, etc.
 *    - QPC RTOS/Framework
 *    - LWIP network stack
 *    - Shared System
 *       
 *       
 *
 * <b>Processor and board support</b>
 * TODO: Update this when the final board is released.
 * The code is completely written in C and is fully CMSIS compliant.
 *
 * <b>Toolchain Support</b>
 *
 * The code has been developed and tested with CodeSourcery Lite before being
 * upgraded to gcc-arm-none-eabi-4_6-2012q4.
 * @note: Newer version of gcc-arm should not be used due to a bug in the newer
 * versions of the compiler.  If you do upgrade, you will not be able to use the
 * custom ASM STM32 specific and very fast version of MEMCPY that comes with
 * this project.
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
 * @defgroup groupApp Application
 */
   /**
    * @defgroup groupComm Communication System
    * @ingroup groupApp
    */

   /**
    * @defgroup groupMenu Menu System
    * @ingroup groupApp
    */

/**
 * @defgroup groupBSP BSP
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
       * @defgroup STM32F4xx_StdPeriph_Driver STM32F4XX Std Peripheral Drivers
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupSerial UART/Serial
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupI2C I2C Bus Driver
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupTime Time/RTC
       * @ingroup groupSharedBSP
       */

      /**
       * @defgroup groupEthernet Ethernet (DP83848 PHY) driver.
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
    * @defgroup groupLinker Linker script specific to this image (can be different depending on whether used for the bootloader or application).
    * @ingroup groupBSP
    */

/**
 * @defgroup groupSYS System level software.  RTOSes, IP stacks, debugging frameworks, etc.
 */

   /**
    * @defgroup groupQPC QPC RTOS/Framework
    * @ingroup groupSYS
    */

   /**
    * @defgroup groupLWIP LWIP TCP/IP network stack
    * @ingroup groupSYS
    */
   
   /**
    * @defgroup groupSharedSYS Shared system level files
    * @ingroup groupSYS
    */
   
       /**
        * @defgroup groupDbgCntrl Debugging Framework
        * @ingroup groupSharedSYS
        */
           
           /**
            * @defgroup groupConOut Console output framework.
            * @ingroup groupDbgCntrl
            */
   
           /**
            * @defgroup groupDbgFast Fast (non-blocking and buffered) Debugging framework.
            * @ingroup groupDbgCntrl
            */

           /**
            * @defgroup groupDbgSlow Slow (blocking and non-buffered) Debugging framework
            * @ingroup groupDbgCntrl
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
