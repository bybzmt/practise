#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "base.h"
#include "i2c.h"

static volatile uint8_t i2c_run;

void my_i2c_init(I2C_HandleTypeDef *hi2c)
{
    hi2c->Instance              = I2C1;
    hi2c->Init.ClockSpeed       = 400000;
    hi2c->Init.DutyCycle        = I2C_DUTYCYCLE_2;
    hi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hi2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hi2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hi2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    hi2c->Init.OwnAddress1      = 0x11;
    hi2c->Init.OwnAddress2      = 0;

    if (HAL_I2C_Init(hi2c) != HAL_OK) {
        printf("i2c init error");
        while(1);
    }
}

static void I2C_get_err(I2C_HandleTypeDef *hi2c)
{
    switch (HAL_I2C_GetError(hi2c)) {
        case HAL_I2C_ERROR_NONE: return;
        case HAL_I2C_ERROR_BERR: printf("i2c err berr\n"); break;
        case HAL_I2C_ERROR_ARLO: printf("i2c err arlo\n"); break;
        case HAL_I2C_ERROR_AF: printf("i2c err af\n"); break;
        case HAL_I2C_ERROR_OVR: printf("i2c err ovr\n"); break;
        case HAL_I2C_ERROR_DMA: printf("i2c err dma\n"); break;
        case HAL_I2C_ERROR_TIMEOUT: printf("i2c err timeout\n"); break;
    }
}

static bool my_i2c_wait_done(I2C_HandleTypeDef *hi2c)
{
    int16_t num = 255;

    while (i2c_run == 0) {
        if (--num<1) {
            printf("i2c timeout\n");
            return false;
        }
        vTaskDelay(1);
    }

    if (i2c_run == 2) {
        I2C_get_err(hi2c);
        return false;
    }

    return true;
}

bool my_i2c_mem_write(I2C_HandleTypeDef *hi2c, uint16_t addr, uint16_t mbuf, uint16_t mlen, uint8_t *buf, size_t len)
{
    i2c_run = 0;

    HAL_StatusTypeDef re = HAL_I2C_Mem_Write_DMA(hi2c, addr, mbuf, mlen, buf, len);
    if (re != HAL_OK) {
        printf("i2c_mem_write err\n");
        return false;
    }

    return my_i2c_wait_done(hi2c);
}

bool my_i2c_mem_read(I2C_HandleTypeDef *hi2c, uint16_t addr, uint16_t subAddr, uint16_t addrSize, uint8_t *buf, size_t len)
{
    i2c_run = 0;

    HAL_StatusTypeDef re = HAL_I2C_Mem_Read_DMA(hi2c, addr, subAddr, addrSize, buf, len);
    if (re != HAL_OK) {
        printf("i2c_mem_read err\n");
        return false;
    }

    return my_i2c_wait_done(hi2c);
}

bool my_i2c_write(I2C_HandleTypeDef *hi2c, uint16_t addr, uint8_t *buf, size_t len)
{
    i2c_run = 0;

    HAL_StatusTypeDef re = HAL_I2C_Master_Transmit_DMA(hi2c, addr, buf, len);
    if (re != HAL_OK) {
        printf("i2c_write err\n");
        return false;
    }

    return my_i2c_wait_done(hi2c);
}

bool my_i2c_read(I2C_HandleTypeDef *hi2c, uint16_t addr, uint8_t *buf, size_t len)
{
    i2c_run = 0;

    HAL_StatusTypeDef re = HAL_I2C_Master_Receive_DMA(hi2c, addr, buf, len);
    if (re != HAL_OK) {
        printf("i2c_read err\n");
        return false;
    }

    return my_i2c_wait_done(hi2c);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    i2c_run = 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    i2c_run = 1;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    i2c_run = 2;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    i2c_run = 1;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    i2c_run = 1;
}
