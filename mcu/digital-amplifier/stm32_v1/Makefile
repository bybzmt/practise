
#工程的名称及最后生成文件的名字
TARGET = ./build/target.elf

#设定临时性环境变量
export CC             = arm-none-eabi-gcc
export AS             = arm-none-eabi-as
export LD             = arm-none-eabi-ld
export OBJCOPY        = arm-none-eabi-objcopy

OBJDUMP=arm-none-eabi-objdump
GDB = arm-none-eabi-gdb

#openocd
OOCD = openocd
OOCDFLAGS = -f interface/cmsis-dap.cfg
OOCDFLAGS+= -f target/stm32f1x.cfg

#读取当前工作目录
TOP=$(shell pwd)

LIB_DIRS= $(TOP)/src
LIB_DIRS+= $(TOP)/lib
LIB_DIRS+= $(TOP)/lib/CMSIS
LIB_DIRS+= $(TOP)/lib/CMSIS/inc
LIB_DIRS+= $(TOP)/lib/CMSIS/Device
LIB_DIRS+= $(TOP)/lib/CMSIS/Device/inc
LIB_DIRS+= $(TOP)/lib/STM32F1xx_HAL_Driver/Inc
LIB_DIRS+= $(TOP)/lib/STM32F1xx_HAL_Driver/Src

LIB_DIRS+= $(TOP)/lib/FreeRTOS
LIB_DIRS+= $(TOP)/lib/FreeRTOS/CMSIS_RTOS
LIB_DIRS+= $(TOP)/lib/FreeRTOS/include
LIB_DIRS+= $(TOP)/lib/FreeRTOS/portable/MemMang
LIB_DIRS+= $(TOP)/lib/FreeRTOS/portable/GCC/ARM_CM3

LIB_DIRS+= $(TOP)/lib/ssd1306

#查找所有有*.h文件的目录
C_H_DIR= $(LIB_DIRS)

#查找所有*.c文件
C_DIR =$(LIB_DIRS)

BUILD= ./build

#makefile查找目录
vpath
vpath %.h $(C_H_DIR)
vpath %.c $(C_DIR)
vpath %.o $(BUILD)
vpath %.d $(BUILD)

INC_FLAGS= $(addprefix -I, $(C_H_DIR))

#所有的需编译文件
C_SRC= main.c
C_SRC+=userMain.c
C_SRC+=stm32f1xx_it.c
C_SRC+=stm32f1xx_hal_msp.c
C_SRC+=system_stm32f1xx.c

C_SRC+=i2c.c

C_SRC+=ssd1306.c
C_SRC+=ssd1306_fonts.c
C_SRC+=ssd1306_tests.c
C_SRC+=ssd1306_lowLevel.c

C_SRC+=stm32f1xx_hal.c
C_SRC+=stm32f1xx_hal_cortex.c
C_SRC+=stm32f1xx_hal_flash.c
C_SRC+=stm32f1xx_hal_flash_ex.c
C_SRC+=stm32f1xx_hal_gpio.c
C_SRC+=stm32f1xx_hal_gpio_ex.c
C_SRC+=stm32f1xx_hal_rcc.c
C_SRC+=stm32f1xx_hal_rcc_ex.c
C_SRC+=stm32f1xx_hal_dma.c
C_SRC+=stm32f1xx_hal_i2c.c
# C_SRC+=stm32f1xx_hal_adc.c
# C_SRC+=stm32f1xx_hal_spi.c
# C_SRC+=stm32f1xx_hal_spi_ex.c
# C_SRC+=stm32f1xx_hal_adc_ex.c
# C_SRC+=stm32f1xx_hal_uart.c

#freeRTOS
C_SRC+=queue.c
C_SRC+=croutine.c
C_SRC+=tasks.c
C_SRC+=timers.c
C_SRC+=list.c
C_SRC+=event_groups.c
C_SRC+=port.c
C_SRC+=heap_4.c
C_SRC+=cmsis_os.c

C_OBJ=$(C_SRC:%.c=%.o)

CFLAGS= -mcpu=cortex-m3 -mthumb -std=gnu11
CFLAGS+= -g3 -Os
CFLAGS+= -W -Wall
CFLAGS+= -ffunction-sections -fdata-sections
CFLAGS+= -D STM32F103xB $(INC_FLAGS)
CFLAGS+= -DSSD1306_USE_I2C
CFLAGS+= -DSTM32F1
CFLAGS+= -Wl,--gc-sections

.PHONY: all clean update test dump debug release

all:$(TARGET)

LDFLAGS= -lc -lm -static

#Semihosting(半主机)
#LDFLAGS += --specs=rdimon.specs -lc -lrdimon
#Note: librdimon 替换 libnosys
#确保initialise_monitor_handles() 在printf之前调用
#extern void initialise_monitor_handles(void);

# CFLAGS+= -DSEMIHOSTING
# LDFLAGS+= --specs=rdimon.specs -lrdimon

LDFLAGS+= --specs=nosys.specs
# LDFLAGS+= --specs=nano.specs

LDFLAGS+= -L $(TOP)/lib/CMSIS/libarm_cortexM3l_math.a
# LDFLAGS+= -T $(TOP)/lib/CMSIS/Device/STM32F103XE_FLASH.ld
LDFLAGS+= -T $(TOP)/lib/CMSIS/Device/STM32F103XB_FLASH.ld
# LDFLAGS+= -T $(TOP)/lib/CMSIS/Device/STM32F103C8Tx_FLASH.ld
LDFLAGS+= $(TOP)/lib/CMSIS/Device/startup_stm32f103xb.s

$(TARGET): $(C_OBJ)
	$(CC) $(LDFLAGS)\
		-Wl,-Map=$(BUILD)/Project.map,--cref \
		-Wl,--defsym=malloc_getpagesize_P=0x80\
		$(CFLAGS) \
		$(addprefix $(BUILD)/, $(C_OBJ)) \
		-o $(TARGET)

release: $(C_OBJ)

%.o:%.c %.d
	$(CC) -c $(CFLAGS) -o $(BUILD)/$@ $<

dump: $(TARGET)
	arm-none-eabi-size $(TARGET)
	$(OBJCOPY) $(<) $(<:%.elf=%.bin) -Obinary
	$(OBJCOPY) $(<) $(<:%.elf=%.hex) -Oihex
	$(OBJDUMP) -S $< > $(<:%.elf=%.dump)
	ls -hl $(<:%.elf=%.bin)

test:
	echo test

clean:
	rm -f $(BUILD)/*
	rm -f $(TARGET)

update: $(TARGET)
	$(OOCD) $(OOCDFLAGS) -c init -c targets -c halt -c "program $(TARGET) verify reset exit"

openocd:
	$(OOCD) $(OOCDFLAGS) -c gdb_port 3333 -c init -c targets

gdb: $(TARGET)
	$(GDB) -iex 'target remote localhost:3333' \
		-iex 'mon reset halt' \
		-iex 'monitor arm semihosting enable' \
		-iex 'set mem inaccessible-by-default off' \
		$<

%.d:%.c
	@set -e; \
	$(CC) -MM $(CFLAGS) $< > $(BUILD)/$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $(BUILD)/$@.$$$$ > $(BUILD)/$@; \
	rm -f $(BUILD)/$@.$$$$;

-include $(C_SRC:%.c=$(BUILD)/%.d)
