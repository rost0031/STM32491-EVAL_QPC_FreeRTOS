// $Id$
/**
 * @file 	Shared.h
 * @brief   Contains all the Shared Events, Priorities, and any other needed
 * 			declarations for the Shared Laminator state machines.  This is
 * 			necessary to make sure there is no overlap in enumerations between
 * 			the shared code and the code pulling the shared code in.
 *
 * @date   	10/01/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

#ifndef SHARED_H_
#define SHARED_H_

#include "qp_port.h"

#define MAX_MSG_LEN                    512

/* This is a MEMCPY that is way faster than the regular one provided by standard
 * libs.  It's specifically tuned for arm cortex M3 processors and written in
 * very fast assembly.  Use it instead of regular memcpy */
#define MEMCPY(dst,src,len)            MEM_DataCopy(dst,src,len)
#define SMEMCPY(dst,src,len)           MEM_DataCopy(dst,src,len)

/* These two macros are handy for debugging and will be disabled if NDEBUG is not set */
/* This is the regular debug print function which includes more info */
#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

/**
 * Use the following functions for printf style debugging.  They will be
 * automatically disabled via macros when a "rel" build is done (as opposed to
 * a dbg or a spy build). debug_printf will print a lot more info than
 * isr_debug_printf
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * debug_printf("Debug print test\n");
 * debug_printf("Debug print test %d\n",i);
 * @endcode
 *
 * will output:
 * DEBUG: ../src/main.c:78:main(): Debug print test
 * DEBUG: ../src/main.c:78:main(): Debug print test 0
 *
 * @code
 * isr_debug_printf("ISR Debug print test\n");
 * isr_debug_printf("ISR Debug print test %d\n", i);
 * @endcode
 *
 * will output:
 * D ISR Debug print test
 * D ISR Debug print test 0
 */
#define debug_printf(fmt, ...) \
      do { if (DEBUG) fprintf(stderr, "DBG: %d:%s(): " fmt, \
            __LINE__, __func__, ##__VA_ARGS__); } while (0)

/* This is the isr debug print function which is more concise */
#define isr_debug_printf(fmt, ...) \
      do { if (DEBUG) fprintf(stderr, "D:" fmt, ##__VA_ARGS__); } while (0)

/* This is just like debug_printf but it doesn't get compiled out if NDEBUG is
 * not set.  It should be used for logging errors only*/
#define err_printf(fmt, ...) \
      do { fprintf(stderr, "ERR: %d:%s(): " fmt, \
            __LINE__, __func__, ##__VA_ARGS__); \
      } while (0)

/* This is just like debug_printf but it doesn't get compiled out if NDEBUG is
 * not set.  It should be used for general info logging */
#define log_printf(fmt, ...) \
      do { fprintf(stderr, "LOG: " fmt, \
            ##__VA_ARGS__); \
      } while (0)

/* These are the priorities of all the Shared Active Objects.  The priorities are lowest at zero. */
enum AO_Priorities {
   NEVER_USE_ZERO_PRIORITY	= 0,  // Never use this.  It breaks everything
   FWFLASHMGR_PRIORITY,
   COMM_STACK_PRIORITY,
   ETH_PRIORITY,        // Ethernet should always be almost the lowest priority
   MAX_SHARED_PRIORITY  // This should always be at the end of this enum list since other modules will reference it
};

typedef enum MsgSrcTag {
   SERIAL = 0,
} MsgSrc;

/* MsgEvt types will use CommStackSignals for signal names.  These events are
 * responsible for getting data from Eth/serial to CommStackMgr */
typedef struct MsgEvtTag {
   QEvent      super;
   MsgSrc      msg_src;
   uint16_t    msg_len;
   char        msg[MAX_MSG_LEN];
} MsgEvt;

enum CommStackSignals {
   /* Q_USER_SIG is the first signal that user applications are allowed to use.
    * Should always be at the very top of the very first enum list
    */
   MSG_SEND_OUT_SIG = Q_USER_SIG,
   MSG_RECEIVED_SIG,
   MSG_FROM_RFID_RECEIVED_SIG,
   MSG_TO_RFID_SEND_OUT_SIG,
   MSG_FROM_RFID1_REC_SIG,
   MSG_FROM_RFID2_REC_SIG,
   MSG_MAX_SIG,
};

enum FWDataSignals {
   FWDATA_INIT_FW_UPGRADE_SIG = MSG_MAX_SIG,
   FWDATA_PACKET_REC_SIG,
   FWDATA_FINISH_FW_UPGRADE_SIG,
   FWDATA_MAX_SIG,
};

/* BtldrEvt types will use BootloaderSignals for signal names.  These events are
 * general bootloader events that don't carry any data and are for general use*/
typedef struct BtldrEvtTag {
   QEvent		super;
} BtldrEvt;

enum BootloaderSignals {
   BTLDR_FLASH_READY_SIG = FWDATA_MAX_SIG,
   BTLDR_FLASH_WRITE_PASS_SIG,
   BTLDR_FLASH_WRITE_FAIL_SIG,
   BTLDR_FLASH_ERROR_SIG,
   BTLDR_TIMEOUT_SIG,
   BTLDR_BOOT_APP_TIMEOUT_SIG,
   BTLDR_BOOT_APP_SIG,
   BTLDR_MAX_SIG,

};

/* INSERT NEW SIGNAL CATEGORIES BEFORE HERE...POINT MAX_SHARED_SIG TO YOUR LAST SIGNAL */
enum FinalSignal {
   MAX_SHARED_SIG = BTLDR_MAX_SIG,  /*Last published shared signal - should always be at the bottom of this list */
};

#endif                                                           /* SHARED_H_ */
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
