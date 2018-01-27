// $Id$
/**
 * @file 	flash_if.c
 * @brief   Contains implementation of the flash memory manipulation functions
 * specific to how the Redwood Laminator bootloader writes to flash.
 *
 * @date   	1/22/2013
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */
// $Log$

#include "flash_if.h"
#include "project_includes.h"
#include <stdio.h>
#include "version.h"

/* This stores the fw version and build date to the special section of flash of
 * the bootloader located in the last 1K of flash. */
__attribute__((__section__(".btldrInfo"))) const char flash_btldr_ver[32] = FW_VER;
__attribute__((__section__(".btldrInfo"))) const char flash_btldr_date[32] = BUILD_DATE;

/******************************************************************************/
void FLASH_If_Init(void) {
	FLASH_Unlock();
}

/******************************************************************************/
uint8_t FLASH_If_Erase(FWImgType flash_image_type, uint32_t expected_size) {

	FLASH_Status flash_status;

	/* These flags have to be cleared before any operation can be done on the
	 * flash memory */
//	debug_printf("Clearing flash flags. Status before clear: %x\n",FLASH->SR );
	FLASH_ClearFlag(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
					FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_EOP);
//	debug_printf("Status after clear: %x\n",FLASH->SR );
	debug_printf("!!!Erasing Flash.  This is slow so please be patient!!!\n");

	/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
     be done by word */
	if (_LamBtLdr == flash_image_type) {
		/* Make sure that the expected size of the bootloader is smaller than
		 * the max allowed size (128K) */
		if (expected_size > 131072) {
		    return (1);
		}

		/* THIS IS VERY DANGEROUS.  WE ARE ERASING THE BOOTLOADER HERE */
		flash_status = FLASH_EraseSector(FLASH_Sector_0, VoltageRange_3); /* 16 Kbyte */
		if (FLASH_COMPLETE != flash_status) {
			debug_printf("Error %d for Flash Erase for sector %d\n",
			      flash_status, FLASH_Sector_0);
			debug_printf("FLASH SR:%lx\n", FLASH->SR);
			return(flash_status);
		}
		if (expected_size > 16384) {
			flash_status = FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3); /* 16 Kbyte */
			if (FLASH_COMPLETE != flash_status) {
				debug_printf("Error %d for Flash Erase for sector %d\n",
				      flash_status, FLASH_Sector_1);
				return(flash_status);
			}
			if (expected_size > 32768) {
				flash_status = FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3); /* 16 Kbyte */
				if (FLASH_ERROR_PROGRAM == flash_status) {
					debug_printf("FLASH_ERROR_PROGRAM Error for Flash Erase for "
					      "sector %d\n", FLASH_Sector_2);
					return(flash_status);
				}
				if (expected_size > 49152) {
					flash_status = FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3); /* 16 Kbyte */
					if (FLASH_ERROR_PROGRAM == flash_status) {
						debug_printf("FLASH_ERROR_PROGRAM Error for Flash Erase "
						      "for sector %d\n", FLASH_Sector_3);
						return(flash_status);
					}
					if (expected_size > 65536) {
						flash_status = FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3); /* 64 Kbyte */
						if (FLASH_ERROR_PROGRAM == flash_status) {
							debug_printf("FLASH_ERROR_PROGRAM Error for Flash Erase "
							      "for sector %d\n", FLASH_Sector_4);
							return(flash_status);
						}
					}
				}
			}
		}

	} else if (_LamApp == flash_image_type) {

		/* Make sure that the expected size of the app FW image is smaller than
		 * the max allowed size (917K) */
//		if (expected_size > 917504) { //TODO: This depends on whether we can write some values into the end of the bootloader section.
	    if (expected_size > 786432) {
			debug_printf("Error.  Expected flash image size too big.  Max image size is 786432 bytes\n");
			return(1);
		}

		flash_status = FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3); /* 128 Kbyte */
		if (FLASH_COMPLETE != flash_status) {
			debug_printf("Error %d for Flash Erase for sector %d\n",
			      flash_status, FLASH_Sector_5);
			return(flash_status);
		}

		if (expected_size > 131072) {
			flash_status = FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3); /* 128 Kbyte */
			if (FLASH_COMPLETE != flash_status) {
				debug_printf("Error %d for Flash Erase for sector %d\n",
				      flash_status, FLASH_Sector_6);
				return(flash_status);
			}

			if (expected_size > 262144) {
				flash_status = FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3); /* 128 Kbyte */
				if (FLASH_COMPLETE != flash_status) {
					debug_printf("Error %d for Flash Erase for sector %d\n",
					      flash_status, FLASH_Sector_7);
					return(flash_status);
				}

				if (expected_size > 393216) {
					flash_status = FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3); /* 128 Kbyte */
					if (FLASH_COMPLETE != flash_status) {
						debug_printf("Error %d for Flash Erase for sector %d\n",
						      flash_status, FLASH_Sector_8);
						return(flash_status);
					}

					if (expected_size > 524288) {
						flash_status = FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3); /* 128 Kbyte */
						if (FLASH_COMPLETE != flash_status) {
							debug_printf("Error %d for Flash Erase for sector %d\n",
							      flash_status, FLASH_Sector_9);
							return(flash_status);
						}

						if (expected_size > 655360) {
							flash_status = FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3); /* 128 Kbyte */
							if (FLASH_COMPLETE != flash_status) {
								debug_printf("Error %d for Flash Erase for sector %d\n",
								      flash_status, FLASH_Sector_10);
								return(flash_status);
							}

							/*TODO: maybe store the size and CRC of the application fw image at the end of the 64K block
							 * so that we can actually use this space?  Look into this once space becomes an issue
							 */
