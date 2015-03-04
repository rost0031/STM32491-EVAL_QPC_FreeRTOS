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
#include "FreeRTOS.h"                                 /* for FreeRTOS support */

#include "LWIPMgr.h"                               /* for starting LWIPMgr AO */
#include "CommStackMgr.h"                     /* for starting CommStackMgr AO */
#include "SerialMgr.h"                           /* for starting SerialMgr AO */
#include "DbgMgr.h"                                 /* for starting DbgMgr AO */
#include "I2CBusMgr.h"                           /* for starting I2CBusMgr AO */
#include "I2C1DevMgr.h"                         /* for starting I2C1DevMgr AO */
#include "cplr.h"                               /* for starting the CPLR task */

#include "project_includes.h"           /* Includes common to entire project. */
#include "Shared.h"
#include "version.h"

#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_defs.h"
#include "bsp.h"
#include "db.h"                                       /* for settings support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_GENERAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define THREAD_STACK_SIZE  1024U

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static QEvt const    *l_CommStackMgrQueueSto[30];  /**< Storage for CommStackMgr event Queue */
static QEvt const    *l_LWIPMgrQueueSto[200];       /**< Storage for LWIPMgr event Queue */
static QEvt const    *l_SerialMgrQueueSto[200];     /**< Storage for SerialMgr event Queue */
static QEvt const    *l_I2CBusMgrQueueSto[30][MAX_I2C_BUS];    /**< Storage for I2CBusMgr event Queue */
static QEvt const    *l_I2C1DevMgrQueueSto[30];    /**< Storage for I2C1DevMgr event Queue */
static QEvt const    *l_DbgMgrQueueSto[30];        /**< Storage for DbgMgr event Queue */
static QSubscrList   l_subscrSto[MAX_PUB_SIG];      /**< Storage for subscribe/publish event Queue */

static QEvt const    *l_CPLRQueueSto[10]; /**< Storage for raw QE queue for communicating with CPLR task */
/**
 * \union Small Events.
 * This union is a storage for small sized events.
 */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvt)];
    uint8_t e2[sizeof(I2CStatusEvt)];
    uint8_t e3[sizeof(I2CEEPROMReadReqEvt)];
} l_smlPoolSto[50];                     /* storage for the small event pool */

/**
 * \union Medium Events.
 * This union is a storage for medium sized events.
 */
static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MenuEvt)];
    uint8_t e2[sizeof(I2CAddrEvt)];
    uint8_t e3[sizeof(I2CReadMemReqEvt)];
    uint8_t e4[sizeof(I2CDevRegWriteReqEvt)];
    uint8_t e5[sizeof(I2CDevRegReadReqEvt)];
} l_medPoolSto[50];                    /* storage for the medium event pool */

/**
 * \union Large Events.
 * This union is a storage for large sized events.
 */
