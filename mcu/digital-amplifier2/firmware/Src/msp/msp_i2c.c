#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "msp_i2c.h"

static void MY_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);
static void MY_I2C_MspInit(I2C_HandleTypeDef *hi2c);

static I2C_HandleTypeDef hi2c1 = {
    .Instance              = I2C1,
    .Init = {
        .ClockSpeed       = 100000,
        .DutyCycle        = I2C_DUTYCYCLE_2,
        .AddressingMode   = I2C_ADDRESSINGMODE_7BIT,
        .DualAddressMode  = I2C_DUALADDRESS_DISABLE,
        .GeneralCallMode  = I2C_GENERALCALL_DISABLE,
        .NoStretchMode    = I2C_NOSTRETCH_DISABLE,
        .OwnAddress1      = 0xFF,
        .OwnAddress2      = 0xFE,
    },
    .MspInitCallback = MY_I2C_MspInit,
    .MspDeInitCallback = MY_I2C_MspDeInit,
};

static void MY_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  gpio;
    gpio.Pin       = GPIO_PIN_6|GPIO_PIN_7;
    gpio.Mode      = GPIO_MODE_AF_OD;
    gpio.Pull      = GPIO_PULLUP;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &gpio);
}

static void MY_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    __HAL_RCC_I2C1_CLK_DISABLE();
}

void msp_i2c_init(void)
{
    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }
}

void msp_i2c_deInit(void)
{
    HAL_StatusTypeDef flag = HAL_I2C_DeInit(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c deInit err\n");
    }
}

bool msp_i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, addr, (uint16_t)reg, 1, data, 1, 100);
    if (flag != HAL_OK) {
        printf("i2c read %d/%d err\n", addr, reg);
        return false;
    }
    return true;
}

bool msp_i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, addr, (uint16_t)reg, 1, data, len, 500);
    if (flag != HAL_OK) {
        printf("i2c write %d/%d err\n", addr, reg);
        return false;
    }
    return true;
}

uint8_t msp_i2c_read_reg(uint8_t addr, uint8_t reg)
{
    uint8_t data = 0;
    msp_i2c_read(addr, reg, &data, 1);
    return data;
}

bool msp_i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data)
{
    return msp_i2c_write(addr, reg, &data, 1);
}


