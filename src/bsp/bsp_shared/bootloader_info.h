/**
 * @file   bootloader_info.h
 * @brief  This file contains information about the bootloader that needs to be
 * accessible from both the bootloader and the application.
 *
 * @date   10/15/2013
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupSharedBSP
 * @{
 * These variables define the location and size of storage allocated by the
 * bootloader linker script that stores the FW_VER and BUILD_DATE of the
 * bootloader.  Reading from these locations allows the application to access
 * this information.  Older versions of the bootloader may not have this info,
 * in which case, there will be 0xFF in these locations.  The application should
 * report some fixed version of the bootloader that didn't have this information
 * to prevent unexpected behavior.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOTLOADER_INFO_H__
#define BOOTLOADER_INFO_H__

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

/**
 * @brief Flash address of the location where the Bootloader FW version resides.
 * @note 1: This value MUST match the .btldrInfo address in the linker script
 * @note 2: This value is specific to the STM32F2x7 family processors and to the
 * flash size.
 */
#define FLASH_BOOTLDR_FW_VER_ADDRESS      0x0801FC00

/**
 * @brief Number of bytes taken up by the Bootloader FW version.
 */
#define FLASH_BOOTLDR_FW_VER_SIZE         32

/**
 * @brief Flash address of the location where the Bootloader FW build date resides.
 * @note 1: This value MUST match the .btldrInfo address in the linker script
 */
#define FLASH_BOOTLDR_FW_DATE_ADDRESS     ( FLASH_BOOTLDR_FW_VER_ADDRESS + \
                                            FLASH_BOOTLDR_FW_VER_SIZE )

/**
 * @brief Number of bytes taken up by the Bootloader FW build date.
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
/******** Copyright (C) 2013 Harry Rostovtsev. All rights reserved *****END OF FILE****/
