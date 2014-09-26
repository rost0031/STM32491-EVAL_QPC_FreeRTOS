/**
 * @file no_heap.c
 *
 *
 * @brief  Dummy heap management to reduce the codesize
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2012 Datacard. All rights reserved.
 */
/*****************************************************************************
* Product: Dummy heap management to reduce the codesize
* Date of the Last Update:  Apr 07, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include <stdlib.h>                  /* for prototypes of malloc() and free() */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE;                 /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*..........................................................................*/
void *malloc(size_t size) {
    Q_ERROR();
    (void)size;
    return ((void *)0);
}
/*..........................................................................*/
void free(void *ptr) {
    Q_ERROR();
    (void)ptr;
}
/*..........................................................................*/
void *calloc(size_t nelements, size_t elementSize) {
    Q_ERROR();
    (void)nelements;
    (void)elementSize;
    return ((void *)0);
}
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
