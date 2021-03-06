// $Id$
/**
 * @file   lwipopts.c
 * @brief  lwIP options for the Redwood_H1 Board
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2012 Harry Rostovtsev. All rights reserved.
 */
// $Log$

/*
 * NOTE:  This file has been derived from the lwip/src/include/lwip/opt.h
 * header file. For additional details, refer to the original "opt.h" file
 * and lwIP documentation.
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "ipAndMac.h"
#include "mem_datacopy.h"

#define CHECKSUM_BY_HARDWARE 1 // Enables the use of hardware for eth checksum offloading
//****************************************************************************
//
// -------------- Static IPv4 addresses  ----------------
// (relevant only when DHCP or AUTOIP are NOT configured
//
//****************************************************************************
// the default IP address of the host...
#ifndef DCC_MAC
#define DCC_MAC                         0x3b
#endif

// If the makefile does not define IP address, set it to our proper value for communicating with printer, 169.254.2.3
// The makefile must define all four (192.168.1.xx) in ipandMac.h, or none
#ifndef STATIC_IPADDR3 
#define STATIC_IPADDR0                  169
#define STATIC_IPADDR1                  254
#define STATIC_IPADDR2                  2
#define STATIC_IPADDR3                  3
#endif

#ifndef STATIC_IPADDR0
#error STATIC_IPADDR0 is not defined!
#endif

#ifndef STATIC_IPADDR1
#error STATIC_IPADDR1 is not defined!
#endif

#ifndef STATIC_IPADDR2
#error STATIC_IPADDR2 is not defined!
#endif

// the default network mask...
#define STATIC_NET_MASK0                255
#define STATIC_NET_MASK1                255
#define STATIC_NET_MASK2                255
#define STATIC_NET_MASK3                0

// the default gateway IP address...
#define STATIC_GW_IPADDR0               0
#define STATIC_GW_IPADDR1               0
#define STATIC_GW_IPADDR2               0
#define STATIC_GW_IPADDR3               0

/**
 * TODO: Until a better spot can be found, use this for MAC address as well.
 */
#define DEF_MAC_ADDR0                                   0x00
#define DEF_MAC_ADDR1                                   0x02
#define DEF_MAC_ADDR2                                   0x4E
#define DEF_MAC_ADDR3                                   0x09
#define DEF_MAC_ADDR4                                   0x00
#define DEF_MAC_ADDR5                                   DCC_MAC

/**
 * TODO: Until a better spot can be found, use this for IF name as well.
 */
#define IFNAME0                                                 "E"
#define IFNAME1                                                 "L"
//****************************************************************************
//
// ----------------  lwIP Port Options ----------
//
//****************************************************************************
/*
The STM32F207 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
 */
//#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
/* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 0
/* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                0
/* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                0
/* CHECKSUM_GEN_ICMP==0: Generate checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               0
/* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               0
/* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              0
/* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              0
#else
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                1
/* CHECKSUM_GEN_ICMP==0: Generate checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              1
#endif

//#define HOST_TMR_INTERVAL
//#define DHCP_EXPIRE_TIMER_MSECS         (60 * 1000)
#define TX_PBUF_QUEUE_LEN               8

#define TCP_TMR_INTERVAL               50

//****************************************************************************
//
// ---------- Platform specific locking ----------
//
//****************************************************************************
//#define SYS_LIGHTWEIGHT_PROT            0
#define NO_SYS                          1           // default is 0
//#define NO_SYS_NO_TIMERS                0
//#define MEMCPY(dst,src,len)             MEM_DataCopy(dst,src,len)
//#define SMEMCPY(dst,src,len)            MEM_DataCopy(dst,src,len)


//****************************************************************************
//
// ----------------  HTTPD Options ----------
//
//****************************************************************************
//#define INCLUDE_HTTPD_SSI
//#define INCLUDE_HTTPD_CGI
//#define DYNAMIC_HTTP_HEADERS
//#define INCLUDE_HTTPD_DEBUG

//****************************************************************************
//
// ---------- Memory options ----------
//
//****************************************************************************
//#define MEM_LIBC_MALLOC                 0
//#define MEMP_MEM_MALLOC                 0
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT                   4           // default is 1
#define MEM_SIZE                        (3 * 1024)  // default is 1600
//#define MEMP_SEPARATE_POOLS             0
//#define MEMP_OVERFLOW_CHECK             0
//#define MEMP_SANITY_CHECK               0
//#define MEM_USE_POOLS                   0
//#define MEM_USE_POOLS_TRY_BIGGER_POOL   0
//#define MEMP_USE_CUSTOM_POOLS           0
//#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0

//****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//****************************************************************************
/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           		100
//#define MEMP_NUM_RAW_PCB                4
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB				6
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. Technically, we don't need any since this
   application is all UDP */
