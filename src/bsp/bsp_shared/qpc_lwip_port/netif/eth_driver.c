// $Id$
/**
 * @file eth_driver.c
 * @brief  his file contains QPC LWIP Ethernet layer for Redwood_H1 Board
 * This file is derived from the ``ethernetif.c'' skeleton Ethernet network
 * interface driver for lwIP.
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 */

#include "netif/eth_driver.h"
#include "stm32f2x7_eth.h"
#include "stm32f2x7_eth_bsp.h"
#include "project_includes.h"

/* Ethernet Rx & Tx DMA Descriptors */
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];

/* Ethernet Receive buffers  */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];

/* Ethernet Transmit buffers */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

/* Global pointers to track current transmit and receive descriptors */
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

/* Global pointer for last received frame infos */
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

/**
 * Sanity Check:  This interface driver will NOT work if the following defines
 * are incorrect.
 */
#if (PBUF_LINK_HLEN != 16)
	#error "PBUF_LINK_HLEN must be 16 for this interface driver!"
#endif
#if (ETH_PAD_SIZE != 2)
	#error "ETH_PAD_SIZE must be 2 for this interface driver!"
#endif

/****************************************************************************/
/*                            Static variables                              */
/****************************************************************************/

static void 			PbufQueue_ctor(PbufQueue *me);
static uint8_t 			PbufQueue_put(PbufQueue *me, struct pbuf *p);
static struct pbuf*		PbufQueue_get(PbufQueue *me);

static struct netif 	l_netif;       /* the single network interface */
static QActive* 		l_active;      /* active object associated with this driver */
static PbufQueue 		l_txq;         /* queue of pbufs for transmission */

/*..........................................................................*/
/* Ethernet Interrupt handler.  It is located here because it uses a lot of
 * the shared objects from QP */
void ETH_IRQHandler(void) __attribute__((__interrupt__));
void ETH_IRQHandler(void) {
    QK_ISR_ENTRY();                      /* inform QK about entering an ISR */

    if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) {
    	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_R);/* clear the interrupt sources */
    	static QEvent const evt_eth_rx = { LWIP_RX_READY_SIG, 0 };
        QACTIVE_POST(l_active, &evt_eth_rx, &l_Ethernet_IRQHandler);

        ETH_DMAITConfig(ETH_DMA_IT_R, DISABLE);       /* disable further RX */
    }

    if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_T) == SET) {
    	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_T);/* clear the interrupt sources */
        static QEvent const evt_eth_tx = { LWIP_TX_READY_SIG, 0 };
        QACTIVE_POST(l_active, &evt_eth_tx, &l_Ethernet_IRQHandler);
    }

    /* When Rx Buffer unavailable flag is set: clear it and resume reception. Taken from:
     * http://lists.gnu.org/archive/html/lwip-users/2012-09/msg00053.html */
    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET) {
    	/* Clear RBUS ETHERNET DMA flag */
    	ETH->DMASR = ETH_DMASR_RBUS;

    	/* Resume DMA reception. The register doesn't care what you write to it. */
    	ETH->DMARPDR = 0;
    }


#if LINK_STATS
    if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_RO) == SET) {
        static QEvent const evt_eth_er = { LWIP_RX_OVERRUN_SIG, 0 };
        QACTIVE_POST(l_active, &evt_eth_er, &l_Ethernet_IRQHandler);
    }
#endif

    QK_ISR_EXIT();                        /* inform QK about exiting an ISR */
}

/*..........................................................................*/
// Called from lwipmgr.c
struct netif *eth_driver_init(QActive *active,
                              u8_t macaddr[NETIF_MAX_HWADDR_LEN])
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;


    lwip_init();	/* initialize the lwIP stack */

                             /* set MAC address in the network interface... */
    l_netif.hwaddr_len = NETIF_MAX_HWADDR_LEN;
    MEMCPY(&l_netif.hwaddr[0], macaddr, NETIF_MAX_HWADDR_LEN);

    l_active = active; /*save the active object associated with this driver */

#if LWIP_NETIF_HOSTNAME
    l_netif.hostname = "Laminator";        /* initialize interface hostname */
#endif

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    NETIF_INIT_SNMP(&l_netif, snmp_ifType_ethernet_csmacd, 100000000);

    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    l_netif.output     = &etharp_output;

    l_netif.linkoutput = &ethernetif_output;

    PbufQueue_ctor(&l_txq);                 /* initialize the TX pbuf queue */

