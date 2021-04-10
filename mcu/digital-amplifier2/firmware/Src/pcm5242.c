#include "base.h"

#define ADDR 0b10011000

static uint8_t MY_Read_REG(uint8_t reg)
{
    uint8_t data = 0;
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 100);
    if (flag != HAL_OK) {
        printf("pcm5242 read err\n");
    }
    return data;
}

static void MY_Write_REG(uint8_t reg, uint8_t data)
{
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("pcm5242 write err\n");
    }
}

void pcm5242_init(void)
{
    //初始化引脚
    /* GPIO_InitTypeDef  GPIO_InitStruct; */
    /* GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP; */
    /* GPIO_InitStruct.Pull  = GPIO_NOPULL; */
    /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; */
    /* GPIO_InitStruct.Pin = GPIO_PIN_0; */
    /* HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); */
    //mute
    /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1); */
    //en
    /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1); */

    /* w 98 00 01 # PCM5242 back to page 1 */
    MY_Write_REG(0x00, 0x01);
    /* w 98 02 11 # PCM5242 output to 1 Vrms */
    MY_Write_REG(0x02, 0x11);

    /* w 98 00 00 # PCM5242 back to page 0 */
    MY_Write_REG(0x00, 0x00);
    /* w 98 3B 66 # set auto mute time to six seconds of audio zero. */
    MY_Write_REG(0x3b, 0x66);
    /* w 98 3F BB # set vol changes for every 4 samples, 0.5 sample steps. */
    MY_Write_REG(0x3f, 0xbb);

    /* I2S Shift */
    MY_Write_REG(0x29, 1);

    /* TDM */
    MY_Write_REG(0x28, 0b00010000);

    /* Standby mode */
    MY_Write_REG(0x02, 0b00010000);
}

void pcm5242_play(uint32_t AudioFreq, uint8_t bit_depth)
{
    MY_Write_REG(0x02, 0b00010000);

    if (bit_depth == 32U) {
        MY_Write_REG(0x28, 0b00010011); /* TDM 32bit */
    } else if (bit_depth == 24U) {
        MY_Write_REG(0x28, 0b00010010); /* TDM 24bit */
    } else {
        MY_Write_REG(0x28, 0b00010000); /* TDM 16bit */
    }

    if (AudioFreq == SAI_AUDIO_FREQUENCY_96K) {
        MY_Write_REG(0x22, 0b00000001); /* FS 96khz */
    } else {
        MY_Write_REG(0x22, 0);          /* FS 48khz */
    }

    /* play */
    MY_Write_REG(0x02, 0b00000000);
}

void pcm5242_mute(bool ok)
{
    if (ok) {
        MY_Write_REG(0x03, 0b00010001);
    } else {
        MY_Write_REG(0x03, 0b00000000);
    }
}

void pcm5242_volume(uint8_t vol)
{
    vol = 0xFF - vol;
    MY_Write_REG(0x3d, vol);
    MY_Write_REG(0x3e, vol);
}

void pcm5242_stop(void)
{
    MY_Write_REG(0x02, 0b00010000);
}

void pcm5242_headphone_in_irq()
{
}
