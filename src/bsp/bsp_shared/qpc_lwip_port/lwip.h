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
 *
 * @addtogroup groupLWIP_QPC_Eth
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LWIP_H_
#define LWIP_H_

/* protection against including lwIP in unauthorized modules */
#ifndef LWIP_ALLOWED
#error "The lwIP code is not reentrant and is not allowed in this module."
#endif

/* Includes ------------------------------------------------------------------*/
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


/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
                                                  /* utilities added by QL... */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate a new pbuf from the LWIP mem pool.
 *
 * This function allocates a transport-layer pbuf and copies the provided data
 * buffer 'data' of length 'len' bytes into the payload(s) of the pbuf. The
 * function takes care of splitting the data into successive pbuf payloads, if
 * necessary.
 *
 * @param [in]  *data: uint8_t pointer to the data buffer containing data.
 * @param [in]  len: uint16_t length of the data in the buffer *data.
 * @retval   *pbuf: a pbuf pointer to the newly created pbuf
 *           NULL: if the pbuf cannot be allocated.
 */
struct pbuf *pbuf_new(u8_t *data, u16_t len);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupLWIP_QPC_Eth
 */

#endif                                                             /* LWIP_H_ */
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
