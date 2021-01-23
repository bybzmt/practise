#include "ssd1306.h"
#include <stdbool.h>
#include <stdio.h>

// freeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"

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

void wait_i2c() {
    HAL_I2C_StateTypeDef state;

    while(true) {
        state = HAL_I2C_GetState(&SSD1306_I2C_PORT);
        switch(state) {
            case HAL_I2C_STATE_BUSY:
            case HAL_I2C_STATE_BUSY_TX:
            case HAL_I2C_STATE_BUSY_RX:
                vTaskDelay(1);
                break;
            default:
                return;
        }
    }
}

// Send a byte to the command register
bool ssd1306_WriteCommand(uint8_t byte) {
    /* HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY); */

    wait_i2c();
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write_DMA(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1);
    if (flag != HAL_OK) {
        printf("ssd1306 write err\n");
        return false;
    }
    return true;
}

// Send data
bool ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    /* HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY); */

    wait_i2c();
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write_DMA(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size);
    if (flag != HAL_OK) {
        printf("ssd1306 write err\n");
        return false;
    }
    return true;
}
