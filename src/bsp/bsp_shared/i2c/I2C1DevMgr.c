/*****************************************************************************
* Model: I2C1DevMgr.qm
* File:  ./I2C1DevMgr_gen.c
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/*${.::I2C1DevMgr_gen.c} ...................................................*/
/**
 * @file    I2C1DevMgr.c
 * @brief   Declarations for functions for the I2C1DevMgr AO.
 * This state machine handles any device that is attached to the I2C1 bus.
 * This AO doesn't handle the low level I2C commands and instead communicates
 * with I2CBusMgr AO to send the events that kick off the low level I2C cmds.
 * The rationale behind this is that different I2C devices require different
 * I2C commands in different order.  Some EEPROMs request 2 Start bits, some
 * other devices have different timeouts for their write/read commands. This
 * way, the I2C bus logic can stay common and any device differences are
 * handled in the device specific AOs.
 *
 * @note 1: If editing this file, please make sure to update the I2C1DevMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    10/24/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "I2C1DevMgr.h"
#include "project_includes.h"           /* Includes common to entire project. */
#include "bsp_defs.h"     /* For seconds to bsp tick conversion (SEC_TO_TICK) */
#include "i2c.h"                                  /* For I2C bus declarations */
#include "i2c_dev.h"                           /* For I2C device declarations */
#include "I2CBusMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/

/**
 * @brief I2C1DevMgr Active Object (AO) "class" that manages the all the I2C
 * devices on the I2C1 Bus.
 * This AO manages the devices connected to the I2C1 bus and all events a
 * ssociated with those devices. It deesn't access to the I2C1 bus directly and
 * instead communicates with the I2CBusMgr AO to request and monitor the direct
 * I2C commands that need to be sent down that are specific for the device that
 * is currently being handled.  See I2CDevMgr.qm for diagram and model.
 */
/*${AOs::I2C1DevMgr} .......................................................*/
typedef struct {
/* protected: */
    QActive super;

    /**< Native QF queue for deferred request events. */
    QEQueue deferredEvtQueue;

    /**< Storage for deferred event queue. */
    QTimeEvt const * deferredEvtQSto[100];

    /**< Specifies which I2CBus1 device is currently being handled by this AO.
     * This should be set when a new I2C_READ_START or I2C_WRITE_START events come
     * in.  Those events should contain the device for which they are meant for. */
    I2CBus1_Dev_t iDev;

    /**< Which I2C bus this AO is responsible for.  This variable is set on
         startup and is used to index into the structure that holds all the
         I2C bus settings. */
    I2C_Bus_t iBus;

    /**< QPC timer Used to timeout overall I2C device interactions. */
    QTimeEvt i2cTimerEvt;

    /**< QPC timer Used to timeout discrete I2C bus requests to the I2CBusMgr AO. */
    QTimeEvt i2cOpTimerEvt;

    /**< Total number of bytes to be read or written with an operation */
    uint16_t bytesTotal;

    /**< Number of bytes already read or written as part of the bytesTotal.
     * This is for R/W operations that don't fit into a single bus request */
    uint16_t bytesCurr;

    /**< Keep track of the starting internal memory address of a read or a
     write operation */
    uint16_t addrStart;

    /**< Number of bytes the "address" variables are */
    uint8_t addrSize;

    /**< Keep track of the last memory location accessed.  This is for R/W operations
     * that don't fit into a single bus request */
    uint16_t addrCurr;

    /**< Keep track of last error that occurs. */
    CBErrorCode errorCode;
} I2C1DevMgr;

/* protected: */
static QState I2C1DevMgr_initial(I2C1DevMgr * const me, QEvt const * const e);

/**
 * @brief This state is a catch-all Active state.
 * If any signals need to be handled that do not cause state transitions and
 * are common to the entire AO, they should be handled here.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status_: QState type that specifies where the state
 * machine is going next.
 */
