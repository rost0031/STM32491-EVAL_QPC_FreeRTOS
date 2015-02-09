/*****************************************************************************
* Product: QF/C port to FreeRTOS.org (version 8)
* Last updated for version 5.3.1
* Last updated on  2014-09-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#ifndef qf_port_h
#define qf_port_h

/* FreeRTOS event queue and thread types */
#define QF_EQUEUE_TYPE        QEQueue
#define QF_THREAD_TYPE        TaskHandle_t
#define QF_OS_OBJECT_TYPE     TaskHandle_t

/* The maximum number of active objects in the application, see NOTE1 */
#define QF_MAX_ACTIVE         32

/* QF critical section for FreeRTOS/ARM-Cortex-M, see NOTE2 */
#define QF_CRIT_STAT_TYPE     uint32_t
#define QF_CRIT_ENTRY(stat_)  ((stat_) = portSET_INTERRUPT_MASK_FROM_ISR())
#define QF_CRIT_EXIT(stat_)   portCLEAR_INTERRUPT_MASK_FROM_ISR(stat_)

#define QF_INT_DISABLE()      portDISABLE_INTERRUPTS()
#define QF_INT_ENABLE()       portENABLE_INTERRUPTS()

#include "FreeRTOS.h"  /* FreeRTOS master include file */
#include "task.h"      /* FreeRTOS task  management */

#include "qep_port.h"  /* QEP port */
#include "qequeue.h"   /* this QP port uses the native QF event queue */
#include "qmpool.h"    /* this QP port uses the native QF memory pool */
#include "qf.h"        /* QF platform-independent public interface */


/* FreeRTOS "extras" for handling ISRs for FreeRTOS/ARM-Cortex-M */
typedef struct {
    BaseType_t volatile isrNest;
    BaseType_t volatile ctxtReq;
} FreeRTOSExtras;

extern FreeRTOSExtras FreeRTOS_extras;

#define QF_ISR_ENTRY(stat_) do { \
   (stat_) = portSET_INTERRUPT_MASK_FROM_ISR(); \
   if ((FreeRTOS_extras.isrNest++) == (BaseType_t)0) { \
       FreeRTOS_extras.ctxtReq = pdFALSE; \
   } \
   portCLEAR_INTERRUPT_MASK_FROM_ISR(stat_); \
} while (0)

#define QF_ISR_EXIT(stat_, ctxtReq_) do { \
   (stat_) = portSET_INTERRUPT_MASK_FROM_ISR(); \
   if (FreeRTOS_extras.ctxtReq != pdFALSE) { \
       (ctxtReq_) = pdTRUE; \
   } \
   --FreeRTOS_extras.isrNest; \
   portCLEAR_INTERRUPT_MASK_FROM_ISR(stat_); \
} while (0)

/* FreeRTOS hooks prototypes (not provided by FreeRTOS) */
#if (configUSE_IDLE_HOOK > 0)
    void vApplicationIdleHook(void);
#endif
#if (configUSE_TICK_HOOK > 0)
    void vApplicationTickHook(void);
#endif
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
    void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
#endif

/*3****************************************************************************
* interface used only inside QF, but not in applications
*/
#ifdef QP_IMPL
    /* FreeRTOS blocking for event queue implementation */
    #define QACTIVE_EQUEUE_WAIT_(me_) \
        while ((me_)->eQueue.frontEvt == (QEvt *)0) { \
            QF_CRIT_EXIT_(); \
            vTaskSuspend((TaskHandle_t)0); \
            QF_CRIT_ENTRY_(); \
        }

    #define QACTIVE_EQUEUE_SIGNAL_(me_) do { \
        if (FreeRTOS_extras.isrNest == (BaseType_t)0) { \
            QF_CRIT_EXIT_(); \
            vTaskResume((me_)->osObject); \
            QF_CRIT_ENTRY_(); \
        } \
        else { \
            BaseType_t ctxtReq; \
            QF_CRIT_EXIT_(); \
            ctxtReq = xTaskResumeFromISR((me_)->osObject); \
            QF_CRIT_ENTRY_(); \
            if (ctxtReq != pdFALSE) { \
                FreeRTOS_extras.ctxtReq = pdTRUE; \
            } \
        } \
    } while (0)

    #define QACTIVE_EQUEUE_ONEMPTY_(me_) ((void)0)

    /* native QF event pool operations */
    #define QF_EPOOL_TYPE_            QMPool
    #define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
        (QMPool_init(&(p_), (poolSto_), (poolSize_), (evtSize_)))
    #define QF_EPOOL_EVENT_SIZE_(p_)  ((uint_fast16_t)(p_).blockSize)
    #define QF_EPOOL_GET_(p_, e_, m_) ((e_) = (QEvt *)QMPool_get(&(p_), (m_)))
    #define QF_EPOOL_PUT_(p_, e_)     (QMPool_put(&(p_), (e_)))

#endif /* ifdef QP_IMPL */

/*****************************************************************************
* NOTE1:
* The maximum number of active objects in QP can be increased to 63,
* inclusive, but it can be reduced to save some memory. Also, the number of
* active objects cannot exceed the number of FreeRTOS task priorities,
* because each QP active object requires a unique priority level.
*
* NOTE2:
* The FreeRTOS critical section defined here is correct only for ARM Cortex-M
* which define the portSET_INTERRUPT_MASK_FROM_ISR() /
* portCLEAR_INTERRUPT_MASK_FROM_ISR() pair of macros, and where the interrupt
* disabling policy from the ISRs is actually the same as from the tasks.
* This critical section might NOT work for other CPUs (!)
*
* NOTE3:
* The "FreeRTOS.h" header file includes the "FreeRTOSConfig.h" header file
* which creates an artificial dependency on the FreeRTOS configuration. This
* port uses a dummy "FreeRTOSConfig.h" from the "config" sub-directory, so that
* applications can still use their own (and potentially different) FreeRTOS
* configuration at compile time.
*/

#endif /* qf_port_h */
