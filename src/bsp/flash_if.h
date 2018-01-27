// $Id$
/**
 * @file 	flash_if.h
 * @brief   Contains declarations of the flash memory manipulation functions
 * specific to how the Redwood Laminator bootloader writes to flash.
 *
 * @date   	1/22/2013
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */
// $Log$
#ifndef FLASH_IF_H_
#define FLASH_IF_H_

#include "stm32f4xx_flash.h"
#include "Shared.h"
#include "project_includes.h"

/* These defines specify where the various flash regions start and end on the
 * STM32F2xx 1MB flash chip that is being used by the Redwood Laminator */
#define FLASH_BTLDR_PAGE_ADDRESS 			0x08000000
#define FLASH_BTLDR_LAST_ADDRESS 			0x0801FFFF
#define FLASH_APPL_PAGE_ADDRESS  			0x08020000
#define FLASH_END_ADDRESS        			0x080FFFFF

#define FLASH_APPL_CRC_ADDRESS				0x080FFFFC
#define FLASH_APPL_SIZE_ADDRESS				0x080FFFF8

/**
 * Unlocks Flash for write access.
 * @param	None
 * @return	None
 */
void FLASH_If_Init(void);

/**
 * This function does an erase of the flash reserved for the bootloader
 * @param	flash_image_type: FWImgType type that specifies which section of
 * flash to erase
 * @param	expected_size: number of bytes that the FW image is expected to be.
 * This will determine how many pages to erase since erases are slow and we want
 * to minimize them.
 * @return	0: user flash area successfully erased
 * 			1: error occurred
 */
uint8_t FLASH_If_Erase(FWImgType flash_image_type, uint32_t expected_size);

/**
 * This function writes a buffer of "size" bytes starting from the "start_address".
 * @param	start_address: Address at which the buffer should start being written
 * @param	buffer: pointer to the buffer containing the bytes to be written
 * @param	size: number of bytes in the buffer that will be written to flash
 * @return	size: number of bytes written if successful
 * 			-size: number of bytes written before error occurred.
 */
int16_t FLASH_Write_FW_Packet(uint32_t start_address, char* buffer, uint8_t size);

/**
 * This function reads the application CRC stored at the very end of the flash.
 * @param	None
 * @return	size: uint32_t CRC stored at that address location
 */
uint32_t FLASH_Read_Appl_CRC(void);

/**
 * This function writes the application CRC to the very end of the flash.
 * @param	crc: uint32_t crc that is to be written
 * @return	flash_status: status of the flash write operation.
 */
FLASH_Status FLASH_Write_Appl_CRC(uint32_t crc);

/**
 * This function reads the application size stored at the very end of the flash.
 * @param	None
 * @return	size: uint32_t application size stored at that address location
 */
uint32_t FLASH_Read_Appl_size(void);

/**
 * This function writes the application size to the very end of the flash.
 * @param	size: uint32_t size that is to be written
 * @return	flash_status: status of the flash write operation.
 */
FLASH_Status FLASH_Write_Appl_size(uint32_t size);

/**
 * This function performs a basic uint32_t read from flash
 * @param	address: Address where to read from
 * @return	data_read: uint32_t value stored at the specified address
 */
uint32_t FLASH_Read_UINT32(uint32_t address);

/**
 * This function performs a basic uint32_t write to the flash
 * @param	address: Address where to write data to
 * @param	data: uint32_t value to write to the specified address
 * @return	flash_status: status of the flash write operation
 */
FLASH_Status FLASH_Write_UINT32(uint32_t address, uint32_t data);

#endif /* FLASH_IF_H_ */
/******** Copyright (C) 2013 Harry Rostovtsev. All rights reserved *****END OF FILE****/