#if (LWIP_DHCP == 0) && (LWIP_AUTOIP == 0)
          /* No mechanism of obtaining IP address specified, use static IP: */
    IP4_ADDR(&ipaddr,  STATIC_IPADDR0,    STATIC_IPADDR1,
                       STATIC_IPADDR2,    STATIC_IPADDR3);
    IP4_ADDR(&netmask, STATIC_NET_MASK0,  STATIC_NET_MASK1,
                       STATIC_NET_MASK2,  STATIC_NET_MASK3);
    IP4_ADDR(&gw,      STATIC_GW_IPADDR0, STATIC_GW_IPADDR1,
                       STATIC_GW_IPADDR2, STATIC_GW_IPADDR3);
#else
     /* either DHCP or AUTOIP are configured, start with zero IP addresses: */
    IP4_ADDR(&ipaddr,  0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw,      0, 0, 0, 0);
#endif
          /* add and configure the Ethernet interface with default settings */
    netif_add(&l_netif,
              &ipaddr, &netmask, &gw,            /* configured IP addresses */
              active,                /* use this active object as the state */
              &ethernetif_init,        /* Ethernet interface initialization */
              &ip_input);                   /* standard IP input processing */

    netif_set_default(&l_netif);

    netif_set_up(&l_netif);                       /* bring the interface up */

#if (LWIP_DHCP != 0)
    dhcp_start(&l_netif);         /* start DHCP if configured in lwipopts.h */
    /* NOTE: If LWIP_AUTOIP is configured in lwipopts.h and
    * LWIP_DHCP_AUTOIP_COOP is set as well, the DHCP process will start
    * AutoIP after DHCP fails for 59 seconds.
    */
#elif (LWIP_AUTOIP != 0)
    autoip_start(&l_netif);     /* start AutoIP if configured in lwipopts.h */
#endif

#if LINK_STATS
    ETH_DMAITConfig(ETH_DMA_IT_TU | ETH_DMA_IT_TU, ENABLE);
#endif

    QS_OBJ_DICTIONARY(&l_Ethernet_IRQHandler);

    return(&l_netif);
}
/*..........................................................................*/
void eth_driver_read(void) {

/* This is a fix for the slowdown issue due to a buildup of packets in the
 * DMA buffer.  Normally, only one packet is read at a time and the rest sit
 * there eliminating useful buffers, eventually leading to a slowdown. Yeah,
 * GOTOs are bad.  Get over it. */
GET_NEXT_FRAGMENT:;
    struct pbuf *p = low_level_receive();
    if (p != NULL) {                  /* new packet received into the pbuf? */
        if (ethernet_input(p, &l_netif) != ERR_OK) {   /* pbuf not handled? */
            LWIP_DEBUGF(NETIF_DEBUG, ("eth_driver_input: input error\n"));
            pbuf_free(p);                                  /* free the pbuf */
            p = NULL;
        } else {
        	goto GET_NEXT_FRAGMENT;  // See note at the beginning of function.
        }

        /* try to output a packet if TX fifo is empty and pbuf is available */
        if( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_TBU) == RESET) {
            p = PbufQueue_get(&l_txq);
            if (p != NULL) {
                low_level_transmit(&l_netif, p);
                pbuf_free(p);    /* free the pbuf, lwIP knows nothing of it */
                p = NULL;
            }
        }
    }

    /* re-enable the RX interrupt */
    ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}
/*..........................................................................*/
void eth_driver_write(void) {
	if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_TBU) == RESET) { /* TX fifo available*/
        struct pbuf *p = PbufQueue_get(&l_txq);
        if (p != NULL) {                        /* pbuf found in the queue? */
            low_level_transmit(&l_netif, p);
            pbuf_free(p);        /* free the pbuf, lwIP knows nothing of it */
        }
    }
}

/*..........................................................................*/
/*
 * This function will either write the pbuf into the TX FIFO,
 * or will put the packet in the TX queue of pbufs for subsequent
 * transmission when the transmitter becomes idle.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the pbuf to send
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 */
err_t ethernetif_output(struct netif *netif, struct pbuf *p) {
    if (PbufQueue_isEmpty(&l_txq) &&            /* nothing in the TX queue? */
        ETH_GetDMAFlagStatus(ETH_DMA_FLAG_TBU) == RESET) {     /* TX empty? */
    	low_level_transmit(netif, p);                  /* send the pbuf right away */
        /* the pbuf will be freed by the lwIP code */
    }
    else {                 /* otherwise post the pbuf to the transmit queue */
        if (PbufQueue_put(&l_txq, p)) { /*could the TX queue take the pbuf? */
            pbuf_ref(p);     /* reference the pbuf to spare it from freeing */
        } else {                                    /* no room in the queue */
        	/* the pbuf will be freed by the lwIP code */
            return(ERR_MEM);
        }
    }
    return(ERR_OK);
}