static QState I2C1DevMgr_Active(I2C1DevMgr * const me, QEvt const * const e);

/**
 * @brief   This state indicates that the I2C is currently busy and cannot
 * process incoming data; incoming events will be deferred in this state and
 * handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState I2C1DevMgr_Busy(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_GenerateStart(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_Send7BitAddrTxMode(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_SendInternalAddr(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_CheckingBus(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_GenerateStart1(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_Send7BitAddrRxMode(I2C1DevMgr * const me, QEvt const * const e);
static QState I2C1DevMgr_ReadMem(I2C1DevMgr * const me, QEvt const * const e);

/**
 * @brief This state indicates that the I2C bus is currently idle and the
 * incoming msg can be handled.
 * This state is the default rest state of the state machine and can handle
 * various I2C requests.  Upon entry, it also checks the deferred queue to see
 * if any request events are waiting which were posted while I2C bus was busy.
 * if there are any waiting, it will read them out, which automatically posts
 * them and the state machine will go and handle them.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState I2C1DevMgr_Idle(I2C1DevMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static I2C1DevMgr l_I2C1DevMgr;   /* the single instance of the active object */

/* Global-scope objects ----------))------------------------------------------*/
QActive * const AO_I2C1DevMgr = (QActive *)&l_I2C1DevMgr;/**< "opaque" AO pointer */

extern I2CBus1_DevSettings_t s_I2CBus1_Dev[MAX_I2C1_DEV]; /**< I2C devices this AO uses */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief C "constructor" for I2C1DevMgr "class".
 * Initializes all the timers and queues used by the AO, sets up a deferral
 * queue, and sets of the first state.
 * @param [in]: none.
 * @retval: none
 */
/*${AOs::I2C1DevMgr_ctor} ..................................................*/
void I2C1DevMgr_ctor(void) {
    I2C1DevMgr *me = &l_I2C1DevMgr;

    QActive_ctor( &me->super, (QStateHandler)&I2C1DevMgr_initial );
    QTimeEvt_ctor( &me->i2cTimerEvt, I2C1_DEV_TIMEOUT_SIG );
    QTimeEvt_ctor( &me->i2cOpTimerEvt, I2C1_DEV_OP_TIMEOUT_SIG );

    /* Initialize the deferred event queue and storage for it */
    QEQueue_init(
        &me->deferredEvtQueue,
        (QEvt const **)( me->deferredEvtQSto ),
        Q_DIM(me->deferredEvtQSto)
    );

    dbg_slow_printf("Constructor\n");
}

/**
 * @brief I2C1DevMgr Active Object (AO) "class" that manages the all the I2C
 * devices on the I2C1 Bus.
 * This AO manages the devices connected to the I2C1 bus and all events a
 * ssociated with those devices. It deesn't access to the I2C1 bus directly and
 * instead communicates with the I2CBusMgr AO to request and monitor the direct
 * I2C commands that need to be sent down that are specific for the device that
 * is currently being handled.  See I2CDevMgr.qm for diagram and model.
 */
/*${AOs::I2C1DevMgr} .......................................................*/
/*${AOs::I2C1DevMgr::SM} ...................................................*/
static QState I2C1DevMgr_initial(I2C1DevMgr * const me, QEvt const * const e) {
    /* ${AOs::I2C1DevMgr::SM::initial} */
    (void)e;        /* suppress the compiler warning about unused parameter */

    me->iBus = I2CBus1;          /* This AO only handles devices on I2CBus1 */

    QS_OBJ_DICTIONARY(&l_I2C1DevMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&I2C1DevMgr_initial);
    QS_FUN_DICTIONARY(&I2C1DevMgr_Active);
    QS_FUN_DICTIONARY(&I2C1DevMgr_Idle);

    QActive_subscribe((QActive *)me, EEPROM_RAW_MEM_READ_SIG);
    QActive_subscribe((QActive *)me, EEPROM_SN_READ_SIG);
    QActive_subscribe((QActive *)me, EEPROM_EUI64_READ_SIG);
    return Q_TRAN(&I2C1DevMgr_Idle);
}

