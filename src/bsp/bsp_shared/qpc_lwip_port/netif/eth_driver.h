/**
 * @file   eth_driver.h
 * @brief  This file contains QPC LWIP Ethernet layer headers for STM32F4xx
 * 		  Board. This file is derived from the original version distributed by
 *         Quantum Leaps as part of the demo on the Luminary devel kit.
 *
 * @date   08/25/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupLWIP_QPC_Eth
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ETH_DRIVER_H_
#define ETH_DRIVER_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */

/* for LwIp support */
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "netif/etharp.h"

/* for STM32 support */
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"

/* Exported defines ----------------------------------------------------------*/
#if LWIP_PTPD
/**
 * @brief Setup processing for PTP (IEEE-1588)
 * @param [out]   *time_s: returned seconds
 * @param [out]   *time_ns: returned nanoseconds
 * @return  None
 */
void lwIPHostGetTime(u32_t *time_s, u32_t *time_ns);
#endif

#ifndef TX_PBUF_QUEUE_LEN
/**
 * @brief   TX Pbuffer queue lenth.
 */
#define TX_PBUF_QUEUE_LEN 8
#endif


/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Function to check if PBuf queue is empty
 * @param   [in]  me: pointer to queue structure
 * @retval  1: if empty
 *          0: if not empty
 */
#define PbufQueue_isEmpty(me_) ((me_)->qwrite == (me_)->qread)

/* Exported types ------------------------------------------------------------*/
/**
 * @struct Helper queue of pbufs
 */
typedef struct PbufQueueTag {
    struct pbuf *ring[TX_PBUF_QUEUE_LEN];    /**< pbuf pointer to ring buffer */
    uint8_t qwrite;                     /**< index of last write in the queue */
    uint8_t qread;                       /**< index of last read in the queue */
    uint8_t overflow;                           /**< flag for queue overflow  */
} PbufQueue;

/**
 * @enum Ethernet signals used by QPC
 */
enum EthDriverSignals {
    LWIP_SLOW_TICK_SIG = DEV_DRIVER_SIG,
    LWIP_RX_READY_SIG,
    LWIP_TX_READY_SIG,
    LWIP_RX_OVERRUN_SIG
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize ethernet LWIP QPC bridge.
 *
 * @note 1: Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * @note 2: This function should be passed as a parameter to netif_add().
 *
 * @param [in] *active: a QActive pointer to the AO that is handling LWIP
 * @param [in] macaddr: buffer containing the MAC address to use.
 *
 * @return *netif: pointer to a netif struct that has been initialized
 */
struct netif *eth_driver_init(
      QActive *active,
      u8_t macaddr[NETIF_MAX_HWADDR_LEN]
);

/**
 * @brief Read data from the ethernet buffer
 *
 * Initialize the DMA connnected to the ethernet MAC to read data from the MAC
 * into its private buffer.
 *
 * @param   None
 * @return  None
 */
void eth_driver_read( void );

/**
 * @brief Write data to the ethernet buffer
 *
 * Initialize the DMA connnected to the ethernet MAC to write data to the MAC
 * from its private buffer.
 *
 * @param   None
 * @return  None
 */
void eth_driver_write( void );

/**
 * @brief   Initialize the ethernet hardware.
 *
 * @note 1: Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * @note 2: This function should be passed as a parameter to netif_add().
 *
 * @param [in, out]  *netif: netif pointer the lwip network interface structure
 * for this ethernetif
 * @retval ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init( struct netif *netif );

/**
 * @brief   Initialize the ethernet low level hardware.
 *
 * In this function, the hardware should be initialized. Called from
 * ethernetif_init().
 *
 * @param [in,out]   *netif: netif pointer to the already initialized lwip
 * network interface structure for this ethernetif
 */
void low_level_init( struct netif *netif );

/*
 * @brief Output data over the ethernet interface.
 *
 * This function will either write the pbuf into the TX FIFO, or will put the
 * packet in the TX queue of pbufs for subsequent transmission when the
 * transmitter becomes idle.
 *
 * @param [in] *netif: netif pointer to the lwip network interface structure for
 * this ethernetif
 * @param [in] *p: pbuf pointer to the pbuf to send
 * @retval ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 */
err_t ethernetif_output( struct netif *netif, struct pbuf *p );

/**
 * @brief   Low level receive data callback function.
 *
 * This function allocates a pbuf and transfers the bytes of the incoming
 * packet from the interface into the pbuf.
 * This function will read a single packet from the ethernet interface,
 * if available, and return a pointer to a pbuf.  The timestamp of the packet
 * will be placed into the pbuf structure.
 *
 * @param   Nonte
 * @retval *pbuf: a pointer to pbuf filled with the received packet (including
 * MAC header)
 *       NULL on memory error
 */
struct pbuf *low_level_receive( void );

/**
 * @brief   Low level transmit callback function.
 *
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be
 * chained.
 *
 * @param [in] *netif: netif pointer to the the lwip network interface structure
 * for this ethernetif
 * @param [in] *p: pbuf pointer to the MAC packet to send (e.g. IP packet
 * including MAC addresses and type)
 * @retval ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note 1: Returning ERR_MEM here if a DMA queue of your MAC is full can lead
 * to strange results. You might consider waiting for space in the DMA queue to
 * become available since the stack doesn't retry to send a packet dropped
 * because of memory failure (except for the TCP timers).
 */
void low_level_transmit(struct netif *netif, struct pbuf *p);

#ifdef Q_SPY
    static uint8_t l_Ethernet_IRQHandler;
#endif

/**
 * @}
 * end addtogroup groupLWIP_QPC_Eth
 */

#endif                                                       /* ETH_DRIVER_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