/*==========================================================================*/

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif) {
	LWIP_ASSERT("netif != NULL", (netif != NULL));


    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...)
     */
    netif->output = etharp_output;
    netif->linkoutput = (void *)low_level_transmit;

    /* Initialize the Ethernet PHY, MAC, and DMA hardware as well as any
     * necessary buffers */
    low_level_init(netif);

    /* Set link status */
    netif->flags |= NETIF_FLAG_LINK_UP;

    return(ERR_OK);
}

/*..........................................................................*/
/*
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf might be
* chained.
*
* @param p the MAC packet to send (e.g. IP packet including MAC addr and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
* @note This function MUST be called with interrupts disabled or with the
*       Stellaris Ethernet transmit fifo protected.
*/
void low_level_transmit(struct netif *netif, struct pbuf *p) {
	struct pbuf *q;
	u32_t l = 0;
	err_t res = ERR_OK;
	u8 *buffer;

	/**
	    * Fill in the first two bytes of the payload data (configured as padding
	    * with ETH_PAD_SIZE = 2) with the total length of the payload data
	    * (minus the Ethernet MAC layer header).
	    */
	//*((unsigned short *)(p->payload)) = p->tot_len - 16;

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	buffer =  (u8 *)(DMATxDescToSet->Buffer1Addr);

    /* Copy data from the pbuf(s) into the TX Fifo. */
    for (q = p; q != NULL; q = q->next) {
        /* 	Send the data from the pbuf to the interface, one pbuf at a
        	time. The size of the data in each pbuf is kept in the ->len
        	variable. */
    	//printf("q->payload=%p\n",(void *)(q->payload));
    	MEMCPY((u8_t*)&buffer[l], q->payload, q->len);
        l += q->len;
    }

    res = ETH_Prepare_Transmit_Descriptors(l);

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

	LINK_STATS_INC(link.xmit);
}

/*..........................................................................*/
/*
 * This function will read a single packet from the Stellaris ethernet
 * interface, if available, and return a pointer to a pbuf.  The timestamp
 * of the packet will be placed into the pbuf structure.
 *
 * @return pointer to pbuf packet if available, NULL otherswise.
 */
struct pbuf *low_level_receive(void) {

#if LWIP_PTPD
    u32_t time_s, time_ns;

    /* Get the current timestamp if PTPD is enabled */
    lwIPHostGetTime(&time_s, &time_ns);
#endif

    __IO ETH_DMADESCTypeDef *DMARxNextDesc;
    struct pbuf *p = NULL, *q = NULL;
	u8 *buffer;
	u16_t len, l = 0, i =0;
	FrameTypeDef frame;

	/* Get received frame */
	frame = ETH_Get_Received_Frame_interrupt();

	/* TODO: investigate this. This is an attempt to fix the slowdown in network
	 * speed after some time passes.  This was found on an ST forum here:
	 * https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy.st.com%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fSTM32F2x7%20lwip%20ping%20-l%201472%20causes%20library%20mailfunction&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=344 */
	/* Check if we really have something or not */
	if (NULL == frame.descriptor) {
		return(p);
	}

	/* Obtain the size of the packet and put it into the "len"
	 variable. */
	len = frame.length;
	buffer = (u8 *)frame.buffer;

	/* check that frame has no error */
	if ((frame.descriptor->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) {
		if (len) {

#if ETH_PAD_SIZE
			len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

			/* We allocate a pbuf chain of pbufs from the pool. */
			p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

			if (p != NULL) {
#if ETH_PAD_SIZE
				pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

				/* We iterate over the pbuf chain until we have read the entire
				 * packet into the pbuf. */
				for (q = p; q != NULL; q = q->next) {
					/* Read enough bytes to fill this pbuf in the chain. The
					 * available data in the pbuf is given by the q->len
					 * variable. */
					MEMCPY((u8_t*) q->payload, (u8_t*)&buffer[l], q->len);
					l += q->len;
				}

#if ETH_PAD_SIZE
				pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

				LINK_STATS_INC(link.recv);

			} else {

				LINK_STATS_INC(link.memerr); LINK_STATS_INC(link.drop);

			} /* End else */
		} /* End if */



	} /* End - check that frame has no error */

	#if LWIP_PTPD
	/* Place the timestamp in the PBUF */
	p->time_s = time_s;
	p->time_ns = time_ns;
#endif


	/* Release descriptors to DMA */
	/* Check if received frame with multiple DMA buffer segments */
	if (DMA_RX_FRAME_infos->Seg_Count > 1)  {
		DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
	} else {
		DMARxNextDesc = frame.descriptor;
	}

	/* Set Own bit in Rx descriptors: gives the buffers back to DMA */
	for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++) {
		DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
		DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
	}

	/* Clear Segment_Count */
	DMA_RX_FRAME_infos->Seg_Count =0;


