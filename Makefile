##############################################################################
# Product: Makefile for Project
# Date of the Last Update:  Jul 16, 2012
#
#                             Datacard 
#                    ---------------------------
#
# Copyright (C) 2012 Datacard. All rights reserved.
#
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
#
# env.mk contains an optional CONF define (as above) and IP define
# if IP=slave the default IP address built into the code will be 169.254.2.3
# if not, then the user's printer IP will be built in.
ifeq ($(IP),)
  -include env.mk
endif

# config.mk also must contains the version number components
# config.mk and env.mk must be separate since env.mk is NOT version controlled: config.mk is
-include config.mk

# If DCC_IP is defined, but not the MAC, complain
ifneq ($(IP),)
  ifeq ($(MAC),)
    $(error Please include MAC with IP! );
  endif
endif

ifeq ($(IP),)       # Slave IP address............................
IPADDR0 = 169
IPADDR1 = 254
IPADDR2 = 2
IPADDR3 = 3
MAC = 0x3b
else
IPADDR0 = 192
IPADDR1 = 168
IPADDR2 = 1
IPADDR3 = $(IP)
endif

# If TRACE=0 -->TRACE_FLAG=
# If TRACE=1 -->TRACE_FLAG=@
# If TRACE=something -->TRACE_FLAG=something
TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

# Output file basename (should be the same as the directory name)
PROJECT_NAME			= L1BootLdr

#------------------------------------------------------------------------------
#  TOOLCHAIN SETUP
#------------------------------------------------------------------------------
CROSS    				= arm-none-eabi-
CC       				= $(CROSS)gcc
CPP      				= $(CROSS)g++
AS       				= $(CROSS)as
LINK     				= $(CROSS)gcc
OBJCPY   				= ${CROSS}objcopy
RM       				= rm -rf
ECHO     				= echo
MKDIR    				= mkdir

#-----------------------------------------------------------------------------
# Directories
#
#-----------------------------------------------------------------------------
SRC_DIR					= ./src
APP_DIR     			= $(SRC_DIR)/app
BSP_DIR					= $(SRC_DIR)/bsp
SYS_DIR					= $(SRC_DIR)/sys

# Ethernet Driver
ETH_DRV_DIR				= $(BSP_DIR)/bsp_shared/KSZ8863_Eth_Driver
QP_LWIP_PORT_DIR		= $(SYS_DIR)/sys_shared/qpc_lwip_stm32f2xx_port

# QPC directories
QPC 					= $(SYS_DIR)/qpc_shared
QP_PORT_DIR 			= $(QPC)/ports/arm-cortex/qk/gnu

# STM32 Drivers
STM32F2XX_STD_PERIPH	= $(BSP_DIR)/bsp_shared/STM32F2xx_StdPeriph_Driver_shared

# LWIP
LWIP_DIR				= $(SYS_DIR)/lwip_shared

# Base64 encoding module
BASE64_DIR				= $(SYS_DIR)/libb64_shared

# Directories that need to be passed down to LWIP
LWIP_PORT_FOR_LWIP		= ../sys_shared/qpc_lwip_stm32f2xx_port
BSP_DIR_FOR_LWIP		= ../sys_shared/runtime
LWIP_SRC_DIR			= ../../

# Common state machines directory
COMM_SM					= $(APP_DIR)/StateMachines

# Source virtual directories
VPATH 					= $(APP_DIR) \
						  $(COMM_SM) \
						  \
						  $(BSP_DIR) \
						  $(BSP_DIR)/bsp_shared \
						  $(ETH_DRV_DIR)/src \
						  \
						  $(QP_LWIP_PORT_DIR) \
						  $(QP_LWIP_PORT_DIR)/netif \
						  \
						  $(BASE64_DIR) \
						  $(SYS_DIR)/sys_shared/runtime \
						  \
						  $(STM32F2XX_STD_PERIPH)/src

# include directories
INCLUDES  				= -I$(SRC_DIR) \
						  -I$(APP_DIR) \
						  \
						  -I$(COMM_SM) \
						  \
						  -I$(QPC)/include \
						  -I$(QPC)/qf/source \
						  -I$(QP_PORT_DIR) \
						  \
						  -I$(BSP_DIR)  \
						  -I$(BSP_DIR)/bsp_shared \
						  -I$(ETH_DRV_DIR)/inc \
						  -I$(BASE64_DIR) \
						  -I$(SYS_DIR)/sys_shared/runtime \
						  \
						  -I$(LWIP_DIR)/src/include \
						  -I$(LWIP_DIR)/src/include/netif \
						  -I$(LWIP_DIR)/src/include/lwip \
						  -I$(LWIP_DIR)/src/include/ipv4 \
						  \
						  -I$(BSP_DIR)/bsp_shared/CMSIS_shared/Include \
						  -I$(BSP_DIR)/bsp_shared/CMSIS_shared/Device/ST/STM32F2xx/Include \
						  \
						  -I$(QP_LWIP_PORT_DIR) \
						  -I$(QP_LWIP_PORT_DIR)/arch \
						  -I$(QP_LWIP_PORT_DIR)/netif \
						  \
						  -I$(STM32F2XX_STD_PERIPH)/inc

