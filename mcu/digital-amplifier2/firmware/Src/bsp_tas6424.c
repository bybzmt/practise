#include "base.h"

#define ADDR 0xD4

uint8_t volume = 100;
bool volume_mute = false;

bool bsp_tas6424_DC_diagnostic(void);

static void MY_Read(uint8_t reg, uint8_t*data, uint16_t len)
{
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, data, len, 100);
    if (flag != HAL_OK) {
        printf("tas6424 read err\n");
    }
}

static uint8_t MY_Read_REG(uint8_t reg)
{
    uint8_t data = 0;
    MY_Read(reg, &data, 1);
    return data;
}

static void MY_Write(uint8_t reg, uint8_t*data, uint16_t len)
{
    /* HAL_StatusTypeDef flag = HAL_I2C_Mem_Write_DMA(&hi2c1, ADDR, (uint16_t)reg, 1, &data, len); */
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, ADDR, (uint16_t)reg, 1, data, len, 500);
    if (flag != HAL_OK) {
        printf("tas6424 write err\n");
    }
}

static void MY_Write_REG(uint8_t reg, uint8_t data)
{
    MY_Write(reg, &data, 1);
}

static void bsp_tas6424_en(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
    vTaskDelay(50);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
    vTaskDelay(50);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void bsp_tas6424_init(void)
{
    bsp_tas6424_en();

    /* reset device */
    MY_Write_REG(0x00, 0x80);

    /* Automatic diagnostics when leaving Hi-Z and after channel fault */
    /* MY_Write_REG(0x09, 0x00); */
    /* shorted-load threshold: 1.5Ω */
    /* MY_Write_REG(0x0a, 0b00100010); */
    /* MY_Write_REG(0x0b, 0b00100010); */
    /* All Channel State: DC load diagnostics */
    MY_Write_REG(0x04, 0xff);

    /* Set Channels BTL or PBTL mode */
    /*
     * High pass filter eneabled
     * Global overtemperature warning set to 110°C
     * Overcurrent is level 2
     * Volume update rate is 1 step / FSYNC
     */
    /* MY_Write_REG(0x01, 0b01110010); */
    /* i2s 44k */
    MY_Write_REG(0x03, 0b00000100);
    /* 44khz TDM */
    /* MY_Write_REG(0x03, 0b00000110); */

    bsp_tas6424_vol(volume);
    bsp_tas6424_mute(volume_mute);
}

void bsp_tas6424_deInit(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
}

void bsp_tas6424_mute(bool ok)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, !ok);
}

void bsp_tas6424_vol(uint8_t _vol)
{
    /* Volume */
    uint8_t vol = _vol>0 ? (_vol* 2) + 7 : 0;
    uint8_t raw[] = {vol, vol, vol, vol};
    MY_Write(0x5, raw, 4);
}

void bsp_tas6424_play(uint32_t AudioFreq)
{
    if (bsp_tas6424_DC_diagnostic()) {
        /* printf("tas6424 stop\n"); */
        /* return; */
    }

    /* 44khz TDM */
    uint8_t freq = 0b00000100;
    switch(AudioFreq) {
        case SAI_AUDIO_FREQUENCY_48K:
            freq |= 0b01000000;
            printf("48khz\n");
            break;
        case SAI_AUDIO_FREQUENCY_96K:
            freq |= 0b10000000;
            printf("96khz\n");
            break;
    }
    MY_Write_REG(0x03, freq);

    /* play */
    MY_Write_REG(0x04, 0b00000101);
}

bool bsp_tas6424_DC_diagnostic(void)
{
    uint16_t data=0;

    MY_Read(0x0C, (uint8_t*)&data, 2);

    if (data != 0) {
        printf("diagnostic: 0x%X 0x%X\n", (uint8_t)(data>>8), (uint8_t)data);
        return true;
    }

    return false;
}
