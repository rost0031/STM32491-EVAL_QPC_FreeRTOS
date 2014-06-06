// $Id$
/**
 * @file        MtrTestLWIPMgr.c
 * @brief   This file contains the definition of the MtrTestLWIPMgr Active
 *                      Object and its state machine.
 *
 * @date        09/27/2012
 * @author      Harry Rostovtsev
 * @email       harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$
#define LWIP_ALLOWED

#include "qp_port.h"                                             /* QP-port */
#include "LWIPMgr.h"               /* application events and active objects */
#include "CommStackMgr.h"                                 /* for msg events */
#include "bsp.h"                                   /* Board Support Package */
#include "Shared.h"
#include "lwip.h"                                             /* lwIP stack */

#include <string.h>
#include <stdio.h>

#include "project_includes.h"

Q_DEFINE_THIS_FILE

            /* application signals cannot overlap the device-driver signals */
Q_ASSERT_COMPILE(MAX_SHARED_SIG < DEV_DRIVER_SIG);

#define LWIP_SLOW_TICK_MS       TCP_TMR_INTERVAL

/* Active object class -----------------------------------------------------*/
typedef struct LWIPMgrTag {
    QActive super;                                   /* derive from QActive */

    QTimeEvt te_LWIP_SLOW_TICK;
    struct netif   *netif;
    struct udp_pcb *upcb;
    uint32_t        ip_addr;    /* IP address in the native host byte order */

#if LWIP_TCP
    uint32_t tcp_tmr;
#endif
#if LWIP_ARP
    uint32_t arp_tmr;
#endif
#if LWIP_DHCP
    uint32_t dhcp_fine_tmr;
    uint32_t dhcp_coarse_tmr;
#endif
#if LWIP_AUTOIP
    uint32_t auto_ip_tmr;
#endif
} LWIPMgr;

static QState LWIPMgr_initial(LWIPMgr *me, QEvent const *e);
static QState LWIPMgr_Running(LWIPMgr *me, QEvent const *e);


/* Local objects -----------------------------------------------------------*/
static LWIPMgr l_LWIPMgr;              /* the single instance of LwIPMgr AO */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_LWIPMgr = (QActive *)&l_LWIPMgr;     /* "opaque" pointer */

/**
  * @brief  This function is the UDP handler callback. It is automatically
  *             called by the LWIP stack when a new UDP message arrives.  It
  *             creates a new MsgEvt event (for CommStack) and publishes it to
  *             the shared CommStackMgr AO. IT SHOULD NOT BE CALLED DIRECTLY.
  *
  * @param  arg: a pointer to arg.  (Not currently used but in the original
  *             implementation of the callback.
  * @param  upcb: a pointer to the udb structure containing UDP connect data.
  * @param  p:  a pointer to the pbuf containing the received data.
  * @param  addr: a pointer to struct containing the IP data.
  * @param  part: a u16_t type containing the port number used to connect.
  * @retval None
  */
static void udp_rx_handler(void *arg, struct udp_pcb *upcb,
                           struct pbuf *p, struct ip_addr *addr, u16_t port);
/*..........................................................................*/
void LWIPMgr_ctor(void) {
    LWIPMgr *me = &l_LWIPMgr;

    QActive_ctor(&me->super, (QStateHandler)&LWIPMgr_initial);
    QTimeEvt_ctor(&me->te_LWIP_SLOW_TICK, LWIP_SLOW_TICK_SIG);
}
/*..........................................................................*/
QState LWIPMgr_initial(LWIPMgr *me, QEvent const *e) {
    uint8_t  macaddr[NETIF_MAX_HWADDR_LEN];

    (void)e;        /* suppress the compiler warning about unused parameter */

    QS_OBJ_DICTIONARY(&l_LWIPMgr);
    QS_OBJ_DICTIONARY(&l_LWIPMgr.te_LWIP_SLOW_TICK);

    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&LWIPMgr_initial);
    QS_FUN_DICTIONARY(&LWIPMgr_Running);
    QS_FUN_DICTIONARY(&udp_rx_handler);
    QS_FUN_DICTIONARY(&ETH_SendMsg_Handler);

    QS_FILTER_SM_OBJ(0);                  /* Turn off all tracing for this SM */
    QS_FILTER_AO_OBJ(0);                  /* Turn off all tracing for this AO */
    QS_FILTER_TE_OBJ(0);           /* Turn off time event tracing for this AO */

    /* Configure the hardware MAC address for the Ethernet Controller */

    /*
     * Set up the MAC address and make sure it's not all FF values
     * TODO: This will evetually be read somewhere
     */

    /* the MAC address must have been programmed! */
    Q_ASSERT((STATIC_IPADDR0 != 0xFF) &&
            (STATIC_IPADDR1 != 0xFF) &&
            (STATIC_IPADDR2 != 0xFF) &&
            (STATIC_IPADDR3 != 0xFF));

    macaddr[0] = DEF_MAC_ADDR0;
    macaddr[1] = DEF_MAC_ADDR1;
    macaddr[2] = DEF_MAC_ADDR2;
    macaddr[3] = DEF_MAC_ADDR3;
    macaddr[4] = DEF_MAC_ADDR4;
    macaddr[5] = DEF_MAC_ADDR5;

    debug_printf("Trying static address:\n");

    debug_printf("IP address:\n");
    debug_printf("%d.%d.%d.%d\n",
          STATIC_IPADDR0, STATIC_IPADDR1, STATIC_IPADDR2, STATIC_IPADDR3);
    debug_printf("MAC address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\n",
          macaddr[0], macaddr[1],macaddr[2],macaddr[3],macaddr[4], macaddr[5]);
                                          /* initialize the Ethernet Driver */
    me->netif = eth_driver_init((QActive *)me, macaddr);

    me->ip_addr = 0xFFFFFFFF;             /* initialize to impossible value */

    me->upcb = udp_new();
    udp_bind(me->upcb, IP_ADDR_ANY, 777);           /* use port 777 for UDP */
    udp_recv(me->upcb, &udp_rx_handler, me);

    QActive_subscribe((QActive *)me, ETH_SEND_SIG);

    return Q_TRAN(&LWIPMgr_Running);
}

