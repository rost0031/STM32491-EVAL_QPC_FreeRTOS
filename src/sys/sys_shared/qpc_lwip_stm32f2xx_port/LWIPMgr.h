// $Id$
/**
 * @file 	LWIPMgr.h
 * @brief   This file contains the declarations of the LWIPMgr
 * 			Active Object and their state machines.
 *
 * @date   	09/28/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$

#ifndef LWIPMGR_H_
#define LWIPMGR_H_

#include "qf.h"                                             /* for QF support */
#include "Shared.h"
#include "project_includes.h"                       /* for motor enums support*/

enum LWIPMgrSignals {
   ETH_SEND_SIG = MAX_SHARED_SIG,          
   MAX_PUB_SIG,                                  /* the last published signal */
};

typedef struct EthEvtTag {
	QEvent super;
	uint8_t source;
	uint8_t msg_len;
	char msg[MAX_MSG_LEN];
} EthEvt;

extern QActive * const AO_LWIPMgr;         /* "opaque" pointer  to LWIPMgr AO */

void LWIPMgr_ctor(void);

/**
  * This function is the implementation of a stub function in CommStackMgr.  It
  * is used to do the actual sending of data over ethernet on the embedded side.
  * The simulator will have its own implementation of this function due to the
  * use of totally different ip stacks and threading paradigms.
  *
  * @param  e: a MsgEvt const event pointer to the MsgEvt that contains the
  * length and message to be sent.
  *
  * @return None
  */
void ETH_SendMsg_Handler(MsgEvt const *e);


#endif /* LWIPMGR_H_ */
/******** Copyright (C) 2012 Datacard. All rights reserved *****END OF FILE****/
