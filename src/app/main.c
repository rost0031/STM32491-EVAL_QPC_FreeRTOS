// $Id$
/**
 * @file 	main.c
 * @brief   Main for starting up the Laminator
 *
 * @date   	09/27/2012
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
// $Log$
#include <stdio.h>
#include "qp_port.h"                                        /* for QP support */

#include "LWIPMgr.h"                               /* for starting LWIPMgr AO */
#include "CommStackMgr.h"                     /* for starting CommStackMgr AO */
#include "SerialMgr.h"                           /* for starting SerialMgr AO */

//#include "project_includes.h"
#include "Shared.h"
#include "version.h"

/* Local-scope objects -----------------------------------------------------*/
static QEvent const *l_CommStackMgrQueueSto[10];
static QEvent const *l_LWIPMgrQueueSto[10];
static QEvent const *l_SerialMgrQueueSto[10];
static QSubscrList   l_subscrSto[MAX_PUB_SIG];

static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvent)];
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
} l_medPoolSto[10];                    /* storage for the medium event pool */

static union LargeEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MsgEvt)];
    uint8_t e2[sizeof(EthEvt)];
    uint8_t e3[sizeof(SerialDataEvt)];
} l_lrgPoolSto[10];                    /* storage for the large event pool */

/*..........................................................................*/
int main(void) {

    /* initialize the Board Support Package */
    BSP_init();
    debug_printf("Initialized BSP\n");
    log_printf("Starting Bootloader version %s built on %s\n", FW_VER, BUILD_DATE);

	/* Instantiate the Active objects by calling their "constructors"          */
    debug_printf("Initializing AO constructors\n");
    SerialMgr_ctor();
    CommStackMgr_ctor();
    LWIPMgr_ctor();

    debug_printf("Initializing QF\n");
    QF_init();       /* initialize the framework and the underlying RT kernel */

    /* object dictionaries... */
    debug_printf("Initializing object dictionaries for QSPY\n");
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);
    QS_OBJ_DICTIONARY(l_lrgPoolSto);
    QS_OBJ_DICTIONARY(l_CommStackMgrQueueSto);
    QS_OBJ_DICTIONARY(l_LWIPMgrQueueSto);
    QS_OBJ_DICTIONARY(l_SerialMgrQueueSto);

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));     /* init publish-subscribe */

    /* initialize event pools... */
    debug_printf("Initializing event storage pools\n");
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
    QF_poolInit(l_lrgPoolSto, sizeof(l_lrgPoolSto), sizeof(l_lrgPoolSto[0]));

    /* Start Active objects */
    debug_printf("Starting Active Objects\n");

    QActive_start(AO_CommStackMgr, SERIAL_MGR_PRIORITY,
                  l_SerialMgrQueueSto, Q_DIM(l_SerialMgrQueueSto),
                  (void *)0, 0, (QEvent *)0);

    QActive_start(AO_CommStackMgr, COMM_STACK_PRIORITY,
                  l_CommStackMgrQueueSto, Q_DIM(l_CommStackMgrQueueSto),
                  (void *)0, 0, (QEvent *)0);

    QActive_start(AO_LWIPMgr, ETH_PRIORITY,
    			  l_LWIPMgrQueueSto, Q_DIM(l_LWIPMgrQueueSto),
                  (void *)0, 0, (QEvent *)0);

    log_printf("Bootloader started successfully\n");

    QF_run();                                       /* run the QF application */

    return(0);
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
