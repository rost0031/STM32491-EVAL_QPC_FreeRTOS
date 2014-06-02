/*****************************************************************************
* Product: QF/C
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 17, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qf_pkg.h"
#include "qassert.h"
#include <stdio.h>
Q_DEFINE_THIS_MODULE("qte_arm")

/**
* \file
* \ingroup qf
* \brief QF_timeEvtListHead_ definition;
* QTimeEvt_arm_() and QF_noTimeEvtsActive implementations.
*/

/* Package-scope objects ---------------------------------------------------*/
QTimeEvt *QF_timeEvtListHead_;        /* head of linked list of time events */

/*..........................................................................*/
void QTimeEvt_arm_(QTimeEvt * const me, QActive * const act,
                   QTimeEvtCtr const nTicks, char* filex, int line)
{
    QF_CRIT_STAT_

    if ((nTicks == (QTimeEvtCtr)0) /* cannot arm a timer with 0 ticks */
              || (act == (QActive *)0)    /* active object must be provided */
              || (me->ctr != (QTimeEvtCtr)0)            /* must be disarmed */
              || (me->super.sig < (QSignal)Q_USER_SIG))
    {
       printf("\n\n From %s, %d\n", filex, line);
       printf("Pre assert nTicks=%hu, act=%lu, me->ctr=%hu, me->super.sig=%hu\n",
             (uint16_t)nTicks, (uint32_t)(act), (uint16_t)(me->ctr), me->super.sig);
    }
    Q_REQUIRE((nTicks != (QTimeEvtCtr)0) /* cannot arm a timer with 0 ticks */
              && (act != (QActive *)0)    /* active object must be provided */
              && (me->ctr == (QTimeEvtCtr)0)            /* must be disarmed */
              && (me->super.sig >= (QSignal)Q_USER_SIG));   /* valid signal */

    QF_CRIT_ENTRY_();
    me->ctr = nTicks;
    me->act = act;
    if (QF_EVT_REF_CTR_(&me->super) == (uint8_t)0) {         /* not linked? */
        me->next = QF_timeEvtListHead_;
        QF_timeEvtListHead_ = me;
        QF_EVT_REF_CTR_INC_(&me->super);                  /* mark as linked */
    }

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_ARM, QS_teObj_, me)
        QS_TIME_();                                            /* timestamp */
        QS_OBJ_(me);                              /* this time event object */
        QS_OBJ_(act);                                  /* the active object */
        QS_TEC_(nTicks);                             /* the number of ticks */
        QS_TEC_(me->interval);                              /* the interval */
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}
/*..........................................................................*/
uint8_t QF_noTimeEvtsActive(void) { /* must be invoked from a critical sect.*/
    return (QF_timeEvtListHead_ == (QTimeEvt *)0) ? (uint8_t)1 : (uint8_t)0;
}