	/* When Rx Buffer unavailable flag is set: clear it and resume reception */
	if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET) {
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_RBUS;

		/* Resume DMA reception */
		ETH->DMARPDR = 0;
	}

	return(p);
}

/*..........................................................................*/
static void PbufQueue_ctor(PbufQueue *me) {
    me->qread    = 0;
    me->qwrite   = 0;
    me->overflow = 0;
}
/*..........................................................................*/
static struct pbuf *PbufQueue_get(PbufQueue *me) {
    struct pbuf *pBuf;

    if (PbufQueue_isEmpty(me)) {
        /* Return a NULL pointer if the queue is empty. */
        pBuf = (struct pbuf *)0;
    }
    else {
        /*
        * The queue is not empty so return the next frame from it
        * and adjust the read pointer accordingly.
        */
        pBuf = me->ring[me->qread];
        if ((++me->qread) == Q_DIM(me->ring)) {
            me->qread = 0;
        }
    }
    return(pBuf);
}
/*..........................................................................*/
static uint8_t PbufQueue_put(PbufQueue *me, struct pbuf *p) {
    uint8_t next_qwrite = me->qwrite + 1;

    if (next_qwrite == Q_DIM(me->ring)) {
        next_qwrite = 0;
    }
    if (next_qwrite != me->qread) {
        /*
        * The queue isn't full so we add the new frame at the current
        * write position and move the write pointer.
        */
        me->ring[me->qwrite] = p;
        if ((++me->qwrite) == Q_DIM(me->ring)) {
            me->qwrite = 0;
        }
        return(1);                          /* successfully posted the pbuf */
    }
    else {
        /*
        * The stack is full so we are throwing away this value.
        * Keep track of the number of times this happens.
        */
        ++me->overflow;
        return(0);                               /* could not post the pbuf */
    }
}


/*..........................................................................*/
#if NETIF_DEBUG
/* Print an IP header by using LWIP_DEBUGF
 * @param p an IP packet, p->payload pointing to the IP header
 */
void eth_driver_debug_print(struct pbuf *p) {
    struct eth_hdr *ethhdr = (struct eth_hdr *)p->payload;
    u16_t *plen = (u16_t *)p->payload;

    LWIP_DEBUGF(NETIF_DEBUG, ("ETH header:\n"));
    LWIP_DEBUGF(NETIF_DEBUG, ("Packet Length:%5"U16_F" \n",*plen));
    LWIP_DEBUGF(NETIF_DEBUG, ("Destination: %02"X8_F"-%02"X8_F"-%02"X8_F
        "-%02"X8_F"-%02"X8_F"-%02"X8_F"\n",
        ethhdr->dest.addr[0],
        ethhdr->dest.addr[1],
        ethhdr->dest.addr[2],
        ethhdr->dest.addr[3],
        ethhdr->dest.addr[4],
        ethhdr->dest.addr[5]));
    LWIP_DEBUGF(NETIF_DEBUG, ("Source: %02"X8_F"-%02"X8_F"-%02"X8_F
        "-%02"X8_F"-%02"X8_F"-%02"X8_F"\n",
        ethhdr->src.addr[0],
        ethhdr->src.addr[1],
        ethhdr->src.addr[2],
        ethhdr->src.addr[3],
        ethhdr->src.addr[4],
        ethhdr->src.addr[5]));
    LWIP_DEBUGF(NETIF_DEBUG, ("Packet Type:0x%04"U16_F" \n", ethhdr->type));
}
#endif /* NETIF_DEBUG */

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void low_level_init(struct netif *netif) {
	uint32_t i;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = DEF_MAC_ADDR0;
    netif->hwaddr[1] = DEF_MAC_ADDR1;
    netif->hwaddr[2] = DEF_MAC_ADDR2;
    netif->hwaddr[3] = DEF_MAC_ADDR3;
    netif->hwaddr[4] = DEF_MAC_ADDR4;
    netif->hwaddr[5] = DEF_MAC_ADDR5;

	/* set netif maximum transfer unit */
	netif->mtu = 1500;

	/* Accept broadcast address and ARP traffic */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	/* initialize MAC address in ethernet MAC */
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr);

	/* Initialize Tx Descriptors list: Chain Mode */
	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
	/* Initialize Rx Descriptors list: Chain Mode  */
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

	/* Enable Ethernet Rx interrrupt */
	for(i=0; i<ETH_RXBUFNB; i++) {
		ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
	}

#ifdef CHECKSUM_BY_HARDWARE
	/* Enable the checksum insertion for the Tx frames */
	for(i=0; i<ETH_TXBUFNB; i++) {
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}

#endif

	/* Enable MAC and DMA transmission and reception */
	ETH_Start();
}

/*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