/**
 * @brief This state is a catch-all Active state.
 * If any signals need to be handled that do not cause state transitions and
 * are common to the entire AO, they should be handled here.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status_: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::I2C1DevMgr::SM::Active} ...........................................*/
static QState I2C1DevMgr_Active(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            /* Post and disarm all the timer events so they can be rearmed at any time */
            QTimeEvt_postIn(
                &me->i2cTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV5 )
            );
            QTimeEvt_disarm(&me->i2cTimerEvt);

            QTimeEvt_postIn(
                &me->i2cOpTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV5 )
            );
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/**
 * @brief   This state indicates that the I2C is currently busy and cannot
 * process incoming data; incoming events will be deferred in this state and
 * handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::I2C1DevMgr::SM::Active::Busy} .....................................*/
static QState I2C1DevMgr_Busy(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy} */
        case Q_ENTRY_SIG: {
            /* Post a timer on entry */
            QTimeEvt_rearm(
                &me->i2cTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_DEV_REQ )
            );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm( &me->i2cTimerEvt ); /* Disarm timer on exit */
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::I2C1_DEV_TIMEOUT} */
        case I2C1_DEV_TIMEOUT_SIG: {
            ERR_printf("I2C1Dev timeout occurred with error: 0x%08x\n", me->errorCode);
            status_ = Q_TRAN(&I2C1DevMgr_Idle);
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::EEPROM_RAW_MEM_READ, EEPROM_SN_READ, EEPROM_EUI64_READ} */
        case EEPROM_RAW_MEM_READ_SIG: /* intentionally fall through */
        case EEPROM_SN_READ_SIG: /* intentionally fall through */
        case EEPROM_EUI64_READ_SIG: {
            if (QEQueue_getNFree(&me->deferredEvtQueue) > 0) {
               /* defer the request - this event will be handled
                * when the state machine goes back to Idle state */
               QActive_defer((QActive *)me, &me->deferredEvtQueue, e);
               DBG_printf("Deferring I2C request until current is done\n");
            } else {
               /* notify the request sender that the request was ignored.. */
               ERR_printf("Unable to defer I2C request\n");
            }
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Active);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart} ......................*/
static QState I2C1DevMgr_GenerateStart(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV5_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV5 )
            );

            /* Directly post an event to the appropriate I2CBusMgr AO */
            static QEvt const qEvt = { I2C_BUS_START_BIT_SIG, 0U, 0U };
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], &qEvt, me);

            DBG_printf("ActivePosted I2C_BUS_START_BIT\n");
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_Send7BitAddrTxMode);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode} .................*/
static QState I2C1DevMgr_Send7BitAddrTxMode(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV6_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV6 )
            );

            /* Allocate and directly post an event to the appropriate I2CBusMgr AO */
            I2CAddrEvt *i2cAddrEvt   = Q_NEW( I2CAddrEvt, I2C_BUS_SEND_7BIT_ADDR_SIG );
            i2cAddrEvt->i2cBus       = me->iBus;
            i2cAddrEvt->addr         = I2C_getI2C1DevAddr(me->iDev);
            i2cAddrEvt->addrSize     = I2C_getI2C1DevAddrSize(me->iDev);
            i2cAddrEvt->i2cDirection = I2C_Direction_Transmitter;
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], (QEvt *)i2cAddrEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_SendInternalAddr);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrTxMode::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr} ...................*/
static QState I2C1DevMgr_SendInternalAddr(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV8_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV8 )
            );

            /* Allocate and directly post an event to the appropriate I2CBusMgr AO */
            I2CAddrEvt *i2cAddrEvt   = Q_NEW( I2CAddrEvt, I2C_BUS_SEND_DEV_ADDR_SIG );
            i2cAddrEvt->i2cBus       = me->iBus;
            i2cAddrEvt->addr         = me->addrStart;
            i2cAddrEvt->addrSize     = I2C_getI2C1MemAddrSize(me->iDev);
            i2cAddrEvt->i2cDirection = I2C_Direction_Transmitter;
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], (QEvt *)i2cAddrEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_GenerateStart1);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::SendInternalAddr::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus} ........................*/
static QState I2C1DevMgr_CheckingBus(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_CHECK_BUS_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_BUS_CHECK )
            );

            /* Allocate a dynamic event to send back the result after attempting to recover
             * the I2C bus. */
            I2CAddrEvt *i2cAddrEvt = Q_NEW( I2CAddrEvt, I2C_BUS_CHECK_FREE_SIG );
            i2cAddrEvt->i2cBus     = me->iBus;
            i2cAddrEvt->addr       = I2C_getI2C1DevAddr(me->iDev);
            i2cAddrEvt->addrSize   = I2C_getI2C1DevAddrSize(me->iDev);
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], (QEvt *)(i2cAddrEvt), me);

            DBG_printf("ActivePosted I2C_BUS_CHECK_FREE\n");
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_GenerateStart);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::CheckingBus::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1} .....................*/
static QState I2C1DevMgr_GenerateStart1(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV5_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV5 )
            );

            /* Directly post an event to the appropriate I2CBusMgr AO */
            static QEvt const qEvt = { I2C_BUS_START_BIT_SIG, 0U, 0U };
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], &qEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_Send7BitAddrRxMode);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::GenerateStart1::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode} .................*/
static QState I2C1DevMgr_Send7BitAddrRxMode(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV6_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV6 )
            );

            /* Allocate and directly post an event to the appropriate I2CBusMgr AO */
            I2CAddrEvt *i2cAddrEvt   = Q_NEW( I2CAddrEvt, I2C_BUS_SEND_7BIT_ADDR_SIG );
            i2cAddrEvt->i2cBus       = me->iBus;
            i2cAddrEvt->addr         = I2C_getI2C1DevAddr(me->iDev);
            i2cAddrEvt->addrSize     = I2C_getI2C1DevAddrSize(me->iDev);
            i2cAddrEvt->i2cDirection = I2C_Direction_Receiver;
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], (QEvt *)i2cAddrEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CStatusEvt const *)e)->errorCode) {
                DBG_printf("Got I2C_BUS_DONE with no error\n");
                status_ = Q_TRAN(&I2C1DevMgr_ReadMem);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::Send7BitAddrRxMode::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem} ............................*/