#define MEMP_NUM_TCP_PCB                5          // default 5
//#define MEMP_NUM_TCP_PCB_LISTEN         8
//#define MEMP_NUM_TCP_SEG                16
//#define MEMP_NUM_REASSDATA              5
//#define MEMP_NUM_ARP_QUEUE              30
//#define MEMP_NUM_IGMP_GROUP             8
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT            10           // default 3
//#define MEMP_NUM_NETBUF                 2
//#define MEMP_NUM_NETCONN                4
//#define MEMP_NUM_TCPIP_MSG_API          8
//#define MEMP_NUM_TCPIP_MSG_INPKT        8
//#define MEMP_NUM_SNMP_NODE              50
//#define MEMP_NUM_SNMP_ROOTNODE          30
//#define MEMP_NUM_SNMP_VARBIND           2
//#define MEMP_NUM_SNMP_VALUE             3
//#define MEMP_NUM_NETDB                  1
//#define MEMP_NUM_LOCALHOSTLIST          1
//#define MEMP_NUM_PPPOE_INTERFACES       1
//#define PBUF_POOL_SIZE                  16

//****************************************************************************
//
// ---------- ARP options ----------
//
//****************************************************************************
//#define LWIP_ARP                        1
//#define ARP_TABLE_SIZE                  10
//#define ARP_QUEUEING                    0
//#define ETHARP_TRUST_IP_MAC             0
//#define ETHARP_SUPPORT_VLAN             0
//#define LWIP_ETHERNET                   (LWIP_ARP || PPPOE_SUPPORT)
//#define ETH_PAD_SIZE                    0
//#define ETHARP_SUPPORT_STATIC_ENTRIES   0

//****************************************************************************
//
// ---------- IP options ----------
//
//****************************************************************************
//#define IP_FORWARD                      0
//#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   1           // default is 1
#define IP_FRAG                         1           // default is 1
//#define IP_REASS_MAXAGE                 3
//#define IP_REASS_MAX_PBUFS              10
//#define IP_FRAG_USES_STATIC_BUF         0
//#define IP_FRAG_MAX_MTU                 1500
//#define IP_DEFAULT_TTL                  255
//#define IP_SOF_BROADCAST                0
//#define IP_SOF_BROADCAST_RECV           0

//****************************************************************************
//
// ---------- ICMP options ----------
//
//****************************************************************************
#define LWIP_ICMP                       1
#define ICMP_TTL                       (IP_DEFAULT_TTL)
#define LWIP_BROADCAST_PING             0
#define LWIP_MULTICAST_PING             0

//****************************************************************************
//
// ---------- RAW options ----------
//
//****************************************************************************
//#define LWIP_RAW                        1
//#define RAW_TTL                        (IP_DEFAULT_TTL)

//****************************************************************************
//
// ---------- DHCP options ----------
//
//****************************************************************************
#define LWIP_DHCP                       0           // default is 0
//#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))

//****************************************************************************
//
// ---------- AUTOIP options ----------
//
//****************************************************************************
//#define LWIP_AUTOIP                     1            // default is 0
//#define LWIP_DHCP_AUTOIP_COOP           0
//#define LWIP_DHCP_AUTOIP_COOP_TRIES     9

//****************************************************************************
//
// ---------- SNMP options ----------
//
//****************************************************************************
//#define LWIP_SNMP                       0
//#define SNMP_CONCURRENT_REQUESTS        1
//#define SNMP_TRAP_DESTINATIONS          1
//#define SNMP_PRIVATE_MIB                0
//#define SNMP_SAFE_REQUESTS              1
//#define SNMP_MAX_OCTET_STRING_LEN       127
//#define SNMP_MAX_TREE_DEPTH             15

//****************************************************************************
//
// ---------- IGMP options ----------
//
//****************************************************************************
//#define LWIP_IGMP                       0

//****************************************************************************
//
// ---------- DNS options -----------
//
//****************************************************************************
//#define LWIP_DNS                        0
//#define DNS_TABLE_SIZE                  4
//#define DNS_MAX_NAME_LENGTH             256
//#define DNS_MAX_SERVERS                 2
//#define DNS_DOES_NAME_CHECK             1
//#define DNS_USES_STATIC_BUF             1
//#define DNS_MSG_SIZE                    512
//#define DNS_LOCAL_HOSTLIST              0
//#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0

//****************************************************************************
//
// ---------- UDP options ----------
//
//****************************************************************************
#define LWIP_UDP                        1
//#define LWIP_UDPLITE                    0
#define UDP_TTL                         (IP_DEFAULT_TTL)
//#define LWIP_NETBUF_RECVINFO            0

//****************************************************************************
//
// ---------- UPNP options ----------
//
//****************************************************************************
//#define LWIP_UPNP                       0

