##############################################################################
# Product: Makefile for LWIP
# Date of the Last Update:  Jul 16, 2012
#
#                             Datacard 
#                    ---------------------------
#
# Copyright (C) 2012 Datacard. All rights reserved.
#
##############################################################################
# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

LWIP_DIR 	=.
# The path to the project that is using LWIP must be passed in.  It's the only
# way for LWIP to be compiled as a library.
ifndef LWIP_PORT_DIR
$(error Please call this Makefile with the "make LWIP_PORT_DIR=relateive_location_of_LWIP_PORT_for_your_target_BSP)
endif

ifndef PROJ_BSP
$(error Please call the LWIP Makefile with the "make PROJ_BSP=relative_location_of_bsp".  The directory should be where ever you have lwip config files.)
endif

ifndef LWIP_SRC_DIR 
$(error Please call the LWIP Makefile with the "make LWIP_SRC_DIR=relative_location_of _src_dir".  The directory should be where ever you have lwip config files.)
endif


#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP
#------------------------------------------------------------------------------
ARM_CORE 				= cortex-m3
CROSS    				= arm-none-eabi-
CC       				= $(CROSS)gcc
CPP      				= $(CROSS)g++
AS       				= $(CROSS)as
AR       				= $(CROSS)ar
OBJCPY   				= ${CROSS}objcopy
RM       				= rm -rf
ECHO     				= echo
MKDIR    				= mkdir

#-----------------------------------------------------------------------------
# build options for various configurations
# TODO: fix this section since the CONF flag is not used but there is 
# debugging that can be enabled/disabled in LWIP

ARM_CORE 				= cortex-m3

ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR   				=$(LWIP_DIR)/rel
CFLAGS  				=-c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os -DNDEBUG
ASMFLAGS 				=-c -mcpu=$(ARM_CORE)

else ifeq (spy, $(CONF))  # Spy configuration ................................

BINDIR   				=$(LWIP_DIR)/spy
CFLAGS  				=-g -c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os -DQ_SPY
ASMFLAGS 				=-g -c -mcpu=$(ARM_CORE)

else                     # default Debug configuration .......................

BINDIR   				= $(LWIP_DIR)/dbg
CFLAGS  				= -g -c -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections -Os
ASMFLAGS 				= -g -c -mcpu=$(ARM_CORE)

endif

#-----------------------------------------------------------------------------
# Common files, directories, and flags/options
#

LIBDIR   				= $(BINDIR)

LIBFLAGS 				= rs

#-----------------------------------------------------------------------------
# LWIP Specific
#
LWIP_SRC  				= src
LWIP_INC  				= 	-I$(PROJ_BSP) \
							-I$(LWIP_SRC_DIR) \
							-I$(LWIP_SRC)/include \
							-I$(LWIP_SRC)/include/ipv4 \
							-I$(LWIP_PORT_DIR) \
							-I$(LWIP_PORT_DIR)/arch \
							-I$(LWIP_PORT_DIR)/netif
LWIP_ASRC 				=
LWIP_CSRC 				= 	$(LWIP_SRC)/core/def.c \
							$(LWIP_SRC)/core/mem.c \
							$(LWIP_SRC)/core/memp.c \
							$(LWIP_SRC)/core/netif.c \
							$(LWIP_SRC)/core/pbuf.c \
							$(LWIP_SRC)/core/raw.c \
							$(LWIP_SRC)/core/stats.c \
							$(LWIP_SRC)/core/timers.c \
							$(LWIP_SRC)/core/sys.c \
							$(LWIP_SRC)/core/tcp.c \
							$(LWIP_SRC)/core/tcp_in.c \
							$(LWIP_SRC)/core/tcp_out.c \
							$(LWIP_SRC)/core/udp.c \
							$(LWIP_SRC)/core/dhcp.c \
							$(LWIP_SRC)/core/dns.c \
							$(LWIP_SRC)/core/init.c \
							\
							$(LWIP_SRC)/core/ipv4/ip.c \
							$(LWIP_SRC)/core/ipv4/icmp.c \
							$(LWIP_SRC)/core/ipv4/inet.c \
							$(LWIP_SRC)/core/ipv4/ip_addr.c \
							$(LWIP_SRC)/core/ipv4/ip_frag.c \
							$(LWIP_SRC)/core/ipv4/inet_chksum.c \
							\
							$(LWIP_SRC)/api/err.c \
							\
							$(LWIP_SRC)/netif/etharp.c 
           

LWIP_AOBJS  				= $(patsubst %.s,%.o,$(LWIP_ASRC))
LWIP_COBJS  				= $(patsubst %.c,%.o,$(LWIP_CSRC))
LWIP_C_DEP  				= $(patsubst %.o, %.d, $(LWIP_COBJS))


#-----------------------------------------------------------------------------
# Common targets 
#


all: $(LIBDIR)/liblwip_$(ARM_CORE)_cs.a

#-----------------------------------------------------------------------------
# LWIP Specific targets
#
$(LIBDIR)/liblwip_$(ARM_CORE)_cs.a: $(LWIP_AOBJS) $(LWIP_COBJS) $(LIBDIR)
	@echo "--- Compiling libLWIP library"
	$(TRACE_FLAG)$(AR) $(LIBFLAGS) $(LIBDIR)/liblwip_$(ARM_CORE)_cs.a $(LWIP_AOBJS) $(LWIP_COBJS)

# $(LWIP_AOBJS): $(LWIP_ASRC)
# 	@echo "--- Compiling $(<F)"
# 	$(AS) $(ASMFLAGS) $< -o $@

$(LWIP_SRC)/%.o: $(LWIP_SRC)/%.c
	@echo "--- Compiling $(<F)"
	$(TRACE_FLAG)$(CC) $(CFLAGS) $(LWIP_INC) -c $< -o $@

$(LIBDIR):
	@echo
	$(TRACE_FLAG)mkdir -p $@

.PHONY : clean

clean: clean_lwip
	-$(RM) $(LIBDIR)/*.o \
	$(LIBDIR)/*.d \
	$(LIBDIR)/*.hex \
	$(LIBDIR)/*.elf \
	$(LIBDIR)/*.map \
	$(LIBDIR)/*.a \
	$(LIBDIR)/*.so

clean_lwip:
	-$(RM) $(LWIP_AOBJS) $(LWIP_COBJS) 

show:
	@echo CONF = $(CONF)
	@echo LWIP_ASRC = $(LWIP_ASRC)
	@echo LWIP_CSRC = $(LWIP_CSRC)
	@echo ROOTDIR = $(ROOTDIR)
	