static union LargeEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(MsgEvt)];
    uint8_t e2[sizeof(EthEvt)];
    uint8_t e3[sizeof(LogDataEvt)];
    uint8_t e4[sizeof(LrgDataEvt)];
    uint8_t e5[sizeof(I2CEEPROMWriteReqEvt)];
} l_lrgPoolSto[100];                    /* storage for the large event pool */

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

    /* Enable debugging for select modules - Note: this has no effect in rel
     * builds since all DBG level logging is disabled and only LOG and up msgs
     * will get printed. */
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_GENERAL);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_SERIAL);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_TIME);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_ETH);
    DBG_DISABLE_DEBUG_FOR_MODULE(DBG_MODL_I2C);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_NOR);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_SDRAM);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_DBG);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_COMM);
    DBG_ENABLE_DEBUG_FOR_MODULE(DBG_MODL_CPLR);

    /* initialize the Board Support Package */
    BSP_init();

    dbg_slow_printf("Initialized BSP\n");
    log_slow_printf("Starting Bootloader version %s built on %s\n", FW_VER, BUILD_DATE);

    log_slow_printf("Checking settings DB validity...\n");
    CBErrorCode status = DB_isValid();
    if ( ERR_NONE != status ) {
       wrn_slow_printf("Settings DB validity check returned: 0x%08x\n", status);
       wrn_slow_printf("Attempting to write default DB to EEPROM...\n");
       status = DB_initToDefault( ACCESS_BARE_METAL );
       if ( ERR_NONE != status ) {
          err_slow_printf("Unable to write default DB to EEPROM. Error: 0x%08x\n", status);
       } else {
          log_slow_printf("Wrote default DB to EEPROM. Attempting to validate...\n");
          status = DB_isValid();
          if ( ERR_NONE != status ) {
             err_slow_printf("DB validity check returned: 0x%08x\n", status);
             err_slow_printf("Unable to fix DB in EEPROM\n");
          } else {
             log_slow_printf("A default DB has been successfully written to EEPROM\n");
          }
       }
    } else {
       log_slow_printf("Valid settings DB found.\n");
    }

    dbg_slow_printf("Reading the MAC address from the settings DB...\n");
    uint8_t macAddrBuffer[6];
    memset(macAddrBuffer, 0, sizeof(macAddrBuffer));

    status = DB_getElemBLK(
          DB_MAC_ADDR,
          macAddrBuffer,
          sizeof(macAddrBuffer),
          ACCESS_BARE_METAL
    );
    if ( ERR_NONE != status ) {
       err_slow_printf("Unable to read stored MAC address, error: 0x%08x\n", status);
    } else {
       log_slow_printf(
             "Read MAC address from settings DB: %02x:%02x:%02x:%02x:%02x:%02x\n",
             macAddrBuffer[0], macAddrBuffer[1], macAddrBuffer[2],
             macAddrBuffer[3], macAddrBuffer[4], macAddrBuffer[5]
       );
    }

    /* Read the stored IP address from DB */
    dbg_slow_printf("Reading the IP address from the settings DB...\n");
    uint8_t ipAddrBuffer[4];
    memset(ipAddrBuffer, 0, sizeof(ipAddrBuffer));
    status = DB_getElemBLK(
          DB_IP_ADDR,
          ipAddrBuffer,
          sizeof(ipAddrBuffer),
          ACCESS_BARE_METAL
    );

    if ( ERR_NONE != status ) {
       err_slow_printf("Unable to read stored IP address, error: 0x%08x\n", status);
    } else {
       log_slow_printf(
             "Read IP address from settings DB: %d:%d:%d:%d\n",
             ipAddrBuffer[0], ipAddrBuffer[1], ipAddrBuffer[2], ipAddrBuffer[3]
       );
    }

    /* Instantiate the Active objects by calling their "constructors"         */
    dbg_slow_printf("Initializing AO constructors\n");
    SerialMgr_ctor();
    LWIPMgr_ctor();

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

    /* initialize the raw queue */
    QEQueue_init(&CPLR_evtQueue, l_CPLRQueueSto, Q_DIM(l_CPLRQueueSto));

    /* Start Active objects */
    dbg_slow_printf("Starting Active Objects\n");

    QACTIVE_START(AO_SerialMgr,
          SERIAL_MGR_PRIORITY,                                    /* priority */
          l_SerialMgrQueueSto, Q_DIM(l_SerialMgrQueueSto),       /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "SerialMgr"                                     /* Name of the task */
    );

    QACTIVE_START(AO_LWIPMgr,
          ETH_PRIORITY,                                           /* priority */
          l_LWIPMgrQueueSto, Q_DIM(l_LWIPMgrQueueSto),           /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "LWIPMgr"                                       /* Name of the task */
    );

    QACTIVE_START(AO_DbgMgr,
          DBG_MGR_PRIORITY,                                       /* priority */
          l_DbgMgrQueueSto, Q_DIM(l_DbgMgrQueueSto),             /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "DbgMgr"                                        /* Name of the task */
    );

    /* Iterate though the available I2C busses on the system and start an
     * instance of the I2CBusMgr AO for each bus.
     * WARNING!!!: make sure that the priorities for them are all together since
     * this loop will iterates through them and will take another AO's priority.
     * You will end up with clashing priorities for your AOs.*/
    for( uint8_t i = 0; i < MAX_I2C_BUS; ++i ) {
    QACTIVE_START(AO_I2CBusMgr[i],
          I2CBUS1MGR_PRIORITY + i,                                /* priority */
          l_I2CBusMgrQueueSto[i], Q_DIM(l_I2CBusMgrQueueSto[i]), /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "I2CBusMgr"                                     /* Name of the task */
    );
    }

    QACTIVE_START(AO_I2C1DevMgr,
          I2C1DEVMGR_PRIORITY,                                    /* priority */
          l_I2C1DevMgrQueueSto, Q_DIM(l_I2C1DevMgrQueueSto),     /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "I2CDevMgr"                                     /* Name of the task */
    );

    QACTIVE_START(AO_CommStackMgr,
          COMM_MGR_PRIORITY,                                      /* priority */
          l_CommStackMgrQueueSto, Q_DIM(l_CommStackMgrQueueSto), /* evt queue */
          (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
          (QEvt *)0,                               /* no initialization event */
          "CommMgr"                                       /* Name of the task */
    );

    xTaskCreate(
          CPLR_Task,
          ( const char * ) "CPLRTask",                    /* Name of the task */
          THREAD_STACK_SIZE,                         /* per-thread stack size */
          NULL,
          CPLR_PRIORITY,                                          /* priority */
          ( xTaskHandle * ) NULL
    );

    log_slow_printf("Starting QPC. All logging from here on out shouldn't show 'SLOW'!!!\n\n");
    QF_run();                                       /* run the QF application */

    return(0);
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
