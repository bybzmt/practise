#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "base.h"
#include "i2c.h"

static QueueHandle_t i2c_queue;

typedef enum {
    I2C_SUCCESS = 1,
    I2C_FAIL = 2,
} I2C_RET;

I2C_HandleTypeDef hi2c1;

void my_i2c_init(void)
{
    hi2c1.Instance              = I2C1;
    hi2c1.Init.ClockSpeed       = 400000;
    hi2c1.Init.DutyCycle        = I2C_DUTYCYCLE_2;
    hi2c1.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    hi2c1.Init.OwnAddress1      = 0x11;
    hi2c1.Init.OwnAddress2      = 0;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        printf("i2c_init_err");
        while(1);
    }

    i2c_queue = xQueueCreate(2, sizeof(I2C_RET));
}

static void I2C_get_err(void)
{
    switch (HAL_I2C_GetError(&hi2c1)) {
        case HAL_I2C_ERROR_NONE: return;
        case HAL_I2C_ERROR_BERR: printf("i2c err berr\n"); break;
        case HAL_I2C_ERROR_ARLO: printf("i2c err arlo\n"); break;
        case HAL_I2C_ERROR_AF: printf("i2c err af\n"); break;
        case HAL_I2C_ERROR_OVR: printf("i2c err ovr\n"); break;
        case HAL_I2C_ERROR_DMA: printf("i2c err dma\n"); break;
        case HAL_I2C_ERROR_TIMEOUT: printf("i2c err timeout\n"); break;
    }
}

static bool my_i2c_wait_done(void)
{
    I2C_RET flag;
    if (xQueueReceive(i2c_queue, &flag, 1000) != pdTRUE) {
        printf("i2c_timeout");
        return false;
    }

    switch (flag) {
        case I2C_SUCCESS:
            return true;
        case I2C_FAIL:
            I2C_get_err();
            return false;
        default:
            return false;
    }
}

bool my_i2c_mem_write(uint16_t addr, uint16_t mbuf, uint16_t mlen, uint8_t *buf, size_t len)
{
    HAL_StatusTypeDef re = HAL_I2C_Mem_Write_DMA(&hi2c1, addr, mbuf, mlen, buf, len);
    if (re != HAL_OK) {
        printf("i2c_mem_write_err\n");
        return false;
    }

    return my_i2c_wait_done();
}

bool my_i2c_mem_read(uint16_t addr, uint16_t subAddr, uint16_t addrSize, uint8_t *buf, size_t len)
{
    HAL_StatusTypeDef re = HAL_I2C_Mem_Read_DMA(&hi2c1, addr, subAddr, addrSize, buf, len);
    if (re != HAL_OK) {
        printf("i2c_mem_read_err\n");
        return false;
    }

    return my_i2c_wait_done();
}

bool my_i2c_write(uint16_t addr, uint8_t *buf, size_t len)
{
    HAL_StatusTypeDef re = HAL_I2C_Master_Transmit_DMA(&hi2c1, addr, buf, len);
    if (re != HAL_OK) {
        printf("i2c_write_err\n");
        return false;
    }

    return my_i2c_wait_done();
}

bool my_i2c_read(uint16_t addr, uint8_t *buf, size_t len)
{
    HAL_StatusTypeDef re = HAL_I2C_Master_Receive_DMA(&hi2c1, addr, buf, len);
    if (re != HAL_OK) {
        printf("i2c_read_err\n");
        return false;
    }

    return my_i2c_wait_done();
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    I2C_RET flag = I2C_SUCCESS;
    xQueueSendFromISR(i2c_queue, &flag, NULL);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    I2C_RET flag = I2C_SUCCESS;
    xQueueSendFromISR(i2c_queue, &flag, NULL);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    I2C_RET flag = I2C_FAIL;
    xQueueSendFromISR(i2c_queue, &flag, NULL);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    I2C_RET flag = I2C_SUCCESS;
    xQueueSendFromISR(i2c_queue, &flag, NULL);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    UNUSED(hi2c);
    I2C_RET flag = I2C_SUCCESS;
    xQueueSendFromISR(i2c_queue, &flag, NULL);
}
