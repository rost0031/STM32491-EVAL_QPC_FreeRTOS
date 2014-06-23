/**
 * @file   bootloader_info.h
 * @brief  This file contains information about the bootloader that needs to be
 * accessible from both the bootloader and the application.
 *
 * These variables define the location and size of storage allocated by the
 * bootloader linker script that stores the FW_VER and BUILD_DATE of the
 * bootloader.  Reading from these locations allows the application to access
 * this information.  Older versions of the bootloader may not have this info,
 * in which case, there will be 0xFF in these locations.  The application should
 * report some fixed version of the bootloader that didn't have this information
 * to prevent unexpected behavior.
 *
 * @date   10/15/2013
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOTLOADER_INFO_H__
#define BOOTLOADER_INFO_H__

/** @addtogroup groupSharedBSP
 * @{
 */

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

/**
 * \def Flash address of the location where the Bootloader FW version resides.
 * Note 1: This value MUST match the .btldrInfo address in the linker script
 * Note 2: This value is specific to the STM32F2x7 family processors and to the
 * flash size.
 */
#define FLASH_BOOTLDR_FW_VER_ADDRESS      0x0801FC00

/**
 * \def Number of bytes taken up by the Bootloader FW version.
 */
#define FLASH_BOOTLDR_FW_VER_SIZE         32

/**
 * \def Flash address of the location where the Bootloader FW build date resides.
 * Note: This value MUST match the .btldrInfo address in the linker script
 */
#define FLASH_BOOTLDR_FW_DATE_ADDRESS     ( FLASH_BOOTLDR_FW_VER_ADDRESS + \
                                            FLASH_BOOTLDR_FW_VER_SIZE )

/**
 * \def Number of bytes taken up by the Bootloader FW build date.
 */
#define FLASH_BOOTLDR_FW_DATE_SIZE        FLASH_BOOTLDR_FW_VER_SIZE

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @} end addtogroup groupSharedBSP
 */

#endif                                                 /* BOOTLOADER_INFO_H__ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
