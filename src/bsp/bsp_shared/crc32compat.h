// $Id$
/**
 * @file   crc32compat.h
 * @brief  This file contains declarations of CRC32 algorithm that leverages
 * STM32 hardware for CRC calculations but also produces an output compatible
 * with Boost C++ CRC implementation.  These functions were shamelessly ripped
 * off from the STMicro forums and were under no license that I could see.
 *
 * @date   01/22/2013
 * @author Harry Rostovtsev
 * @email  harry_rostovhtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$

#ifndef CRC32COMPAT_H_
#define CRC32COMPAT_H_

#include "stm32f2xx_crc.h"

/**
  * This function leverages the STM32F2xx CRC hardware to calculate the CRC of
  * a databuffer that produces output compatible with the rest of CRC calculators
  * out there such as winzip and Boost C++ CRC library.
  *
  * Rationale: Unfortunately, STM made some bone-headed moves with their
  * hardware choices and decided to use a right shift instead of left shift.
  * This requires that you first bit reverse the data, calculate the CRC, and
  * then reverse the data back.
  *
  * @param  buffer:	Pointer to the buffer containing data.
  * @param  size:	Size of the buffer.
  * @return uint32_t: the CRC value calculated for the given buffer and size.
  */
uint32_t CalcCRC32(char *buffer, int size);

/**
  * This function bit reverses and is written in ARM assembly.
  *
  * @param  data:	a 32 bit word of data to be reversed
  * @return uint32_t: reversed 32 bit word of data.
  */
uint32_t revbit(uint32_t data);

#endif /* CRC32COMPAT_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
