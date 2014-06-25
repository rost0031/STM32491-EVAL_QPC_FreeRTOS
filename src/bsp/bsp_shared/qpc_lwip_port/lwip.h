// $Id$
/**
 * @file 	lwip.c
 * @brief   lwIP consolidated file for QP-lwIP integration.  This file is
 * 			based on a version distributed by Quantum Leaps as part of
 * 			their examples.
 *
 * @date   	09/27/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

#ifndef lwip_h
#define lwip_h

               /* protection against including lwIP in unauthorized modules */
#ifndef LWIP_ALLOWED
#error "The lwIP code is not reentrant and is not allowed in this module."
#endif

#include "lwip/opt.h"                            /* lwIP options come first */
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/mem.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"

#include "netif/etharp.h"
#include "netif/eth_driver.h"
                                                /* utilities added by QL... */
#ifdef __cplusplus
extern "C" {
#endif

/**
* Allocate a transport-layer pbuf and copies the provided data buffer 'data'
* of length 'len' bytes into the payload(s) of the pbuf. The function takes
* care of splitting the data into successive pbuf payloads, if necessary.
*
* The function returns the newly created pbuf or NULL if the pbuf cannot
* be allocated.
*/
struct pbuf *pbuf_new(u8_t *data, u16_t len);

#ifdef __cplusplus
}
#endif

#endif                                                            /* lwip_h */
/*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