//							if (expected_size > 786432) {
//								flash_status = FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); /* 128 Kbyte */
//								if (FLASH_COMPLETE != flash_status) {
//									debug_printf("Error %d for Flash Erase for sector %d\n", flash_status, FLASH_Sector_11);
//									return(flash_status);
//								}
//							}
						}
					}
				}
			}
		}

		/* We always erase the last sector since we store some settings there
		 * and you can't write to flash without first erasing it.*/
		flash_status = FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); /* 128 Kbyte */
		if (FLASH_COMPLETE != flash_status) {
		    debug_printf("Error %d for Flash Erase for sector %d\n",
		          flash_status, FLASH_Sector_11);
		    return(flash_status);
		}

	} else {
		debug_printf("Error.  Invalid FW image type specified.\n");
		return (1);
	}

	debug_printf("!!!Finished erasing flash!!!\n");
	return (0);
}

/******************************************************************************/
int16_t FLASH_Write_FW_Packet(uint32_t start_address, char* buffer, uint8_t size) {

	int16_t bytes_written = 0;
	uint8_t data_read_back = 0;
	FLASH_Status flash_status;

	/* These flags have to be cleared before any operation can be done on the
	 * flash memory */
	FLASH_ClearFlag(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
					FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP);

	for (uint8_t i = 0; i < size; i++, start_address++, bytes_written++) {
		flash_status = FLASH_ProgramByte(start_address, (uint8_t)buffer[i]);
		if (FLASH_COMPLETE != flash_status) {
			debug_printf("Error %d writing %x to address %lx\n",
			      flash_status, buffer[i], start_address);
			return (bytes_written);
		} else {
			data_read_back = *((char *)start_address);
			if (data_read_back != buffer[i]) {
				debug_printf("Failed to verify write at address %lx : Wrote %2x and "
				      "read back %2ux\n", start_address, buffer[i], data_read_back);
				return (bytes_written);
			}
		}
	}

	if (bytes_written != size) {
		debug_printf("Bytes written (%d) != number of bytes requested to be "
		      "written (%d) \n", bytes_written, size);
	}

	return (bytes_written);
}

/******************************************************************************/
uint32_t FLASH_Read_Appl_CRC(void) {
	uint32_t data_read = FLASH_Read_UINT32(FLASH_APPL_CRC_ADDRESS);
	return(data_read);
}

/******************************************************************************/
FLASH_Status FLASH_Write_Appl_CRC(uint32_t crc) {
	FLASH_Status status = FLASH_Write_UINT32(FLASH_APPL_CRC_ADDRESS, crc);
	return(status);
}

/******************************************************************************/
uint32_t FLASH_Read_Appl_size(void) {
	uint32_t data_read = FLASH_Read_UINT32(FLASH_APPL_SIZE_ADDRESS);
	return(data_read);
}

/******************************************************************************/
FLASH_Status FLASH_Write_Appl_size(uint32_t size) {
    FLASH_Status status = FLASH_Write_UINT32(FLASH_APPL_SIZE_ADDRESS, size);
	return(status);
}

/******************************************************************************/
uint32_t FLASH_Read_UINT32(uint32_t address) {
	uint32_t data_read = 0;

	/* These flags have to be cleared before any operation can be done on the
	 * flash memory */
	FLASH_ClearFlag(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
			FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP);

	data_read = *((uint32_t *)address);

	return(data_read);
}

/******************************************************************************/
FLASH_Status FLASH_Write_UINT32(uint32_t address, uint32_t data) {
	uint32_t data_read = 0;
	FLASH_Status flash_status = FLASH_COMPLETE;

	/* These flags have to be cleared before any operation can be done on the
	 * flash memory */
	FLASH_ClearFlag(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
			FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP);

	/* Clear out the sector first */
	flash_status = FLASH_ProgramWord(address, 0xFFFFFFFF);
	if (FLASH_COMPLETE != flash_status) {
	    debug_printf("Error %d writing 0xFFFFFFFF to address %lx\n",
	          flash_status, address);
	    return (flash_status);
	} else {
	    data_read = *((uint32_t *)address);
	    if (data_read != 0xFFFFFFFF) {
	        debug_printf("Failed to verify write at address %lx : Wrote "
	              "0xFFFFFFFF and read back %2lx\n", address, data_read);
	        return (flash_status);
	    }
	}

	flash_status = FLASH_ProgramWord(address, data);
	if (FLASH_COMPLETE != flash_status) {
		debug_printf("Error %d writing %lx to address %lx\n",
		      flash_status, data, address);
		return (flash_status);
	} else {
		data_read = *((uint32_t *)address);
		if (data_read != data) {
			debug_printf("Failed to verify write at address %lx : Wrote %2lx and "
			      "read back %2lx\n", address, data, data_read);
			return (flash_status);
		}
	}

	return(flash_status);
}
