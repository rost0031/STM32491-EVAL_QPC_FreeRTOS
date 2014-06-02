##############################################################################
# Product:  QP/C, Win32 build, GNU compiler
# Last Updated for Version: 4.5.02
# Date of the Last Update:  Jul 29, 2012
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
# mingw32-make
# mingw32-make CONF=rel
# mingw32-make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# mingw32-make clean
# mingw32-make CONF=rel clean
# mingw32-make CONF=spy clean

#-----------------------------------------------------------------------------
# general utilities
#
RM                      = rm -rf
ECHO                    = echo
MKDIR                   = mkdir -p
TARGET_PLATFORM         = WIN32

#-----------------------------------------------------------------------------
# NOTE: this Makefile assumes that the MinGW\bin directory is added
# to the PATH variable.
#
# This will detect whether the linux system is 64 or 32 bit since the mingw 
# compiler's name is dependent on it
LBITS                  := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   CROSS   = x86_64-w64-mingw32-
else
   CROSS   = i686-w64-mingw32-
endif

CC         = $(CROSS)gcc
AS         = $(CROSS)as
OBJCPY     = $(CROSS)objcopy
RANLIB     = $(CROSS)ranlib

QPC        = ../../../..
QP_INCDIR  = $(QPC)/include
QEP_SRCDIR = $(QPC)/qep/source
QF_SRCDIR  = $(QPC)/qf/source
QS_SRCDIR  = $(QPC)/qs/source
QP_PRTDIR  = .
LIBFLAGS   = rs
QS_OBJS    =

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(QF_SRCDIR)

#-----------------------------------------------------------------------------
# build options for various configurations
#
ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR  = rel

# gcc options
# -c    : compile only
# -O    : optimize
CCFLAGS = -c -O2 -Wall -o$@

else ifeq (spy, $(CONF))  # Spy configuration ................................

BINDIR   = spy

# gcc options
# -c    : compile only
# -g    : source level debugging
# -D    : define preprocessor macro
CCFLAGS = -c -g -O -Wall -DQ_SPY -o$@
QS_OBJS = \
	$(BINDIR)/qs.o \
	$(BINDIR)/qs_.o \
	$(BINDIR)/qs_blk.o \
	$(BINDIR)/qs_byte.o \
	$(BINDIR)/qs_f32.o \
	$(BINDIR)/qs_f64.o \
	$(BINDIR)/qs_mem.o \
	$(BINDIR)/qs_str.o \
	$(BINDIR)/qs_u64.o

else                     # default Debug configuration .......................

BINDIR  = dbg
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS = -c -g -O -Wall -o$@

endif

VPATH = $(QEP_SRCDIR) $(QF_SRCDIR) $(QS_SRCDIR) .
#-----------------------------------------------------------------------------
# QEP src and objects 
#
QEP_CSRC 					=  qep.c \
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
QF_CSRC  					=  qa_defer.c \
					           qa_fifo.c \
					           qa_lifo.c \
					           qa_get_.c \
					           qa_sub.c \
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
					           qmp_get.c \
					           qmp_init.c \
					           qmp_put.c \
					           qte_ctor.c \
					           qte_arm.c \
					           qte_darm.c \
					           qte_rarm.c \
					           qte_ctr.c \
					           qf_port.c\
					           win32_gui.c
					           
QF_OBJS 					=  $(patsubst %.c, $(BINDIR)/%.o, $(QF_CSRC))


#-----------------------------------------------------------------------------
# Build Targets
#
.default: all
all: $(BINDIR) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a

$(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a: 	$(BINDIR) $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS)
	-$(RM) $(BINDIR)/libqp.a
	$(AR) $(LIBFLAGS) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a $(QEP_OBJS) $(QF_OBJS) $(QS_OBJS)
	$(RANLIB) $(BINDIR)/libqp_$(TARGET_PLATFORM)_cs.a

$(BINDIR)/%.o: %.c
	@echo Compiling C object
	$(CC) $(CCFLAGS) $(CCINC) $(INCLUDES) -c $< -o $@
	
$(BINDIR)/%.o: %.S
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
