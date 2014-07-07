/**
 * @file 	main.c
 * @brief   Main for starting up the QPC RTOS.
 *
 * @date   	06/11/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */

#include "LWIPMgr.h"                               /* for starting LWIPMgr AO */
#include "CommStackMgr.h"                     /* for starting CommStackMgr AO */
#include "SerialMgr.h"                           /* for starting SerialMgr AO */
#include "I2CMgr.h"                                 /* for starting I2CMgr AO */

#include "project_includes.h"           /* Includes common to entire project. */
#include "Shared.h"
#include "version.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static QEvt const    *l_CommStackMgrQueueSto[50];  /**< Storage for CommStackMgr event Queue */
static QEvt const    *l_LWIPMgrQueueSto[50];       /**< Storage for LWIPMgr event Queue */
static QEvt const    *l_SerialMgrQueueSto[50];     /**< Storage for SerialMgr event Queue */
static QEvt const    *l_I2CMgrQueueSto[50];        /**< Storage for I2CMgr event Queue */
static QSubscrList   l_subscrSto[MAX_PUB_SIG];     /**< Storage for subscribe/publish event Queue */

/**
 * \union Small Events.
 * This union is a storage for small sized events.
 */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvt)];
} l_smlPoolSto[20];                     /* storage for the small event pool */

/**
 * \union Medium Events.
 * This union is a storage for medium sized events.
 */
static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MsgEvt)];
    uint8_t e2[sizeof(EthEvt)];
    uint8_t e3[sizeof(SerialDataEvt)];
    uint8_t e4[sizeof(I2CDataEvt)];
} l_medPoolSto[100];                    /* storage for the medium event pool */

/**
 * \union Large Events.
 * This union is a storage for large sized events.
 */
//static union LargeEvents {
//    void   *e0;                                       /* minimum event size */
//    uint8_t e1[sizeof(MsgEvt)];
//    uint8_t e2[sizeof(EthEvt)];
//    uint8_t e3[sizeof(SerialDataEvt)];
//} l_lrgPoolSto[10];                    /* storage for the large event pool */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*............................................................................*/
int main(void)
{
    /* initialize the Board Support Package */
    BSP_init();
    dbg_slow_printf("Initialized BSP\n");
    log_slow_printf("Starting Bootloader version %s built on %s\n", FW_VER, BUILD_DATE);

    /* Instantiate the Active objects by calling their "constructors"          */
    dbg_slow_printf("Initializing AO constructors\n");
    SerialMgr_ctor();
    LWIPMgr_ctor();
    I2CMgr_ctor();
    CommStackMgr_ctor();

    dbg_slow_printf("Initializing QF\n");
    QF_init();       /* initialize the framework and the underlying RT kernel */

    /* object dictionaries... */
    dbg_slow_printf("Initializing object dictionaries for QSPY\n");
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);
//    QS_OBJ_DICTIONARY(l_lrgPoolSto);
    QS_OBJ_DICTIONARY(l_SerialMgrQueueSto);
    QS_OBJ_DICTIONARY(l_LWIPMgrQueueSto);
    QS_OBJ_DICTIONARY(l_I2CMgrQueueSto);
    QS_OBJ_DICTIONARY(l_CommStackMgrQueueSto);

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));     /* init publish-subscribe */

    /* initialize event pools... */
    dbg_slow_printf("Initializing event storage pools\n");
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
    //    QF_poolInit(l_lrgPoolSto, sizeof(l_lrgPoolSto), sizeof(l_lrgPoolSto[0]));

    /* Start Active objects */
    dbg_slow_printf("Starting Active Objects\n");

    QACTIVE_START(AO_SerialMgr,
          SERIAL_MGR_PRIORITY,                                    /* priority */
          l_SerialMgrQueueSto, Q_DIM(l_SerialMgrQueueSto),       /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

    QACTIVE_START(AO_LWIPMgr,
          ETH_PRIORITY,                                           /* priority */
          l_LWIPMgrQueueSto, Q_DIM(l_LWIPMgrQueueSto),           /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

    QACTIVE_START(AO_I2CMgr,
          I2C_MGR_PRIORITY,                                       /* priority */
          l_I2CMgrQueueSto, Q_DIM(l_I2CMgrQueueSto),             /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

    QACTIVE_START(AO_CommStackMgr,
          COMM_MGR_PRIORITY,                                      /* priority */
          l_CommStackMgrQueueSto, Q_DIM(l_CommStackMgrQueueSto), /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );


    log_slow_printf("Bootloader started successfully.\n");
    log_slow_printf("Starting QPC.  All logging from here on out should not show 'SLOW'!!!\n");
    QF_run();                                       /* run the QF application */

    return(0);
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
