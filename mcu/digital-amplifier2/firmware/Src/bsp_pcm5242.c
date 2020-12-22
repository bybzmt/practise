#include "base.h"

#define ADDR 0b10011000

static uint8_t MY_Read_REG(uint8_t reg)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 100);
    return data;
}

static void MY_Write_REG(uint8_t reg, uint8_t data)
{
    HAL_I2C_Mem_Write_DMA(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1);
}

void bsp_pcm5242_init(void)
{
    //初始化引脚
    //mute
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
    //en
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);

    /* w 98 00 00 # PCM5242 back to page 0 */
    MY_Write_REG(0x00, 0x00);
    /* w 98 02 11 # PCM5242 output to 1 Vrms */
    MY_Write_REG(0x02, 0x11);
    /* w 98 3B 66 # set auto mute time to six seconds of audio zero. */
    MY_Write_REG(0x3b, 0x66);
    /* w 98 3C 01 # Left Vol register controls both */
    MY_Write_REG(0x3c, 0x01);
    /* w 98 3D 4F # Change left channel volume, right will follow. */
    MY_Write_REG(0x3d, 0x4f);
    /* w 98 3F BB # set vol changes for every 4 samples, 0.5 sample steps. */
    MY_Write_REG(0x3f, 0xbb);
}

void bsp_pcm5242_play()
{
    //mute
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
}

static void bsp_pcm5242_start()
{
    //根据耳机是否插入判断是不明
}

void bsp_pcm5242_mute()
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void bsp_pcm5242_stop()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
}


void bsp_pcm5242_headphone_in_irq()
{
}
