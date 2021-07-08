#include <stdbool.h>
#include <stdio.h>
// freeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "msp_i2c.h"

#define SSD1306_I2C_ADDR        (0x3C << 1)

void ssd1306_Delay(uint16_t tick)
{
    vTaskDelay(tick);
}

void ssd1306_Reset(void) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
bool ssd1306_WriteCommand(uint8_t byte) {
    return msp_i2c_write_reg(SSD1306_I2C_ADDR, 0x00, byte);
}

// Send data
bool ssd1306_WriteData(uint8_t* buffer, size_t buf_size) {
    return msp_i2c_write(SSD1306_I2C_ADDR, 0x40, buffer, buf_size);
}
