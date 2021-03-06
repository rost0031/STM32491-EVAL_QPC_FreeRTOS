/*****************************************************************************
* Model: game.qm
* File:  ./mine1.c
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
/*${.::mine1.c} ............................................................*/
#include "qp_port.h"
#include "bsp.h"
#include "game.h"

Q_DEFINE_THIS_FILE

/* encapsulated delcaration of the Mine1 HSM -------------------------------*/
/*${AOs::Mine1} ............................................................*/
typedef struct {
/* protected: */
    QMsm super;

/* private: */
    uint8_t x;
    uint8_t y;

/* public: */
    uint8_t exp_ctr;
} Mine1;

/* protected: */
static QState Mine1_initial(Mine1 * const me, QEvt const * const e);
static QState Mine1_unused  (Mine1 * const me, QEvt const * const e);
static QMState const Mine1_unused_s = {
    (QMState const *)0, /* superstate (top) */
    Q_STATE_CAST(&Mine1_unused),
    Q_ACTION_CAST(0), /* no entry action */
    Q_ACTION_CAST(0), /* no exit action */
    Q_ACTION_CAST(0)  /* no intitial tran. */
};
static QState Mine1_used  (Mine1 * const me, QEvt const * const e);
static QState Mine1_used_x(Mine1 * const me);
static QMState const Mine1_used_s = {
    (QMState const *)0, /* superstate (top) */
    Q_STATE_CAST(&Mine1_used),
    Q_ACTION_CAST(0), /* no entry action */
    Q_ACTION_CAST(&Mine1_used_x),
    Q_ACTION_CAST(0)  /* no intitial tran. */
};
static QState Mine1_exploding  (Mine1 * const me, QEvt const * const e);
static QState Mine1_exploding_e(Mine1 * const me);
static QMState const Mine1_exploding_s = {
    &Mine1_used_s, /* superstate */
    Q_STATE_CAST(&Mine1_exploding),
    Q_ACTION_CAST(&Mine1_exploding_e),
    Q_ACTION_CAST(0), /* no exit action */
    Q_ACTION_CAST(0)  /* no intitial tran. */
};
static QState Mine1_planted  (Mine1 * const me, QEvt const * const e);
static QMState const Mine1_planted_s = {
    &Mine1_used_s, /* superstate */
    Q_STATE_CAST(&Mine1_planted),
    Q_ACTION_CAST(0), /* no entry action */
    Q_ACTION_CAST(0), /* no exit action */
    Q_ACTION_CAST(0)  /* no intitial tran. */
};


