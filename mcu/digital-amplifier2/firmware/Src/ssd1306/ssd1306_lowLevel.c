#include "ssd1306.h"
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <_ansi.h>

// freeRTOS
#include "FreeRTOS.h"
#include "task.h"

#include "i2c.h"

#define SSD1306_I2C_PORT		hi2c1
#define SSD1306_I2C_ADDR        (0x3C << 1)

extern I2C_HandleTypeDef SSD1306_I2C_PORT;

void ssd1306_Delay(uint16_t tick)
{
    vTaskDelay(tick);
}

void ssd1306_Reset(void) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
bool ssd1306_WriteCommand(uint8_t byte) {
    return my_i2c_mem_write(SSD1306_I2C_ADDR, 0x00, 1, &byte, 1);
    /* HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY); */
}

// Send data
bool ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    return my_i2c_mem_write(SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size);
    /* HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY); */
}
