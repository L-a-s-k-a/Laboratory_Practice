TARGET_SERIES := STM32F407
TARGET_VERSION := VG
VARIANT ?= cmsis
VARIANTS := registers macros cmsis

.DEFAULT_GOAL := all
.PHONY: all flash $(VARIANTS)

ifeq ($(VARIANT),all)
all: $(VARIANTS)

$(VARIANTS):
	$(MAKE) VARIANT=$@ all

flash:
	$(error Select VARIANT=registers, VARIANT=macros or VARIANT=cmsis)
else
ifeq ($(filter $(VARIANT),$(VARIANTS)),)
$(error Unknown VARIANT: $(VARIANT))
endif

BUILD_DIR := build/$(VARIANT)
TARGET := $(BUILD_DIR)/lab1
DEVICE_DIR := STMDevices/STM32F407xx
CMSIS_FAMILY := CMSIS/Devices/STM32F4xx
LDSCRIPT := STMDevices/STM32F407xx/$(TARGET_SERIES)$(TARGET_VERSION)Tx_FLASH.ld

C_SOURCES := Core/Src/$(VARIANT)/main.c Core/Src/$(VARIANT)/init.c
ASM_SOURCES := $(DEVICE_DIR)/startup_stm32f407xx.s
C_INCLUDES := -ICore/Inc/$(VARIANT)

ifeq ($(VARIANT),cmsis)
C_SOURCES += $(CMSIS_FAMILY)/Src/system_stm32f4xx.c
C_INCLUDES += -ICMSIS/Include -I$(CMSIS_FAMILY)/Inc -I$(CMSIS_FAMILY)/Inc/STM32F407xx
C_DEFS := -DSTM32F407xx
else
C_SOURCES += Core/Src/system_reset.c
endif

PREFIX := arm-none-eabi-
CC := $(PREFIX)gcc
MCU := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS := $(MCU) $(C_DEFS) $(C_INCLUDES) -Og -g3 -Wall -Wextra -ffunction-sections -fdata-sections
LDFLAGS := $(MCU) --specs=nano.specs --specs=nosys.specs -T$(LDSCRIPT) -Wl,--gc-sections,-Map=$(TARGET).map

OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(TARGET).elf $(TARGET).hex $(TARGET).bin

$(BUILD_DIR):
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(BUILD_DIR)' | Out-Null"

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(CC) $(MCU) -c $< -o $@

$(TARGET).elf: $(OBJECTS) $(LDSCRIPT) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(PREFIX)size $@

$(TARGET).hex: $(TARGET).elf
	$(PREFIX)objcopy -O ihex $< $@

$(TARGET).bin: $(TARGET).elf
	$(PREFIX)objcopy -O binary $< $@

flash: all
	openocd -f interface/stlink.cfg -c "transport select swd" -f target/stm32f4x.cfg -c "program $(TARGET).elf verify reset exit"

-include $(OBJECTS:.o=.d)
endif