static QState I2C1DevMgr_ReadMem(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem} */
        case Q_ENTRY_SIG: {
            /* Set error code */
            me->errorCode = ERR_I2C1DEV_EV6_TIMEOUT;

            /* Set timer */
            QTimeEvt_rearm(
                &me->i2cOpTimerEvt,
                SEC_TO_TICKS( HL_MAX_TOUT_SEC_I2C_EV6 )
            );

            /* Allocate and directly post an event to the appropriate I2CBusMgr AO */
            I2CReadMemReqEvt *i2cReadMemEvt = Q_NEW( I2CReadMemReqEvt, I2C_BUS_READ_MEM_SIG );
            i2cReadMemEvt->i2cBus           = me->iBus;
            i2cReadMemEvt->addr             = me->addrStart;
            i2cReadMemEvt->addrSize         = I2C_getI2C1MemAddrSize(me->iDev);
            i2cReadMemEvt->memAccessType    = I2C_MEM_DMA;
            i2cReadMemEvt->bytes            = me->bytesTotal; //TODO: remember to fix this.  This should not exceed max page size.
            QACTIVE_POST(AO_I2CBusMgr[me->iBus], (QEvt *)i2cReadMemEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->i2cOpTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem::I2C_BUS_DONE} */
        case I2C_BUS_DONE_SIG: {
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem::I2C_BUS_DONE::[NoErr?]} */
            if (ERR_NONE == ((I2CBusDataEvt const *)e)->errorCode) {
                LOG_printf("Got I2C_BUS_DONE with no error\n");
                char tmp[80];
                memset(tmp, 0, sizeof(tmp));
                uint16_t tmpLen = 0;
                CBErrorCode err = CON_hexToStr(
                    (const uint8_t *)((I2CBusDataEvt const *)e)->dataBuf, // data to convert
                    ((I2CBusDataEvt const *)e)->dataLen, // length of data to convert
                    tmp,                                 // where to write output
                    sizeof(tmp),                         // max size of output buffer
                    &tmpLen,                             // size of the resulting output
                    0,                                   // no columns
                    ' '                                  // separator
                );
                LOG_printf("Read %s\n", tmp);
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            /* ${AOs::I2C1DevMgr::SM::Active::Busy::ReadMem::I2C_BUS_DONE::[else]} */
            else {
                ERR_printf(
                    "Got I2C_BUS_DONE with error: 0x%08x\n",
                    ((I2CStatusEvt const *)e)->errorCode
                );
                status_ = Q_TRAN(&I2C1DevMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Busy);
            break;
        }
    }
    return status_;
}

