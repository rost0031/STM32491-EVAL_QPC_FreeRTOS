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


#include "stm32f2xx_it.h"
#include "stm32f2xx.h"
#include "bsp.h"                                     /* Board Support Package */
#include "serial.h"                                   /* Serial Debug Support */

#include "mem_datacopy.h"
#include "Shared.h"
#include "CommStackMgr.h"

/* These are TCP defines to allow us to turn off TCP/IP but still allow compilation */
#define LWIP_TCP           1
#define TCP_TMR_INTERVAL   250
#define LWIP_DHCP          1
#define LWIP_AUTOIP        0

typedef enum {
    FALSE = 0,
    TRUE = !FALSE
} bool;

/* This define allows functions to be placed into RAM by the linker and startup
 * code.  */
#define RAMFUNC __attribute__ ((long_call, section (".ramfunctions")))

#endif /* PROJECT_INCLUDES_H_ */
/*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
