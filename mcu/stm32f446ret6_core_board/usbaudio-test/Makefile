######################################
# target
######################################
TARGET = f446-usbaudio


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
#usb audio lib select 1 or 2
AUDIO_LIB = 2

#######################################
# paths
#######################################
BUILD_DIR = build
RTOS_DIR = lib/FreeRTOSv10.2.1/FreeRTOS/Source
STM32Cube_DIR = lib/STM32Cube_FW_F4_V1.25.0

######################################
# source
######################################
# C sources
C_SOURCES =Src/main.c
C_SOURCES+=Src/userMain.c
C_SOURCES+=Src/stm32f4xx_it.c
C_SOURCES+=Src/bsp_audio.c
C_SOURCES+=Src/hal_pcd_msp.c
C_SOURCES+=Src/system_stm32f4xx.c

C_HAL_FILE = stm32f4xx_hal_pcd.c
C_HAL_FILE+=stm32f4xx_hal_pcd_ex.c
C_HAL_FILE+=stm32f4xx_ll_usb.c
C_HAL_FILE+=stm32f4xx_hal_i2c.c
C_HAL_FILE+=stm32f4xx_hal_i2c_ex.c
C_HAL_FILE+=stm32f4xx_hal_sai.c
C_HAL_FILE+=stm32f4xx_hal_sai_ex.c
C_HAL_FILE+=stm32f4xx_hal_tim.c
C_HAL_FILE+=stm32f4xx_hal_tim_ex.c
C_HAL_FILE+=stm32f4xx_hal_rcc.c
C_HAL_FILE+=stm32f4xx_hal_rcc_ex.c
C_HAL_FILE+=stm32f4xx_hal_flash.c
C_HAL_FILE+=stm32f4xx_hal_flash_ex.c
C_HAL_FILE+=stm32f4xx_hal_flash_ramfunc.c
C_HAL_FILE+=stm32f4xx_hal_gpio.c
C_HAL_FILE+=stm32f4xx_hal_dma_ex.c
C_HAL_FILE+=stm32f4xx_hal_dma.c
C_HAL_FILE+=stm32f4xx_hal_pwr.c
C_HAL_FILE+=stm32f4xx_hal_pwr_ex.c
C_HAL_FILE+=stm32f4xx_hal_cortex.c
C_HAL_FILE+=stm32f4xx_hal.c
C_HAL_FILE+=stm32f4xx_hal_exti.c

C_SOURCES+= $(addprefix $(STM32Cube_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/, $(C_HAL_FILE))

C_SOURCES+=$(RTOS_DIR)/queue.c
C_SOURCES+=$(RTOS_DIR)/croutine.c
C_SOURCES+=$(RTOS_DIR)/tasks.c
C_SOURCES+=$(RTOS_DIR)/timers.c
C_SOURCES+=$(RTOS_DIR)/list.c
C_SOURCES+=$(RTOS_DIR)/event_groups.c
C_SOURCES+=$(RTOS_DIR)/portable/GCC/ARM_CM4F/port.c
C_SOURCES+=$(RTOS_DIR)/portable/MemMang/heap_4.c
C_SOURCES+=$(STM32Cube_DIR)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c

# C includes
C_INCLUDES = Src
C_INCLUDES+= Src/usb_audio
C_INCLUDES+= $(STM32Cube_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc
C_INCLUDES+= $(STM32Cube_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy

#CMSIS
C_INCLUDES+= $(STM32Cube_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include
C_INCLUDES+= $(STM32Cube_DIR)/Drivers/CMSIS/Core/Include
C_INCLUDES+= $(STM32Cube_DIR)/Drivers/CMSIS/Include

#FreeRTOS
C_INCLUDES+= $(RTOS_DIR)/include
C_INCLUDES+= $(RTOS_DIR)/portable/GCC/ARM_CM4F
C_INCLUDES+= $(STM32Cube_DIR)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS

#usb audio lib
ifeq ($(AUDIO_LIB), 1)
C_INCLUDES+= Src/usb_audio1
C_INCLUDES+= Src/usb_audio1/lib

C_SOURCES+=Src/usb_audio1/lib/usbd_audio.c
C_SOURCES+=Src/usb_audio1/lib/usbd_core.c
C_SOURCES+=Src/usb_audio1/lib/usbd_ctlreq.c
C_SOURCES+=Src/usb_audio1/lib/usbd_ioreq.c
C_SOURCES+=Src/usb_audio1/usbd_conf.c
C_SOURCES+=Src/usb_audio1/usbd_desc.c
C_SOURCES+=Src/usb_audio1/usbd_audio_if.c
else
# C_INCLUDES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_Audio/Addons/PDM/Inc
C_INCLUDES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
C_INCLUDES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc
C_INCLUDES+= Src/usb_audio2

C_SOURCES+=Src/usb_audio2/usbd_conf.c
C_SOURCES+=Src/usb_audio2/usbd_desc.c
C_SOURCES+=Src/usb_audio2/usbd_audio_if.c
C_SOURCES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.c
C_SOURCES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
C_SOURCES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
C_SOURCES+= $(STM32Cube_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
endif

# ASM sources
ASM_SOURCES = startup_stm32f446xx.s

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

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F446xx \
-DUSE_USB_FS

# AS includes
AS_INCLUDES =


C_INCLUDES_I= $(addprefix -I, $(C_INCLUDES))

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES_I) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F446RETx_FLASH.ld

# libraries
LIBS = -lc -lm
LIBDIR =
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

#Semihosting(半主机)
ifeq ($(DEBUG), 1)
LDFLAGS += -specs=rdimon.specs
LIBS += -lrdimon
C_DEFS += -DSEMIHOSTING
else
LIBS += -lnosys
LDFLAGS += -specs=nano.specs
endif

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
OOCD = openocd
OOCDFLAGS = -f interface/cmsis-dap.cfg
OOCDFLAGS+= -f target/stm32f4x.cfg

flash:
	$(OOCD) $(OOCDFLAGS) -c init -c targets -c halt -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# -st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x08000000

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