#-----------------------------------------------------------------------------
# defines
# HSE_VALUE=8000000 overrides the External Clock setting for the Redwood L1
# board since the development kit runs at a 25MHz external clock
#-----------------------------------------------------------------------------
DEFINES   				= -DUSE_STM322xG_EVAL \
						  -DUSE_STDPERIPH_DRIVER \
						  -DHSE_VALUE=8000000 \
						  -DLWIP_TCP=1 \
						  -DFLASH_BASE=0x08000000
						  
#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS 				:= \
						startup_stm32f2xx.S \
						memcpy.S \
						stm32_hardfault_handler.S

# C source files
C_SRCS 					:= \
			      		main.c \
			      		CommStackMgr.c \
			      		SerialMgr.c \
			      		stm32f2xx_it.c \
			      		system_stm32f2xx.c \
			      		no_heap.c \
			      		\
			      		LWIPMgr.c \
			      		\
			      		stm32f2x7_eth_bsp.c \
			      		stm32f2x7_eth.c \
						\
			      		bsp.c \
			      		time.c \
			      		lwip.c \
			      		serial.c \
			      		console_output.c \
			      		crc32compat.c \
						stm32f2xx_it.c \
			      		system_stm32f2xx.c \
			      		\
			      		cencode.c \
			      		cdecode.c \
			      		base64_wrapper.c \
			      		\
			      		eth_driver.c \
			      		\
			      		syscalls.c \
						\
			      		misc.c  \
			      		stm32f2xx_crc.c \
			      		stm32f2xx_dma.c \
			      		stm32f2xx_exti.c \
			      		stm32f2xx_flash.c \
						stm32f2xx_gpio.c \
			      		stm32f2xx_pwr.c \
			      		stm32f2xx_rcc.c \
			      		stm32f2xx_rtc.c \
			      		stm32f2xx_syscfg.c \
			      		stm32f2xx_tim.c \
			      		stm32f2xx_usart.c
			      		
			      		
# Temporarily not compiled files.  These will need to be added back in before
# everything is done but for now, leave out. HR			      		
#			      		flash_if.c \
			      		
# Add these back in once that hardware is actually needed. HR.			      		
#			      		stm32f2xx_adc.c  \
#			      		stm32f2xx_can.c \
#			      		stm32f2xx_cryp.c \
#			      		stm32f2xx_cryp_aes.c \
#			      		stm32f2xx_cryp_des.c \
#			      		stm32f2xx_cryp_tdes.c \
#			      		stm32f2xx_dac.c \
#			      		stm32f2xx_dbgmcu.c \
#			      		stm32f2xx_fsmc.c \
#			      		stm32f2xx_hash.c \
#			      		stm32f2xx_hash_md5.c \
#			      		stm32f2xx_hash_sha1.c \
#			      		stm32f2xx_i2c.c \
#			      		stm32f2xx_iwdg.c \
#			      		stm32f2xx_rng.c \
#			      		stm32f2xx_sdio.c \
#			      		stm32f2xx_spi.c \
#			      		stm32f2xx_wwdg.c
						

# C++ source files
CPP_SRCS 				:=	$(wildcard *.cpp)


LD_SCRIPT 				:= $(BSP_DIR)/linkerscript/stm32f207flash.ld
LDFLAGS  				:= -T$(LDSCRIPT)

#-----------------------------------------------------------------------------
# build options for various configurations
#

ARM_CORE 	= cortex-m3


ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR 	:= rel
DEFINES		+= -DNDEBUG
LIBS    	:= -lqp_$(ARM_CORE)_cs -llwip_$(ARM_CORE)_cs
ASFLAGS 	= -mthumb -mcpu=$(ARM_CORE)
CFLAGS 		= -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections \
			  -Os $(INCLUDES) $(DEFINES)

CPPFLAGS 	= -mcpu=$(ARM_CORE) -mthumb \
			  -Wall -fno-rtti -fno-exceptions \
			  -Os $(INCLUDES) $(DEFINES)

LINKFLAGS 	= -nodefaultlibs -Xlinker --gc-sections -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map -mcpu=$(ARM_CORE) -mthumb

else ifeq (spy, $(CONF))  # Spy configuration ................................

