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
//#include "I2CMgr.h"                                 /* for starting I2CMgr AO */
#include "DbgMgr.h"                                 /* for starting DbgMgr AO */
#include "I2CBusMgr.h"                           /* for starting I2CBusMgr AO */
#include "I2C1DevMgr.h"                         /* for starting I2C1DevMgr AO */

#include "project_includes.h"           /* Includes common to entire project. */
#include "Shared.h"
#include "version.h"

#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_defs.h"
#include "bsp.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_GENERAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static QEvt const    *l_CommStackMgrQueueSto[100];  /**< Storage for CommStackMgr event Queue */
static QEvt const    *l_LWIPMgrQueueSto[100];       /**< Storage for LWIPMgr event Queue */
static QEvt const    *l_SerialMgrQueueSto[100];     /**< Storage for SerialMgr event Queue */
//static QEvt const    *l_I2CMgrQueueSto[100];        /**< Storage for I2CMgr event Queue */
static QEvt const    *l_I2CBusMgrQueueSto[100][MAX_I2C_BUS];    /**< Storage for I2CBusMgr event Queue */
static QEvt const    *l_I2C1DevMgrQueueSto[100];    /**< Storage for I2C1DevMgr event Queue */
static QEvt const    *l_DbgMgrQueueSto[100];        /**< Storage for DbgMgr event Queue */
static QSubscrList   l_subscrSto[MAX_PUB_SIG];      /**< Storage for subscribe/publish event Queue */

/**
 * \union Small Events.
 * This union is a storage for small sized events.
 */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvt)];
    uint8_t e2[sizeof(I2CEvt)];
    uint8_t e3[sizeof(I2CStatusEvt)];
    uint8_t e4[sizeof(I2CMemReadReqEvt)];
} l_smlPoolSto[100];                     /* storage for the small event pool */

/**
 * \union Medium Events.
 * This union is a storage for medium sized events.
 */
static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MenuEvt)];
    uint8_t e2[sizeof(I2CAddrEvt)];
    uint8_t e3[sizeof(I2CReadMemReqEvt)];
} l_medPoolSto[100];                    /* storage for the medium event pool */

/**
 * \union Large Events.
 * This union is a storage for large sized events.
 */
static union LargeEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MsgEvt)];
    uint8_t e2[sizeof(EthEvt)];
//    uint8_t e3[sizeof(I2CDataEvt)];
    uint8_t e4[sizeof(LogDataEvt)];
    uint8_t e5[sizeof(LrgDataEvt)];
} l_lrgPoolSto[200];                    /* storage for the large event pool */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*............................................................................*/
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
       startup_stm32f429_439xx.s/startup_stm32f401xx.s) before to branch to
       application main. To reconfigure the default setting of SystemInit()
       function, refer to system_stm32f4xx.c file
     */

    /* Enable debugging for select modules */
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_GENERAL);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_SERIAL);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_TIME);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_ETH);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_I2C);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_NOR);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_SDRAM);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_DBG);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_COMM);

    /* initialize the Board Support Package */
    BSP_init();

    dbg_slow_printf("Initialized BSP\n");
    log_slow_printf("Starting Bootloader version %s built on %s\n", FW_VER, BUILD_DATE);

    /* Instantiate the Active objects by calling their "constructors"         */
    dbg_slow_printf("Initializing AO constructors\n");
    SerialMgr_ctor();
    LWIPMgr_ctor();
//    I2CMgr_ctor( I2CBus1 );        /* Start this instance of AO for I2C1 bus. */

    /* Iterate though the available I2C busses on the system and call the ctor()
     * for each instance of the I2CBusMgr AO for each bus. */
    for( uint8_t i = 0; i < MAX_I2C_BUS; ++i ) {
       I2CBusMgr_ctor( i );      /* Start this instance of AO for this bus. */
    }

    I2C1DevMgr_ctor();
    CommStackMgr_ctor();
    DbgMgr_ctor();                           /* This AO should start up last */

    dbg_slow_printf("Initializing QF\n");
    QF_init();       /* initialize the framework and the underlying RT kernel */

    /* object dictionaries... */
    dbg_slow_printf("Initializing object dictionaries for QSPY\n");
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);
    QS_OBJ_DICTIONARY(l_lrgPoolSto);
    QS_OBJ_DICTIONARY(l_SerialMgrQueueSto);
    QS_OBJ_DICTIONARY(l_LWIPMgrQueueSto);
//    QS_OBJ_DICTIONARY(l_I2CMgrQueueSto);
    QS_OBJ_DICTIONARY(l_I2CBusMgrQueueSto);
    QS_OBJ_DICTIONARY(l_I2C1DevMgrQueueSto);
    QS_OBJ_DICTIONARY(l_CommStackMgrQueueSto);
    QS_OBJ_DICTIONARY(l_DbgMgrQueueSto);

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));     /* init publish-subscribe */

    /* initialize event pools... */
    dbg_slow_printf("Initializing event storage pools\n");
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
    QF_poolInit(l_lrgPoolSto, sizeof(l_lrgPoolSto), sizeof(l_lrgPoolSto[0]));

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

    QACTIVE_START(AO_DbgMgr,
          DBG_MGR_PRIORITY,                                       /* priority */
          l_DbgMgrQueueSto, Q_DIM(l_DbgMgrQueueSto),             /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

//    QACTIVE_START(AO_I2CMgr,
//          I2C_MGR_PRIORITY,                                       /* priority */
//          l_I2CMgrQueueSto, Q_DIM(l_I2CMgrQueueSto),             /* evt queue */
//          (void *)0, 0U,                               /* no per-thread stack */
//          (QEvt *)0                                /* no initialization event */
//    );

    /* Iterate though the available I2C busses on the system and start an
     * instance of the I2CBusMgr AO for each bus.
     * WARNING!!!: make sure that the priorities for them are all together since
     * this loop will iterates through them and will take another AO's priority.
     * You will end up with clashing priorities for your AOs.*/
    for( uint8_t i = 0; i < MAX_I2C_BUS; ++i ) {
    QACTIVE_START(AO_I2CBusMgr[i],
          I2CBUS1MGR_PRIORITY + i,                                /* priority */
          l_I2CBusMgrQueueSto[i], Q_DIM(l_I2CBusMgrQueueSto[i]), /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );
    }

    QACTIVE_START(AO_I2C1DevMgr,
          I2C1DEVMGR_PRIORITY,                                    /* priority */
          l_I2C1DevMgrQueueSto, Q_DIM(l_I2C1DevMgrQueueSto),     /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

    QACTIVE_START(AO_CommStackMgr,
          COMM_MGR_PRIORITY,                                      /* priority */
          l_CommStackMgrQueueSto, Q_DIM(l_CommStackMgrQueueSto), /* evt queue */
          (void *)0, 0U,                               /* no per-thread stack */
          (QEvt *)0                                /* no initialization event */
    );

    log_slow_printf("Starting QPC. All logging from here on out shouldn't show 'SLOW'!!!\n\n");
    QF_run();                                       /* run the QF application */

    return(0);
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
