#include "base.h"

#define ADDR 0b10011000

static bool i2c_err = false;

static uint8_t MY_Read_REG(uint8_t reg)
{
    if (i2c_err) { return 0; }

    uint8_t data = 0;
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 100);
    if (flag != HAL_OK) {
        /* i2c_err = true; */
        printf("pcm1792 read err\n");
    }
    return data;
}

static void MY_Write(uint8_t reg, uint8_t *data, uint8_t len)
{
    if (i2c_err) { return; }

    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, ADDR, (uint16_t)reg, 1, data, len, 500);
    if (flag != HAL_OK) {
        /* i2c_err = true; */
        printf("pcm1792 write err\n");
    }
}

static void MY_Write_REG(uint8_t reg, uint8_t data)
{
    if (i2c_err) { return; }

    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        /* i2c_err = true; */
        printf("pcm1792 write err\n");
    }
}

static uint8_t reg_18 = 0b10000000;

void pcm1792_init(void)
{
    //初始化引脚
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* 5v 电源 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
    vTaskDelay(100);
    /* ±15v 电源 */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 1);
    vTaskDelay(100);
}

void pcm1792_play(uint32_t AudioFreq, uint8_t bit_depth)
{
    reg_18 &= ~(7<<4);

    if (bit_depth == 16) {
        reg_18 |= 4<<4;
    } else {
        reg_18 |= 5<<4;
    }

    MY_Write_REG(18, reg_18);
}

void pcm1792_mute(bool ok)
{
    if (ok) {
        reg_18 |= 1;
    } else {
        reg_18 &= ~1;
    }

    MY_Write_REG(18, reg_18);
}

void pcm1792_volume(uint8_t vol)
{
    if (vol > 0xCF) {
        vol = 0xCF;
    }

    if (vol > 0) {
        vol = (0xFF-0xCF) + vol;
    }

    /* uint8_t d[] = {vol, vol}; */
    /* MY_Write(16, d, 2); */

    MY_Write_REG(16, vol);
    MY_Write_REG(17, vol);
}

void pcm1792_stop(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 0);
}