BIN_DIR 	:= spy
DEFINES		+= -DQ_SPY
LIBS    	:= -lqp_$(ARM_CORE)_cs -llwip_$(ARM_CORE)_cs
ASFLAGS 	= -g -mthumb -mcpu=$(ARM_CORE)
CFLAGS 		= -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections \
			  -Os -g -O $(INCLUDES) $(DEFINES)

CPPFLAGS 	= -mcpu=$(ARM_CORE) -mthumb \
			  -Wall -fno-rtti -fno-exceptions \
			  -g -O $(INCLUDES) $(DEFINES)

LINKFLAGS 	= -nodefaultlibs -Xlinker --gc-sections -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map -mcpu=$(ARM_CORE) -mthumb -g3 -gdwarf-2

else                     # default Debug configuration .......................

BIN_DIR 	:= dbg
LIBS    	:= -lqp_$(ARM_CORE)_cs -llwip_$(ARM_CORE)_cs
ASFLAGS 	= -g -mthumb -mcpu=$(ARM_CORE)
CFLAGS 		= -mcpu=$(ARM_CORE) -std=gnu99 -mthumb -Wall -ffunction-sections -fdata-sections \
			  -g -Os $(INCLUDES) $(DEFINES)

CPPFLAGS 	= -mcpu=$(ARM_CORE) -mthumb \
			  -Wall -fno-rtti -fno-exceptions \
			  -g -O $(INCLUDES) $(DEFINES)
	
LINKFLAGS 	= -nodefaultlibs -Xlinker --gc-sections -Wl,-Map,$(BIN_DIR)/$(PROJECT_NAME).map -mcpu=$(ARM_CORE) -mthumb -g3 -gdwarf-2

endif

ASM_OBJS     = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS       = $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS     = $(patsubst %.cpp,%.o,$(CPP_SRCS))

TARGET_BIN   = $(BIN_DIR)/$(PROJECT_NAME).hex
TARGET_ELF   = $(BIN_DIR)/$(PROJECT_NAME).elf
TARGET_FLSH  = $(BIN_DIR)/$(PROJECT_NAME).bin
ASM_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   = $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   = $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT = $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT = $(patsubst %.o, %.d, $(CPP_OBJS_EXT))

REAL_VER     = BOOT_$(BOARD)$(SERIES)_$(PRODUCT)_$(MAINT)_$(DEVIATION)
SPECIAL_VER  = BOOT_$(BOARD)$(SERIES)_$(PRODUCT)_$(MAINT)_$(SPECIAL)
DEBUG_VER    = BOOT_$(BOARD)$(SERIES)_0_$(MAINT)_65535

#-----------------------------------------------------------------------------
# rules
#

# Default rule:
all: ver build_IP $(BIN_DIR) build_libs $(TARGET_BIN) showver

# Rule for making a special build
special: special_ver build_IP $(BIN_DIR) build_libs $(TARGET_BIN) showver

showver:
	@if [ $(IPADDR0) == 169 ] ; then \
	   echo;\
	   echo -------------------------------------------------;\
		cat $(SRC_DIR)/version.h | grep FW_VER | cut -b 9-;\
	   echo         Built for IP address $(IPADDR0).$(IPADDR1).$(IPADDR2).$(IPADDR3)    --SLAVE DEVICE--;\
	   echo -------------------------------------------------;\
	   echo;\
	else\
	   echo;\
	   echo +++++++++++++++++++++++++++++++++;\
		cat $(SRC_DIR)/version.h | grep FW_VER | cut -b 9-;\
	   echo         Built for IP address $(IPADDR0).$(IPADDR1).$(IPADDR2).$(IPADDR3);\
	   echo +++++++++++++++++++++++++++++++++;\
	   echo;\
	fi

ver: config.mk
	@# This format matches what the printer expects so please leave it
	@if [ $(CONF) == "rel" ] ; then \
	  echo "#define "FW_VER" \"$(REAL_VER)\"" > $(SRC_DIR)/version.h;\
	else\
	  echo "#define "FW_VER" \"$(DEBUG_VER)\"" > $(SRC_DIR)/version.h;\
	fi
	@echo "#define "BUILD_DATE" \"$(shell date +"%b %d %Y %T")\"" >> $(SRC_DIR)/version.h

special_ver: config.mk
	@# This format matches what the printer expects so please leave it
	@echo "#define "FW_VER" \"$(SPECIAL_VER)\"" > $(SRC_DIR)/version.h
	@echo "#define "BUILD_DATE" \"$(shell date +"%b %d %Y %T")\"" >> $(SRC_DIR)/version.h

