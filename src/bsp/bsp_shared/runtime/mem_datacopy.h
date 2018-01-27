/**
 * @file   	mem_datacopy.h
 * @brief  	This file contains the declaration of the mem_datacopy function that
 * 			is optimized for Cortex-M3 processors.
 *
 * @date   	1/07/2013
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2012 Harry Rostovtsev. All rights reserved.
 */

#ifndef MEM_DATACOPY_H_
#define MEM_DATACOPY_H_

#include <stdint.h>

/* This is a MEMCPY that is way faster than the regular one provided by standard
 * libs.  It's specifically tuned for arm cortex M3 processors and written in
 * very fast assembly.  Use it instead of regular memcpy */
#define MEMCPY(dst,src,len)             MEM_DataCopy(dst,src,len)
#define SMEMCPY(dst,src,len)            MEM_DataCopy(dst,src,len)
void * MEM_DataCopy( void * destination, const void * source, uint16_t num );

#endif /* MEM_DATACOPY_H_ */
