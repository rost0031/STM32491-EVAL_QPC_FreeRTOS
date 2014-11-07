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

# Leave this blank here.  This gets appended with needed defines as the options
# are parsed 
DEFINES                     =
 
#------------------------------------------------------------------------------
#  MCU SETUP - these defaults can be overridden by passing in MCU=
#------------------------------------------------------------------------------
ARM_CORE 					= cortex-m3
MFLAGS                      = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
AFLAGS                      = -mcpu=$(ARM_CORE)

ifeq (cortex-m0, $(MCU))
	ARM_CORE                = cortex-m0
	MFLAGS                  = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
	AFLAGS                  = -mcpu=$(ARM_CORE)
else ifeq (cortex-m0plus, $(MCU))
	ARM_CORE                = cortex-m0plus
	MFLAGS                  = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
	AFLAGS                  = -mcpu=$(ARM_CORE)
else ifeq (cortex-m3, $(MCU))
	ARM_CORE                = cortex-m3
	MFLAGS                  = -mcpu=$(ARM_CORE) -mthumb -mfloat-abi=soft
	AFLAGS                  = -mcpu=$(ARM_CORE)
else ifeq (cortex-m4-vfp, $(MCU))
	ARM_CORE                = cortex-m4
	ARM_FPU                 = fpv4-sp-d16
	MFLAGS                  = -mcpu=$(ARM_CORE) -mfpu=$(ARM_FPU) -mfloat-abi=softfp \
							  -mlittle-endian -mthumb
	AFLAGS                  = -mcpu=$(ARM_CORE) -defsym=FPU_VFP_V4_SP_D16=1
    DEFINES                += -D__VFP_FP__
else ifeq (cortex-m4-fpv4-sp-d16, $(MCU))
	ARM_CORE                = cortex-m4
	ARM_FPU                 = fpv4-sp-d16
	MFLAGS                  = -mcpu=$(ARM_CORE) -mfpu=$(ARM_FPU) -mfloat-abi=hard \
	                          -mlittle-endian -mthumb
	AFLAGS                  = -mcpu=$(ARM_CORE) -defsym=FPU_VFP_V4_SP_D16=1
    DEFINES                += -D__VFP_FP__
else
	$(error Must specify the MCU, like MCU=cortex-m0 )
endif


#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP
#------------------------------------------------------------------------------
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

ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR   				= $(LWIP_DIR)/rel
CFLAGS  				= -c $(MFLAGS) -std=gnu99 -mthumb -Wall \
                          -ffunction-sections -fdata-sections -Os -DNDEBUG $(DEFINES)
ASMFLAGS 				= -c $(AFLAGS)

else ifeq (spy, $(CONF))  # Spy configuration ................................

BINDIR   				= $(LWIP_DIR)/spy
CFLAGS  				= -g -c $(MFLAGS) -std=gnu99 -mthumb -Wall \
                          -ffunction-sections -fdata-sections -Os -DQ_SPY $(DEFINES)
ASMFLAGS 				= -g -c $(AFLAGS)

else                     # default Debug configuration .......................

BINDIR   				= $(LWIP_DIR)/dbg
CFLAGS  				= -g -c $(MFLAGS) -std=gnu99 -mthumb -Wall $(DEFINES) \
                          -ffunction-sections -fdata-sections -Os
ASMFLAGS 				= -g -c $(AFLAGS)

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
	