//****************************************************************************
//
// ---------- PTPD options ----------
//
//****************************************************************************
//#define LWIP_PTPD                       0

//****************************************************************************
//
// ---------- TCP options ----------
//
//****************************************************************************
#define LWIP_TCP                        1
#define TCP_TTL                         (IP_DEFAULT_TTL)
#define TCP_WND                         (2 * TCP_MSS)
//#define TCP_MAXRTX                      12
//#define TCP_SYNMAXRTX                   6
//#define TCP_QUEUE_OOSEQ                 (LWIP_TCP)
#define TCP_MSS                         1460 // default is 536
//#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (2 * TCP_MSS) // default is 256
//#define TCP_SND_QUEUELEN     ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
//#define TCP_SNDQUEUELOWAT               ((TCP_SND_QUEUELEN)/2)
//#define TCP_LISTEN_BACKLOG              0
//#define TCP_DEFAULT_LISTEN_BACKLOG      0xFF
//#define TCP_OVERSIZE                    TCP_MSS
//#define LWIP_TCP_TIMESTAMPS             0
//#define TCP_WND_UPDATE_THRESHOLD        (TCP_WND / 4)

//****************************************************************************
//
// ---------- API options ----------
//
//****************************************************************************
//#define LWIP_EVENT_API                  1
#define LWIP_CALLBACK_API               1

//****************************************************************************
//
// ---------- Pbuf options ----------
//
//****************************************************************************
#define ETH_PAD_SIZE                    2           // default is 0
//#define PBUF_LINK_HLEN                  (14 + ETH_PAD_SIZE)
//#define PBUF_POOL_BUFSIZE     LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)

//****************************************************************************
//
// ---------- Network Interfaces options ----------
//
//****************************************************************************
#define LWIP_NETIF_HOSTNAME             1           // default is 0
//#define LWIP_NETIF_API                  0
//#define LWIP_NETIF_STATUS_CALLBACK      0
//#define LWIP_NETIF_LINK_CALLBACK        0
//#define LWIP_NETIF_HWADDRHINT           0
//#define LWIP_NETIF_LOOPBACK             0
//#define LWIP_LOOPBACK_MAX_PBUFS         0
//#define LWIP_NETIF_LOOPBACK_MULTITHREADING (!NO_SYS)
//#define LWIP_NETIF_TX_SINGLE_PBUF       0


//****************************************************************************
//
// ---------- LOOPIF options ----------
//
//****************************************************************************
//#define LWIP_HAVE_LOOPIF                0

//****************************************************************************
//
// ---------- SLIPIF options ----------
//
//****************************************************************************
//#define LWIP_HAVE_SLIPIF                0

//****************************************************************************
//
// ---------- Thread options ----------
//
//****************************************************************************
//#define TCPIP_THREAD_NAME              "tcpip_thread"
//#define TCPIP_THREAD_STACKSIZE          0
//#define TCPIP_THREAD_PRIO               1
//#define TCPIP_MBOX_SIZE                 0
//#define SLIPIF_THREAD_NAME             "slipif_loop"
//#define SLIPIF_THREAD_STACKSIZE         0
//#define SLIPIF_THREAD_PRIO              1
//#define PPP_THREAD_NAME                "pppInputThread"
//#define PPP_THREAD_STACKSIZE            0
//#define PPP_THREAD_PRIO                 1
//#define DEFAULT_THREAD_NAME            "lwIP"
//#define DEFAULT_THREAD_STACKSIZE        0
//#define DEFAULT_THREAD_PRIO             1
//#define DEFAULT_RAW_RECVMBOX_SIZE       0
//#define DEFAULT_UDP_RECVMBOX_SIZE       0
//#define DEFAULT_TCP_RECVMBOX_SIZE       0
//#define DEFAULT_ACCEPTMBOX_SIZE         0

//****************************************************************************
//
// ---------- Sequential layer options ----------
//
//****************************************************************************
//#define LWIP_TCPIP_CORE_LOCKING         0
//#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#define LWIP_NETCONN                    0           // default is 1
//#define LWIP_TCPIP_TIMEOUT              1

//****************************************************************************
//
// ---------- Socket Options ----------
//
//****************************************************************************
#define LWIP_SOCKET                     0           // default is 1
//#define LWIP_COMPAT_SOCKETS             1
//#define LWIP_POSIX_SOCKETS_IO_NAMES     1
//#define LWIP_TCP_KEEPALIVE              0
//#define LWIP_SO_RCVTIMEO                0
//#define LWIP_SO_RCVBUF                  0
//#define RECV_BUFSIZE_DEFAULT            INT_MAX
//#define SO_REUSE                        0
//#define SO_REUSE_RXTOALL                0