/* local objects -----------------------------------------------------------*/
static Mine1 l_mine1[GAME_MINES_MAX];             /* a pool of type-1 mines */

                             /* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((uint8_t)((me_) - l_mine1))

/* Mine1 class definition --------------------------------------------------*/
/*${AOs::Mine1_ctor} .......................................................*/
QMsm * Mine1_ctor(uint8_t id) {
    Mine1 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);
    me = &l_mine1[id];
    /* superclass' ctor */
    QMsm_ctor(&me->super, Q_STATE_CAST(&Mine1_initial));
    return (QMsm *)me;
}
/*${AOs::Mine1} ............................................................*/
/*${AOs::Mine1::SM} ........................................................*/
static QState Mine1_initial(Mine1 * const me, QEvt const * const e) {
    static QMTranActTable const tatbl_ = { /* transition-action table */
        &Mine1_unused_s,
        {
            Q_ACTION_CAST(0) /* zero terminator */
        }
    };
    /* ${AOs::Mine1::SM::initial} */
    static uint8_t dict_sent;

    if (!dict_sent) {
        QS_OBJ_DICTIONARY(&l_mine1[0]); /* obj. dictionaries for Mine1 pool */
        QS_OBJ_DICTIONARY(&l_mine1[1]);
        QS_OBJ_DICTIONARY(&l_mine1[2]);
        QS_OBJ_DICTIONARY(&l_mine1[3]);
        QS_OBJ_DICTIONARY(&l_mine1[4]);

        QS_FUN_DICTIONARY(&Mine1_initial);/*fun. dictionaries for Mine1 HSM */
        QS_FUN_DICTIONARY(&Mine1_unused);
        QS_FUN_DICTIONARY(&Mine1_used);
        QS_FUN_DICTIONARY(&Mine1_planted);
        QS_FUN_DICTIONARY(&Mine1_exploding);

        dict_sent = 1U;
    }
    /* local signals */
    QS_SIG_DICTIONARY(MINE_PLANT_SIG,    me);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG, me);
    QS_SIG_DICTIONARY(MINE_RECYCLE_SIG,  me);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,      me);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,   me);

    (void)e; /* avoid the "unreferenced parameter" warning */
    return QM_TRAN_INIT(&tatbl_);
}
/*${AOs::Mine1::SM::unused} ................................................*/
/* ${AOs::Mine1::SM::unused} */
static QState Mine1_unused(Mine1 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::Mine1::SM::unused::MINE_PLANT} */
        case MINE_PLANT_SIG: {
            static QMTranActTable const tatbl_ = { /* transition-action table */
                &Mine1_planted_s,
                {
                    Q_ACTION_CAST(0) /* zero terminator */
                }
            };
            me->x = Q_EVT_CAST(ObjectPosEvt)->x;
            me->y = Q_EVT_CAST(ObjectPosEvt)->y;
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used} ..................................................*/
/* ${AOs::Mine1::SM::used} */
static QState Mine1_used_x(Mine1 * const me) {
    /* tell the Tunnel that this mine is becoming disabled */
    MineEvt *mev = Q_NEW(MineEvt, MINE_DISABLED_SIG);
    mev->id = MINE_ID(me);
    QACTIVE_POST(AO_Tunnel, (QEvt *)mev, me);
    return QM_EXIT(&Mine1_used_s);
}
/* ${AOs::Mine1::SM::used} */
static QState Mine1_used(Mine1 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::Mine1::SM::used::MINE_RECYCLE} */
        case MINE_RECYCLE_SIG: {
            static struct {
                QMState const *target;
                QActionHandler act[2];
            } const tatbl_ = { /* transition-action table */
                &Mine1_unused_s, /* target state */
                {
                    Q_ACTION_CAST(&Mine1_used_x), /* exit */
                    Q_ACTION_CAST(0) /* zero terminator */
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used::exploding} .......................................*/
/* ${AOs::Mine1::SM::used::exploding} */
static QState Mine1_exploding_e(Mine1 * const me) {
    me->exp_ctr = 0U;
    return QM_ENTRY(&Mine1_exploding_s);
}
/* ${AOs::Mine1::SM::used::exploding} */
static QState Mine1_exploding(Mine1 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::Mine1::SM::used::exploding::TIME_TICK} */
        case TIME_TICK_SIG: {
            /* ${AOs::Mine1::SM::used::exploding::TIME_TICK::[stillonscreen?]} */
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ObjectImageEvt *oie;
                ++me->exp_ctr;  /* advance the explosion counter */
                me->x -= GAME_SPEED_X; /* move explosion by 1 step */

                /* tell the Game to render the current stage of Explosion */
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = me->x + 1U;  /* x of explosion */
                oie->y   = (int8_t)((int)me->y - 4 + 2); /* y of explosion */
                oie->bmp = EXPLOSION0_BMP + (me->exp_ctr >> 2);
                QACTIVE_POST(AO_Tunnel, (QEvt *)oie, me);
                status_ = QM_HANDLED();
            }
            /* ${AOs::Mine1::SM::used::exploding::TIME_TICK::[else]} */
            else {
                static struct {
                    QMState const *target;
                    QActionHandler act[2];
                } const tatbl_ = { /* transition-action table */
                    &Mine1_unused_s, /* target state */
                    {
                        Q_ACTION_CAST(&Mine1_used_x), /* exit */
                        Q_ACTION_CAST(0) /* zero terminator */
                    }
                };
                status_ = QM_TRAN(&tatbl_);
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used::planted} .........................................*/
/* ${AOs::Mine1::SM::used::planted} */
static QState Mine1_planted(Mine1 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::Mine1::SM::used::planted::TIME_TICK} */
        case TIME_TICK_SIG: {
            /* ${AOs::Mine1::SM::used::planted::TIME_TICK::[me->x>=GAME_S~]} */
            if (me->x >= GAME_SPEED_X) {
                ObjectImageEvt *oie;
                me->x -= GAME_SPEED_X; /* move the mine 1 step */
                /* tell the Tunnel to draw the Mine */
                oie = Q_NEW(ObjectImageEvt, MINE_IMG_SIG);
                oie->x   = me->x;
                oie->y   = me->y;
                oie->bmp = MINE1_BMP;
                QACTIVE_POST(AO_Tunnel, (QEvt *)oie, me);
                status_ = QM_HANDLED();
            }
            /* ${AOs::Mine1::SM::used::planted::TIME_TICK::[else]} */
            else {
                static struct {
                    QMState const *target;
                    QActionHandler act[2];
                } const tatbl_ = { /* transition-action table */
                    &Mine1_unused_s, /* target state */
                    {
                        Q_ACTION_CAST(&Mine1_used_x), /* exit */
                        Q_ACTION_CAST(0) /* zero terminator */
                    }
                };
                status_ = QM_TRAN(&tatbl_);
            }
            break;
        }
        /* ${AOs::Mine1::SM::used::planted::SHIP_IMG} */
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            /* ${AOs::Mine1::SM::used::planted::SHIP_IMG::[collisionwith~]} */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                static struct {
                    QMState const *target;
                    QActionHandler act[2];
                } const tatbl_ = { /* transition-action table */
                    &Mine1_unused_s, /* target state */
                    {
                        Q_ACTION_CAST(&Mine1_used_x), /* exit */
                        Q_ACTION_CAST(0) /* zero terminator */
                    }
                };
                static MineEvt const mine1_hit = {
                    { HIT_MINE_SIG, 0U, 0U }, /* the QEvt base instance */
                    1U  /* type of the mine (1 for Mine type-1) */
                };
                QACTIVE_POST(AO_Ship, (QEvt *)&mine1_hit, me);
                /* go straight to 'disabled' and let the Ship do
                 * the exploding */
                status_ = QM_TRAN(&tatbl_);
            }
            else {
                status_ = QM_UNHANDLED();
            }
            break;
        }
        /* ${AOs::Mine1::SM::used::planted::MISSILE_IMG} */
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            /* ${AOs::Mine1::SM::used::planted::MISSILE_IMG::[collisionwith~]} */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                static struct {
                    QMState const *target;
                    QActionHandler act[2];
                } const tatbl_ = { /* transition-action table */
                    &Mine1_exploding_s, /* target state */
                    {
                        Q_ACTION_CAST(&Mine1_exploding_e), /* entry */
                        Q_ACTION_CAST(0) /* zero terminator */
                    }
                };
                static ScoreEvt const mine1_destroyed = {
                    { DESTROYED_MINE_SIG, 0U, 0U },  /* the QEvt base instance */
                    25U  /* score for destroying Mine type-1 */
                };
                QACTIVE_POST(AO_Missile, (QEvt *)&mine1_destroyed, me);
                status_ = QM_TRAN(&tatbl_);
            }
            else {
                status_ = QM_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

