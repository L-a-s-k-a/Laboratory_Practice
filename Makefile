# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------
# List of supported controllers:
#	STM32F446RE -- Target -> STM32F446RET
# 	STM32F429ZI -- Target -> STM32F429ZI
# 	STM32F411VE -- Target -> STM32F411VET
#	STM32F411CE -- Target -> STM32F411CEU
# 	STM32F407VE -- Target -> STM32F407VET
# 	STM32F103C8 -- Target -> STM32F103x6
# 	STM32F103C6 -- Target -> STM32F103C8Tx
# ------------------------------------------------

######################################
# target
######################################
TARGET = STM32F429ZI

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og

#######################################
# paths
#######################################
# Build path
ifeq ($(TARGET), STM32F103C8Tx)
	BUILD_DIR = build_F103C8Tx
else ifeq ($(TARGET), STM32F103x6)
	BUILD_DIR = build_F103x6
else ifeq ($(TARGET), STM32F407VET)
	BUILD_DIR = build_F407VET
else ifeq ($(TARGET), STM32F411VET)
	BUILD_DIR = build_F411VET
else ifeq ($(TARGET), STM32F411CEU)
	BUILD_DIR = build_F411CEU
else ifeq ($(TARGET), STM32F429ZI)
	BUILD_DIR = build_F429ZI
else ifeq ($(TARGET), STM32F446RET)
	BUILD_DIR = build_F446RET
endif
######################################
# selecting source
######################################
# C sources

ifeq ($(TARGET), STM32F103C8Tx)
	SYS = CMSIS/Devices/STM32F1xx/Src/system_stm32f1xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F103C8Tx/startup_stm32f103xb.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F1xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F1xx/Inc/STM32f103xB
	CMSIS_INC = CMSIS/Include
	LD = STMDevices/STM32F103C8Tx/STM32F103C8Tx_FLASH.ld
	DEF = STM32F103xB
	MCPU = cortex-m3
	MFPU = NONE
	TRGT_CFG = stm32f1x
else ifeq ($(TARGET), STM32F103x6)
	SYS = CMSIS/Devices/STM32F1xx/Src/system_stm32f1xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F103x6/startup_stm32f103x6.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F1xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F1xx/Inc/STM32f103x6
	CMSIS_INC = CMSIS/Include
	LD = STMDevices/STM32F103x6/STM32F103X6_FLASH.ld
	DEF = STM32F103x6
	MCPU = cortex-m3
	MFPU = NONE
	TRGT_CFG = stm32f1x
else ifeq ($(TARGET), STM32F407VET)
	SYS = CMSIS/Devices/STM32F4xx/Src/system_stm32f4xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F407xx/startup_stm32f407xx.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F4xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F4xx/Inc/STM32F407xx
	CMSIS_INC = CMSIS/Include
	LD = STMDevices/STM32F407xx/STM32F407VETx_FLASH.ld
	DEF = STM32F407xx
	MCPU = cortex-m4
	MFPU = fpv4-sp-d16 
	TRGT_CFG = stm32f4x
else ifeq ($(TARGET), STM32F411VET)
	SYS = CMSIS/Devices/STM32F4xx/Src/system_stm32f4xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F411xe/startup_stm32f411xe.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F4xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F4xx/Inc/STM32F411xE
	CMSIS_INC = CMSIS/Include
	ifeq ($(TARGET), STM32F411VET)
		LD = STMDevices/STM32F411xe/STM32F411VETx_FLASH.ld
	else ifeq ($(TARGET), STM32F411CEU)
		LD = STMDevices/STM32F411xe/STM32F411CEUx_FLASH.ld
	endif
	DEF = STM32F411xE
	MCPU = cortex-m4
	MFPU = fpv4-sp-d16
	TRGT_CFG = stm32f4x
else ifeq ($(TARGET), STM32F429ZI)
	SYS = CMSIS/Devices/STM32F4xx/Src/system_stm32f4xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F429xx/startup_stm32f429xx.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F4xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F4xx/Inc/STM32F429xx
	CMSIS_INC = CMSIS/Include
	LD = STMDevices/STM32F429xx/STM32F429ZITx_FLASH.ld
	DEF = STM32F429xx
	MCPU = cortex-m4
	MFPU = fpv4-sp-d16  #"vfpv4-d16"
	TRGT_CFG = stm32f4x
else ifeq ($(TARGET), STM32F446RET)
	SYS = CMSIS/Devices/STM32F4xx/Src/system_stm32f4xx.c \ CMSIS/Devices/Src/syscalls.c \ CMSIS/Devices/Src/sysmem.c
	ASM = STMDevices/STM32F446xx/startup_stm32f446xx.s
	CMSIS_INC_DEV = CMSIS/Devices/STM32F4xx/Inc
	CMSIS_INC_UNIT = CMSIS/Devices/STM32F4xx/Inc/STM32F446xx
	CMSIS_INC = CMSIS/Include
	LD = STMDevices/STM32F446xx/STM32F446RETx_FLASH.ld
	DEF = STM32F446xx
	MCPU = cortex-m4
	MFPU = fpv4-sp-d16
	TRGT_CFG = stm32f4x
endif

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(SYS) \
Core/Src/main.c \

# ASM sources
ASM_SOURCES =  \
$(ASM)

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
	CC = $(GCC_PATH)/$(PREFIX)gcc
	AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
	CP = $(GCC_PATH)/$(PREFIX)objcopy
	SZ = $(GCC_PATH)/$(PREFIX)size
else
	CC = $(PREFIX)gcc
	AS = $(PREFIX)gcc -x assembler-with-cpp
	CP = $(PREFIX)objcopy
	SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=$(MCPU)

ifneq ($(MFPU), NONE)
	# fpu
	FPU = -mfpu=$(MFPU)
	# float-abi
	FLOAT-ABI = -mfloat-abi=hard
endif

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-D$(DEF) \

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I$(CMSIS_INC_DEV) \
-I$(CMSIS_INC_UNIT) \
-I$(CMSIS_INC) \
-ICore/Inc \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
	CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = $(LD) -Wl,--no-warn-rwx-segment # "-Wl,--no-warn-rwx-segment" - отключает вывод предупреждения о том,
#										   # что .elf содержит сегмент LOAD с правами RWX (Read, Write, X)
#										   # Нужно разбораться с этой проблемой  
#										   #
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin 
	@echo "==========================================="
	@echo "|| The build was completed successfully! ||"
	@echo "==========================================="

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo "---------------------   SIZE   ----------------------"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@  -Wl,--print-memory-usage
	@echo "" 
	@$(SZ) $@
	@echo "-----------------------------------------------------"	

$(BUILD_DIR):
	@mkdir $@

#######################################
# clean up
#######################################
clean:
	@"rm" -fR $(BUILD_DIR)
  
#######################################
# openocd
#######################################
flash: all
	openocd -f interface/stlink.cfg -f target/$(TRGT_CFG).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
	@echo "=================================="
	@echo "|| The download was successful! ||"
	@echo "=================================="

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