//****************************************************************************
//
// ---------- Statistics options ----------
//
//****************************************************************************
#define LWIP_STATS						      0
//#define LWIP_STATS_DISPLAY              0
#define LINK_STATS                        0
//#define ETHARP_STATS                    (LWIP_ARP)
//#define IP_STATS                        1
//#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
//#define ICMP_STATS                      1
//#define IGMP_STATS                      (LWIP_IGMP)
//#define UDP_STATS                       (LWIP_UDP)
//#define TCP_STATS                       (LWIP_TCP)
//#define MEM_STATS           ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
//#define MEMP_STATS                      (MEMP_MEM_MALLOC == 0)
//#define SYS_STATS                       (NO_SYS == 0)

//****************************************************************************
//
// ---------- PPP options ----------
//
//****************************************************************************
//#define PPP_SUPPORT                     0
//#define PPPOE_SUPPORT                   0
//#define PPPOS_SUPPORT                   PPP_SUPPORT

#if PPP_SUPPORT
//#define NUM_PPP                         1
//#define PAP_SUPPORT                     0
//#define CHAP_SUPPORT                    0
//#define MSCHAP_SUPPORT                  0
//#define CBCP_SUPPORT                    0
//#define CCP_SUPPORT                     0
//#define VJ_SUPPORT                      0
//#define MD5_SUPPORT                     0
//#define FSM_DEFTIMEOUT                  6
//#define FSM_DEFMAXTERMREQS              2
//#define FSM_DEFMAXCONFREQS              10
//#define FSM_DEFMAXNAKLOOPS              5
//#define UPAP_DEFTIMEOUT                 6
//#define UPAP_DEFREQTIME                 30
//#define CHAP_DEFTIMEOUT                 6
//#define CHAP_DEFTRANSMITS               10
//#define LCP_ECHOINTERVAL                0
//#define LCP_MAXECHOFAILS                3
//#define PPP_MAXIDLEFLAG                 100

//#define PPP_MAXMTU                      1500
//#define PPP_DEFMRU                      296
#endif

//****************************************************************************
//
// ---------- checksum options ----------
//
//****************************************************************************
//#define CHECKSUM_GEN_IP                 1
//#define CHECKSUM_GEN_UDP                1
//#define CHECKSUM_GEN_TCP                1
//#define CHECKSUM_CHECK_IP               1
//#define CHECKSUM_CHECK_UDP              1
//#define CHECKSUM_CHECK_TCP              1
//#define LWIP_CHECKSUM_ON_COPY           0

//****************************************************************************
//
// ---------- Debugging options ----------
//
//****************************************************************************
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF // LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_MIN_LEVEL                LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

//#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
#define LWIP_DBG_TYPES_ON  \
		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)

//#define ETHARP_DEBUG                    LWIP_DBG_ON     // default is OFF
//#define NETIF_DEBUG                     LWIP_DBG_ON     // default is OFF
#define PBUF_DEBUG                      LWIP_DBG_ON
//#define API_LIB_DEBUG                   LWIP_DBG_OFF
//#define API_MSG_DEBUG                   LWIP_DBG_OFF
//#define SOCKETS_DEBUG                   LWIP_DBG_OFF
//#define ICMP_DEBUG                      LWIP_DBG_OFF
//#define IGMP_DEBUG                      LWIP_DBG_OFF
//#define INET_DEBUG                      LWIP_DBG_OFF
//#define IP_DEBUG                        LWIP_DBG_ON     // default is OFF
//#define IP_REASS_DEBUG                  LWIP_DBG_OFF
//#define RAW_DEBUG                       LWIP_DBG_OFF
//#define MEM_DEBUG                       LWIP_DBG_OFF
//#define MEMP_DEBUG                      LWIP_DBG_OFF
//#define SYS_DEBUG                       LWIP_DBG_OFF
//#define TCP_DEBUG                       LWIP_DBG_OFF
//#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
//#define TCP_FR_DEBUG                    LWIP_DBG_OFF
//#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
//#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
//#define TCP_WND_DEBUG                   LWIP_DBG_OFF
//#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
//#define TCP_RST_DEBUG                   LWIP_DBG_OFF
//#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
//#define UDP_DEBUG                       LWIP_DBG_ON     // default is OFF
//#define TCPIP_DEBUG                     LWIP_DBG_OFF
//#define PPP_DEBUG                       LWIP_DBG_OFF
//#define SLIP_DEBUG                      LWIP_DBG_OFF
//#define DHCP_DEBUG                      LWIP_DBG_ON     // default is OFF
//#define AUTOIP_DEBUG                    LWIP_DBG_OFF
//#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
//#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
//#define DNS_DEBUG                       LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */
 /*********** Copyright (C) 2012 Harry Rostovtsev. All rights reserved *****END OF FILE****/
