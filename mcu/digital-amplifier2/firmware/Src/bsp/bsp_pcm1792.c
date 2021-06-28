#include "base.h"
#include "bsp_pcm1792.h"
#include "msp_i2c.h"

#define ADDR 0b10011000

static uint8_t reg_18 = 0b10000000;

static uint8_t _volume_filter(volume_t vol)
{
    if (vol > 0xCF) {
        vol = 0xCF;
    }

    if (vol > 0) {
        vol = (0xFF-0xCF) + vol;
    }

    return vol;
}

void bsp_pcm1792_play(uint32_t AudioFreq, uint8_t bit_depth, volume_t vol)
{
    reg_18 &= ~(7<<4);

    if (bit_depth == 16) {
        reg_18 |= 4<<4;
    } else {
        reg_18 |= 5<<4;
    }

    vol = _volume_filter(vol);

    uint8_t d[] = {vol, vol, reg_18};
    msp_i2c_write(ADDR, 16, d, 3);
}

void bsp_pcm1792_volume(uint8_t vol)
{
    vol = _volume_filter(vol);

    uint8_t d[] = {vol, vol};
    msp_i2c_write(ADDR, 16, d, 2);
}

void bsp_pcm1792_init(void)
{
    //初始化引脚
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
}

void bsp_pcm1792_deInit(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
}
