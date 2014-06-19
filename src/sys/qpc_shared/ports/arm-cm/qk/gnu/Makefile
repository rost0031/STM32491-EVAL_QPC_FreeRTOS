##############################################################################
# Product:  QP/C, POSIX port, GNU compiler
# Last Updated for Version: 4.5.02
# Date of the Last Update:  Jul 21, 2012
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################

# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
# make CONF=spy clean

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP
#------------------------------------------------------------------------------
ARM_CORE 					= cortex-m3
CROSS    					= arm-none-eabi-
CC      					= $(CROSS)gcc
CPP      					= $(CROSS)g++
AS       					= $(CROSS)as
AR       					= $(CROSS)ar
OBJCPY   					= ${CROSS}objcopy
RM       					= rm -rf
ECHO    					= echo
MKDIR    					= mkdir

#-----------------------------------------------------------------------------
# NOTE: the Makefile expects that the QPC environment variable is defined
# and points to the QP/C installation directory
#
QPC        					= ../../../..
QP_INCDIR  					= $(QPC)/include
QEP_SRCDIR 					= $(QPC)/qep/source
QF_SRCDIR  					= $(QPC)/qf/source
QS_SRCDIR  					= $(QPC)/qs/source
QK_SRCDIR  					= $(QPC)/qk/source
QP_PRTDIR  					= .
LIBFLAGS   					= rs
QS_OBJS    					=

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR)

#-----------------------------------------------------------------------------
# build options for various configurations
#
ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR  					= rel

# gcc options
# -c    : compile only
# -O    : optimize
CCFLAGS 					= -c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os -DNDEBUG -o$@
ASMFLAGS 					= -c -mcpu=$(ARM_CORE) -mthumb
else ifeq (spy, $(CONF))  # Spy configuration ................................
BINDIR  = spy

# gcc options
# -c    : compile only
# -g    : source level debugging
# -D    : define preprocessor macro
CCFLAGS 					= -g -c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os -DQ_SPY -o$@
QS_CSRC   					= 	qs.c \
					          	qs_.c \
					          	qs_blk.c \
					          	qs_byte.c \
					          	qs_dict.c \
					          	qs_f32.c \
					          	qs_f64.c \
					          	qs_mem.c \
					          	qs_str.c
					          	
QS_OBJS 					= $(patsubst %.c, $(BINDIR)/%.o, $(QS_CSRC))

ASMFLAGS 					= -g -c -mcpu=$(ARM_CORE) -mthumb
else                     # default Debug configuration .......................

BINDIR  					= dbg
QP_DEP  					= $(QP_INCDIR)\qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS 					= -g -c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os
ASMFLAGS 					= -g -c -mcpu=$(ARM_CORE) -mthumb
endif

VPATH = $(QEP_SRCDIR) $(QF_SRCDIR) $(QS_SRCDIR) $(QK_SRCDIR) $(QP_PRTDIR)/src

#-----------------------------------------------------------------------------
# QEP src and objects 
#
QEP_CSRC 					=  qep.c \
					           qmsm_ini.c \
					           qmsm_dis.c \
					           qmsm_in.c \
					           qfsm_ini.c \
					           qfsm_dis.c \
					           qhsm_ini.c \
					           qhsm_dis.c \
					           qhsm_top.c \
					           qhsm_in.c
					           
QEP_OBJS 					=  $(patsubst %.c, $(BINDIR)/%.o, $(QEP_CSRC))

#-----------------------------------------------------------------------------
# QF src and objects 
#			
QF_CSRC  					=  qa_ctor.c \
							   qa_defer.c \
							   qa_fifo.c \
							   qa_lifo.c \
							   qa_get_.c \
							   qa_sub.c  \
							   qa_usub.c \
							   qa_usuba.c \
							   qeq_fifo.c \
							   qeq_get.c \
							   qeq_init.c \
							   qeq_lifo.c \
							   qf_act.c \
							   qf_gc.c \
							   qf_log2.c \
							   qf_new.c \
							   qf_pool.c \
							   qf_psini.c \
							   qf_pspub.c \
							   qf_pwr2.c \
							   qf_tick.c \
							   qma_ctor.c \
							   qmp_get.c \
							   qmp_init.c \
							   qmp_put.c \
							   qte_ctor.c \
							   qte_arm.c \
							   qte_darm.c \
							   qte_rarm.c \
							   qte_ctr.c 

QF_OBJS 					=  $(patsubst %.c, $(BINDIR)/%.o, $(QF_CSRC))

#-----------------------------------------------------------------------------
# QK src and objects 
#
QK_ASRC   					= qk_port.s

QK_CSRC   					= qk_sched.c \
            				  qk_mutex.c \
            				  qk.c
            				              				  
QK_OBJS 					= $(patsubst %.c, $(BINDIR)/%.o, $(QK_CSRC)) 
QK_OBJS					   += $(patsubst %.s, $(BINDIR)/%.o, $(QK_ASRC))

#-----------------------------------------------------------------------------
# All src and objects 
#
ALL_OBJ = $(QEP_OBSJ) $(QF_OBJS) $(QK_OBJS) $(QS_OBJS)
ALL_SRC = $(QEP_CSRC) $(QF_CSRC) $(QK_CSRC) $(QS_CSRC) $(QK_ASRC)

.default: all

#-----------------------------------------------------------------------------
# Build Targets
#
all: $(BINDIR) $(BINDIR)/libqp_$(ARM_CORE)_cs.a

$(BINDIR)/libqp_$(ARM_CORE)_cs.a: 	$(BINDIR) $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS) $(QK_OBJS)
	-$(RM) $(BINDIR)/libqp.a
	$(AR) $(LIBFLAGS) $(BINDIR)/libqp_$(ARM_CORE)_cs.a $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS) $(QK_OBJS)

$(BINDIR)/%.o: %.c
	@echo Compiling C object
	$(CC) $(CCFLAGS) $(CCINC) $(INCLUDES) -c $< -o $@
	
$(BINDIR)/%.o: %.s
	@echo Compiling ASM object
	$(AS) $(ASMFLAGS) $< -o $@

$(BINDIR):
	@echo Creating output directory
	mkdir $@

# clean ======================================================================
.PHONY : clean
clean:
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(BINDIR)/*.a
	-$(RM) $(BINDIR)

