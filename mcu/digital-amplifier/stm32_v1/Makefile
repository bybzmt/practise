
#工程的名称及最后生成文件的名字
TARGET = digital_amplifier

# debug build?
DEBUG = 0
# optimization
OPT = -Og

# Build path
BUILD_DIR = build

# C sources
C_SOURCES = src/main.c
C_SOURCES+=src/userMain.c
C_SOURCES+=src/stm32f1xx_it.c
C_SOURCES+=src/stm32f1xx_hal_msp.c
C_SOURCES+=src/i2c.c
C_SOURCES+=src/oled.c
C_SOURCES+=src/tas5805.c

C_SOURCES+=lib/CMSIS/Device/system_stm32f1xx.c

C_SOURCES+=lib/ssd1306/ssd1306.c
C_SOURCES+=lib/ssd1306/ssd1306_fonts.c
C_SOURCES+=lib/ssd1306/ssd1306_tests.c
C_SOURCES+=lib/ssd1306/ssd1306_lowLevel.c

C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c
C_SOURCES+=lib/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c

#freeRTOS
C_SOURCES+=lib/FreeRTOS/queue.c
C_SOURCES+=lib/FreeRTOS/croutine.c
C_SOURCES+=lib/FreeRTOS/tasks.c
C_SOURCES+=lib/FreeRTOS/timers.c
C_SOURCES+=lib/FreeRTOS/list.c
C_SOURCES+=lib/FreeRTOS/event_groups.c
C_SOURCES+=lib/FreeRTOS/portable/GCC/ARM_CM3/port.c
C_SOURCES+=lib/FreeRTOS/portable/MemMang/heap_4.c
C_SOURCES+=lib/FreeRTOS/CMSIS_RTOS/cmsis_os.c

ASM_SOURCES = lib/CMSIS/Device/startup_stm32f103xb.s

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
GDB = $(GCC_PATH)/$(PREFIX)gdb
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
GDB = $(PREFIX)gdb
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#openocd
OOCD = openocd
OOCDFLAGS = -f interface/cmsis-dap.cfg
OOCDFLAGS+= -f target/stm32f1x.cfg

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=soft

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS = -DSTM32F103xB
C_DEFS+= -DSSD1306_USE_I2C
C_DEFS+= -DSTM32F1

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES = $(sort $(dir $(C_SOURCES)))

C_INCLUDES+= lib/CMSIS/inc
C_INCLUDES+= lib/CMSIS/Device/inc
C_INCLUDES+= lib/STM32F1xx_HAL_Driver/Inc
C_INCLUDES+= lib/FreeRTOS/include

# $(info $(sort $(dir $(C_SOURCES))) )

C_INCLUDES_I= $(addprefix -I, $(C_INCLUDES))


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES_I) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += -std=gnu11

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
# LDSCRIPT = lib/CMSIS/Device/STM32F103XE_FLASH.ld
LDSCRIPT = lib/CMSIS/Device/STM32F103XB_FLASH.ld
# LDSCRIPT = lib/CMSIS/Device/STM32F103C8Tx_FLASH.ld

# libraries
LIBS = -lc -lm
LIBDIR =
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#Semihosting(半主机)
ifeq ($(DEBUG), 1)
LDFLAGS += -specs=rdimon.specs
LIBS += -lrdimon
C_DEFS += -DSEMIHOSTING
else
LIBS += -lnosys
LDFLAGS += -specs=nano.specs
endif

.PHONY: all clean flash gdb openocd

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

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

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# Flash binary
#######################################
flash: all
	$(OOCD) $(OOCDFLAGS) -c init -c targets -c halt -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

#-st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x08000000

openocd:
	$(OOCD) $(OOCDFLAGS) -c gdb_port 3333 -c init -c targets

gdb:
	$(GDB) -iex 'target remote localhost:3333' \
		-iex 'mon reset halt' \
		-iex 'monitor arm semihosting enable' \
		-iex 'set mem inaccessible-by-default off' \
		$(BUILD_DIR)/$(TARGET).elf

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