/*..........................................................................*/
QState LWIPMgr_Running(LWIPMgr *me, QEvent const *e) {

    switch (e->sig) {
        case Q_ENTRY_SIG: {
            QTimeEvt_postEvery(&me->te_LWIP_SLOW_TICK, (QActive *)me,
                (LWIP_SLOW_TICK_MS * BSP_TICKS_PER_SEC) / 1000);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->te_LWIP_SLOW_TICK);
            return Q_HANDLED();
        }

        case ETH_SEND_SIG: {
                /* Event posted that will include (inside it) a msg to send */
                if (me->upcb->remote_port != (uint16_t)0) {
                        struct pbuf *p = pbuf_new((u8_t *)((EthEvt const *)e)->msg, 
                                                  ((EthEvt const *)e)->msg_len);
                        if (p != (struct pbuf *)0) {
                                udp_send(me->upcb, p);
                                pbuf_free(p);                   /* don't leak the pbuf! */
                        }
                }
                return Q_HANDLED();
        }
        case LWIP_RX_READY_SIG: {
            eth_driver_read();
            return Q_HANDLED();
        }
        case LWIP_TX_READY_SIG: {
            eth_driver_write();
            return Q_HANDLED();
        }
        case LWIP_SLOW_TICK_SIG: {
                                                 /* has IP address changed? */
            if (me->ip_addr != me->netif->ip_addr.addr) {
                uint32_t ip_net;    /* IP address in the network byte order */

                me->ip_addr = me->netif->ip_addr.addr; /* save the IP addr. */
                ip_net  = ntohl(me->ip_addr);
            }

#if LWIP_TCP
            me->tcp_tmr += LWIP_SLOW_TICK_MS;
            if (me->tcp_tmr >= TCP_TMR_INTERVAL) {
                me->tcp_tmr = 0;
                tcp_tmr();
            }
#endif
#if LWIP_ARP
            me->arp_tmr += LWIP_SLOW_TICK_MS;
            if (me->arp_tmr >= ARP_TMR_INTERVAL) {
                me->arp_tmr = 0;
                etharp_tmr();
            }
#endif
#if LWIP_DHCP
            me->dhcp_fine_tmr += LWIP_SLOW_TICK_MS;
            if (me->dhcp_fine_tmr >= DHCP_FINE_TIMER_MSECS) {
                me->dhcp_fine_tmr = 0;
                dhcp_fine_tmr();
            }
            me->dhcp_coarse_tmr += LWIP_SLOW_TICK_MS;
            if (me->dhcp_coarse_tmr >= DHCP_COARSE_TIMER_MSECS) {
                me->dhcp_coarse_tmr = 0;
                dhcp_coarse_tmr();
            }
#endif
#if LWIP_AUTOIP
            me->auto_ip_tmr += LWIP_SLOW_TICK_MS;
            if (me->auto_ip_tmr >= AUTOIP_TMR_INTERVAL) {
                me->auto_ip_tmr = 0;
                autoip_tmr();
            }
#endif
            return Q_HANDLED();
        }
        case LWIP_RX_OVERRUN_SIG: {
            LINK_STATS_INC(link.err);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

/* Ethernet message sender ...................................................*/
void ETH_SendMsg_Handler(MsgEvt const *e) {

        /* 1. Construct a new msg event indicating that a msg has been received */
        EthEvt *ethEvt = Q_NEW(EthEvt, ETH_SEND_SIG);

        /* 2. Fill the msg payload with the message */
        MEMCPY(ethEvt->msg, e->msg, e->msg_len);
        ethEvt->msg_len = e->msg_len;

        /* 3. Publish the newly created EthEvt event to LWIPMgr AO */
        QF_PUBLISH((QEvent *)ethEvt, AO_LWIPMgr);
}

/* UDP handler ...............................................................*/
static void udp_rx_handler(void *arg, struct udp_pcb *upcb,
                           struct pbuf *p, struct ip_addr *addr, u16_t port) {

    /* 1. Construct a new msg event indicating that a msg has been received */
    MsgEvt *msgEvt = Q_NEW(MsgEvt, MSG_RECEIVED_SIG);

    /* 2. Fill the msg payload and get the msg source and length */
    MEMCPY(msgEvt->msg, p->payload, p->len);
    msgEvt->msg_len = p->len;

    /* 3. Don't bother publishing locally.  Instead, publish the newly created
     * MsgEvt event to CommStackMgr AO */
    QF_PUBLISH((QEvent *)msgEvt, AO_LWIPMgr);

    /* 4. connect to the remote host */
    udp_connect(upcb, addr, port);

    /* 5. Free up the pbuf */
    pbuf_free(p);
}

/*********** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
