#include "base.h"

#define ADDR 0b10011000

void bsp_pcm5242_init(void)
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
    msp_i2c_write_reg(ADDR, 0x00, 0x01);
    /* w 98 02 11 # PCM5242 output to 1 Vrms */
    msp_i2c_write_reg(ADDR, 0x02, 0x11);

    /* w 98 00 00 # PCM5242 back to page 0 */
    msp_i2c_write_reg(ADDR, 0x00, 0x00);
    /* w 98 3B 66 # set auto mute time to six seconds of audio zero. */
    msp_i2c_write_reg(ADDR, 0x3b, 0x66);
    /* w 98 3F BB # set vol changes for every 4 samples, 0.5 sample steps. */
    msp_i2c_write_reg(ADDR, 0x3f, 0xbb);

    /* I2S Shift */
    msp_i2c_write_reg(ADDR, 0x29, 1);

    /* TDM */
    msp_i2c_write_reg(ADDR, 0x28, 0b00010000);

    /* Standby mode */
    msp_i2c_write_reg(ADDR, 0x02, 0b00010000);
}

void bsp_pcm5242_play(uint32_t AudioFreq, uint8_t bit_depth)
{
    msp_i2c_write_reg(ADDR, 0x02, 0b00010000);

    if (bit_depth == 32U) {
        msp_i2c_write_reg(ADDR, 0x28, 0b00010011); /* TDM 32bit */
    } else if (bit_depth == 24U) {
        msp_i2c_write_reg(ADDR, 0x28, 0b00010010); /* TDM 24bit */
    } else {
        msp_i2c_write_reg(ADDR, 0x28, 0b00010000); /* TDM 16bit */
    }

    if (AudioFreq == SAI_AUDIO_FREQUENCY_96K) {
        msp_i2c_write_reg(ADDR, 0x22, 0b00000001); /* FS 96khz */
    } else {
        msp_i2c_write_reg(ADDR, 0x22, 0);          /* FS 48khz */
    }

    /* play */
    msp_i2c_write_reg(ADDR, 0x02, 0b00000000);
}

void bsp_pcm5242_mute(bool ok)
{
    if (ok) {
        msp_i2c_write_reg(ADDR, 0x03, 0b00010001);
    } else {
        msp_i2c_write_reg(ADDR, 0x03, 0b00000000);
    }
}

void bsp_pcm5242_volume(uint8_t vol)
{
    vol = 0xFF - vol;
    msp_i2c_write_reg(ADDR, 0x3d, vol);
    msp_i2c_write_reg(ADDR, 0x3e, vol);
}

void bsp_pcm5242_stop(void)
{
    msp_i2c_write_reg(ADDR, 0x02, 0b00010000);
}