# All the rigamarole below is to avoid building code if the ip address didn't change (normal situation for development)
build_IP:
	@if [ ! -e $(SRC_DIR)/ipAndMac.h ]; then echo "placeholder" > $(SRC_DIR)/ipAndMac.h; fi
	@echo "#define STATIC_IPADDR0" "$(IPADDR0)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR1" "$(IPADDR1)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR2" "$(IPADDR2)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define STATIC_IPADDR3" "$(IPADDR3)" >> $(SRC_DIR)/ipAndMac.new.h
	@echo "#define DCC_MAC" $(MAC)"" >> $(SRC_DIR)/ipAndMac.new.h
	@-diff $(SRC_DIR)/ipAndMac.new.h $(SRC_DIR)/ipAndMac.h > $(SRC_DIR)/ipAndMac.diff
	@if [ -s $(SRC_DIR)/ipAndMac.diff ]; then mv $(SRC_DIR)/ipAndMac.new.h $(SRC_DIR)/ipAndMac.h; fi
	@-rm -f $(SRC_DIR)/ipAndMac.diff $(SRC_DIR)/ipAndMac.new.h

$(BIN_DIR):
	@echo ---------------------------
	@echo --- Creating directory ---
	@echo ---------------------------
	$(TRACE_FLAG)mkdir -p $@

$(TARGET_BIN): $(TARGET_ELF)
	@echo --- Creating binaries ---
	$(TRACE_FLAG)$(OBJCPY) -O binary $< $@
	$(TRACE_FLAG)mv $(TARGET_BIN) $(TARGET_FLSH)
	$(TRACE_FLAG)$(OBJCPY) -O ihex $< $@
	

$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	@echo --- Linking libraries   ---
	$(TRACE_FLAG)$(LINK) -T$(LD_SCRIPT) $(LINKFLAGS) -L$(QP_PORT_DIR)/$(BIN_DIR) -L$(LWIP_DIR)/$(BIN_DIR) -o $@ $^ $(LIBS)
	
build_libs: build_qpc build_lwip

build_qpc:
	@echo ---------------------------
	@echo --- Building QPC libraries ---
	@echo ---------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make CONF=$(BIN_DIR)

build_lwip:
	@echo ---------------------------
	@echo --- Building LWIP libraries ---
	@echo ----Passing in current proj directory
	#TODO: The passed in directories must be relative to the LWIP library, not this project
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) TRACE=$(TRACE)

$(BIN_DIR)/%.d : %.c
	$(TRACE_FLAG)$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(TRACE_FLAG)$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.S
	@echo --- Compiling $< ---
	$(TRACE_FLAG)$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	@echo --- Compiling $< ---
	$(TRACE_FLAG)$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(TRACE_FLAG)$(CPP) $(CPPFLAGS) -c $< -o $@

# Make sure not to generate dependencies when doing cleans
NODEPS:=clean cleanall cleanlibs cleandirs
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
endif

.PHONY : clean clean_with_libs
cleanall:
	@echo ---------------------------
	@echo --- Cleaning EVERYTHING
	@echo ---------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make CONF=dbg clean
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make CONF=spy clean
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make CONF=rel clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) CONF=dbg clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) CONF=spy clean
	$(TRACE_FLAG)cd $(LWIP_DIR); make LWIP_PORT_DIR=$(LWIP_PORT_FOR_LWIP) PROJ_BSP=$(BSP_DIR_FOR_LWIP) LWIP_SRC_DIR=$(LWIP_SRC_DIR) CONF=rel clean
	-$(RM) dbg/*.o dbg/*.d dbg/*.hex dbg/*.elf dbg/*.map dbg
	-$(RM) spy/*.o spy/*.d spy/*.hex spy/*.elf spy/*.map rel
	-$(RM) rel/*.o rel/*.d rel/*.hex rel/*.elf rel/*.map spy
	
clean:
	$(TRACE_FLAG)-$(RM) $(BIN_DIR)/*.o \
	$(TRACE_FLAG)$(BIN_DIR)/*.d \
	$(TRACE_FLAG)$(BIN_DIR)/*.hex \
	$(TRACE_FLAG)$(BIN_DIR)/*.elf \
	$(TRACE_FLAG)$(BIN_DIR)/*.map
	
clean_with_libs: clean clean_qpc_libs
	@echo ---------------------------
	@echo --- Cleaned All libs and bins
	@echo ---------------------------
	
clean_qpc_libs:
	@echo ---------------------------
	@echo --- Cleaning QPC libraries
	@echo ---------------------------
	$(TRACE_FLAG)cd $(QP_PORT_DIR); make CONF=$(BIN_DIR) clean

show:
	@echo CONF = $(CONF)
	@echo ASM_SRCS = $(ASM_SRCS)
	@echo C_SRCS = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)
	@echo LIBS = $(LIBS)