/**
 * @brief This state indicates that the I2C bus is currently idle and the
 * incoming msg can be handled.
 * This state is the default rest state of the state machine and can handle
 * various I2C requests.  Upon entry, it also checks the deferred queue to see
 * if any request events are waiting which were posted while I2C bus was busy.
 * if there are any waiting, it will read them out, which automatically posts
 * them and the state machine will go and handle them.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::I2C1DevMgr::SM::Active::Idle} .....................................*/
static QState I2C1DevMgr_Idle(I2C1DevMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2C1DevMgr::SM::Active::Idle} */
        case Q_ENTRY_SIG: {
            /* recall the request from the private requestQueue */
            QActive_recall(
                (QActive *)me,
                &me->deferredEvtQueue
            );

            DBG_printf("back in Idle\n");
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Idle} */
        case Q_EXIT_SIG: {
            me->bytesCurr = 0; // Reset the current byte counter
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Idle::EEPROM_RAW_MEM_READ} */
        case EEPROM_RAW_MEM_READ_SIG: {
            me->iDev = EEPROM; // Set which device is being accessed
            me->addrStart = ((I2CMemReadReqEvt const *)e)->addr;
            me->addrSize  = I2C_getI2C1MemAddrSize(me->iDev);
            me->bytesTotal = ((I2CMemReadReqEvt const *)e)->bytes;
            status_ = Q_TRAN(&I2C1DevMgr_CheckingBus);
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Idle::EEPROM_SN_READ} */
        case EEPROM_SN_READ_SIG: {
            me->iDev = SN_ROM; // Set which device is being accessed
            me->addrStart = I2C_getI2C1MinMemAddr(me->iDev);
            me->addrSize  = I2C_getI2C1MemAddrSize(me->iDev);
            me->bytesTotal = I2C_getI2C1PageSize(me->iDev);
            status_ = Q_TRAN(&I2C1DevMgr_CheckingBus);
            break;
        }
        /* ${AOs::I2C1DevMgr::SM::Active::Idle::EEPROM_EUI64_READ} */
        case EEPROM_EUI64_READ_SIG: {
            me->iDev = EUI_ROM; // Set which device is being accessed
            me->addrStart = I2C_getI2C1MinMemAddr(me->iDev);
            me->addrSize  = I2C_getI2C1MemAddrSize(me->iDev);
            me->bytesTotal = I2C_getI2C1PageSize(me->iDev);
            status_ = Q_TRAN(&I2C1DevMgr_CheckingBus);
            break;
        }
        default: {
            status_ = Q_SUPER(&I2C1DevMgr_Active);
            break;
        }
    }
    return status_;
}


/**
 * @} end addtogroup groupI2C
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/