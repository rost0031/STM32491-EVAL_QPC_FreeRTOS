// $Id$
/**
 * @file   project_includes.h
 * @brief  This file contains some common includes needed by multiple files as well
 * 		   as acting like a wrapper for any auto-generated CommStack include files
 * 		   since they do not prevent multiple inclusion.  This file is project
 * 		   specific and should not be shared by multiple modules.  Each project,
 * 		   whether embedded or simulator, should have its own implementation of this
 * 		   file.
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$
#ifndef PROJECT_INCLUDES_H_
#define PROJECT_INCLUDES_H_

#include "mem_datacopy.h"      /* Very fast STM32 specific MEMCPY declaration */
#include "CBSignals.h"                                /* Signal declarations. */
#include "CBTimeouts.h"                             /* Timeouts declarations. */
#include "console_output.h"         /* For debug/console output declarations. */
#include "time.h"                                   /* For time functionality */

typedef enum {
    FALSE = 0,
    TRUE = !FALSE
} bool;

/* This define allows functions to be placed into RAM by the linker and startup
 * code.  */
#define RAMFUNC __attribute__ ((long_call, section (".ramfunctions")))

#endif /* PROJECT_INCLUDES_H_ */
/*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
