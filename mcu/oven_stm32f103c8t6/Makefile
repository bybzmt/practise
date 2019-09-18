
#工程的名称及最后生成文件的名字
TARGET = ./build/LED_project.elf

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
DRIVERS=$(realpath $(TOP)/Drivers)

LIB_DIRS= $(TOP)
LIB_DIRS+= $(DRIVERS)/CMSIS
LIB_DIRS+= $(DRIVERS)/CMSIS/Lib/GCC
LIB_DIRS+= $(DRIVERS)/CMSIS/Device/ST/STM32F1xx
LIB_DIRS+= $(DRIVERS)/STM32F1xx_HAL_Driver

#查找所有有*.h文件的目录
C_H_DIR= $(LIB_DIRS)
C_H_DIR+= $(sort $(foreach t1, $(LIB_DIRS), \
		   $(foreach t2, inc Inc include Include, \
		   $(dir $(wildcard $(t1)/$(t2)/*.h)) \
		)))

#查找所有*.c文件
C_DIR =$(LIB_DIRS)
C_DIR+= $(sort $(foreach t1, $(LIB_DIRS), \
		   $(foreach t2, src Src Source, \
		   $(dir $(wildcard $(t1)/$(t2)/*.c) \
		))))

BUILD= ./build

#makefile查找目录
vpath
vpath %.h $(C_H_DIR)
vpath %.c $(C_DIR)
vpath %.o $(BUILD)

INC_FLAGS= $(addprefix -I, $(C_H_DIR))

#所有的需编译文件
C_SRC= main.c stm32f1xx_it.c debug.c
C_SRC+=adc.c
C_SRC+=MAX31865.c
C_SRC+=spi.c
C_SRC+=stm32f1xx_hal_msp.c
C_SRC+=system_stm32f1xx.c

C_SRC+=stm32f1xx_hal.c stm32f1xx_hal_cortex.c stm32f1xx_hal_flash.c stm32f1xx_hal_flash_ex.c stm32f1xx_hal_gpio.c stm32f1xx_hal_gpio_ex.c stm32f1xx_hal_rcc.c stm32f1xx_hal_rcc_ex.c
C_SRC+=stm32f1xx_hal_dma.c
C_SRC+=stm32f1xx_hal_adc.c
C_SRC+=stm32f1xx_hal_spi.c
C_SRC+=stm32f1xx_hal_spi_ex.c
C_SRC+=stm32f1xx_hal_adc_ex.c
C_SRC+=stm32f1xx_hal_uart.c

C_OBJ=$(C_SRC:%.c=%.o)

CFLAGS= -mcpu=cortex-m3 -mthumb -std=gnu11
CFLAGS+= -g3 -Os
CFLAGS+= -W -Wall
CFLAGS+= -ffunction-sections -fdata-sections
CFLAGS+= -D STM32F103x6 $(INC_FLAGS)

LDFILE = -T $(TOP)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/STM32F103X6_FLASH.ld
LDFILE+=$(TOP)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/startup_stm32f103x6.s

.PHONY: all clean update test dump debug

all:$(TARGET)

$(TARGET): $(C_OBJ)
	$(CC) -L libarm_cortexM3l_math.a \
		-lc -lm -specs=nano.specs -specs=nosys.specs -static\
		-Wl,-Map=$(BUILD)/Project.map \
		-Wl,--gc-sections \
		-Wl,--defsym=malloc_getpagesize_P=0x80\
		$(CFLAGS) \
		$(LDFILE) \
		$(addprefix $(BUILD)/, $(C_OBJ)) \
		-o $(TARGET)

%.o:%.c
	$(CC) -c $(CFLAGS) -o $(BUILD)/$@ $<

dump: $(TARGET)
	$(OBJCOPY) $(<) $(BUILD)/$(<:%.elf=%.bin) -Obinary
	$(OBJCOPY) $(<) $(BUILD)/$(<:%.elf=%.hex) -Oihex
	$(OBJDUMP) -S $< > $(BUILD)/$(<:%.elf=%.dump)

test:
	echo test

clean:
	rm -f $(BUILD)/*
	rm -f $(TARGET)

update: $(TARGET)
	$(OOCD) $(OOCDFLAGS) -c init -c targets -c halt \
		-c "flash write_image erase $<" -c reset -c shutdown

debug: $(TARGET)
	$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
		-iex 'mon halt' $<

%.d:%.c
	@set -e; \
	$(CC) -MM $(CFLAGS) $< > $(BUILD)/$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $(BUILD)/$@.$$$$ > $(BUILD)/$@; \
	rm -f $(BUILD)/$@.$$$$;

-include $(C_SRC:%.c=$(BUILD)/%.d)
