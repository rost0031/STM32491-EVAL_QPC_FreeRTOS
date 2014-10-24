/**
 * @file 	Shared.h
 * @brief   Contains all the Shared Events, Priorities, and any other needed
 * 			declarations for the Shared state machines.  This is necessary to
 * 			make sure there is no overlap in enumerations between	the shared
 * 			code and the code pulling the shared code in.
 *
 * @date   	06/23/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SHARED_H_
#define SHARED_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "CBErrors.h"                               /* for system error codes */
#include "CBSignals.h"                                  /* for system signals */

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Global maximum length of message buffers (serial and ethernet).
 */
#define MAX_MSG_LEN                                                        300

/* Exported macros -----------------------------------------------------------*/
/**
 * @brief STM32 optimized MEMCPY.
 * This STM32 optimized MEMCPY is much faster than the regular one provided by
 * standard libs.  It's specifically tuned for arm cortex M3 processors and
 * written in very fast assembly.  Use it instead of regular memcpy */
#define MEMCPY(dst,src,len)            MEM_DataCopy(dst,src,len)
#define SMEMCPY(dst,src,len)           MEM_DataCopy(dst,src,len)

#ifndef CB_UNUSED_ARG
#define CB_UNUSED_ARG(x) (void)x
#endif
/* Exported types ------------------------------------------------------------*/
/**
 * \enum Active Object priorities.
 * These are the priorities of all the Active Objects in the system.  The
 * priorities are lowest at zero.
 * @note 1: Never use priority 0.
 */
enum AO_Priorities {
   NEVER_USE_ZERO_PRIORITY = 0,   /**< Never use this.  It breaks everything. */
   ETH_PRIORITY,       /**< Priority of LWIP AO which handles ethernet comms. */
   SERIAL_MGR_PRIORITY,                        /**< Priority of SerialMgr AO. */
   COMM_MGR_PRIORITY,                       /**< Priority of CommStackMgr AO. */
   I2C_MGR_PRIORITY,                              /**< Priority of I2CMgr AO. */

   /* Insert new priorities here ... */
   DBG_MGR_PRIORITY,                             /**< Priority of MenuMgr AO. */
};

/* These need to be visible to LWIPMgr AO, which is part of a shared port. Most
 * event types should be defined within their respective AOs. */

/**
 * \enum Source of the message.
 */
typedef enum MsgSrcTag {
   NA_SRC_DST     = 0x00000000,  /**< No src/dst info */
   SERIAL_CON     = 0x00000001,  /**< Message to/from the console serial port */
   ETH_PORT_SYS   = 0x00000002,  /**< Message to/from the the sys tcp port */
   ETH_PORT_LOG   = 0x00000004,  /**< Message to/from the the log tcp port */
} MsgSrc;

/**
 * \struct Struct for messages.
 * Uses CommStackSignals for signal names.  These events are responsible
 * for getting data from Eth/Serial to CommStackMgr
 */
typedef struct MsgEvtTag {
/* protected: */
    QEvt super;
    MsgSrc msg_src;                                /**< Source of the message */
    uint16_t msg_len;                           /**< Length of the msg buffer */
    char msg[MAX_MSG_LEN];        /**< Buffer that holds the data of the msg. */
} MsgEvt;

/**
 * @brief Event type for transferring large data.
 */
typedef struct LrgDataEvtTag {
/* protected: */
    QEvt       super;
    MsgSrc     src;                                   /**< Source of the data */
    MsgSrc     dst;                              /**< Destination of the data */
    uint16_t   dataLen;                    /**< Length of the data in dataBuf */
    uint8_t    dataBuf[MAX_MSG_LEN];          /**< Buffer that holds the data */
} LrgDataEvt;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#endif                                                           /* SHARED_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
