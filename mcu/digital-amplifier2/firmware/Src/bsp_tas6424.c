#include "base.h"

#define ADDR 0xD5

static HAL_StatusTypeDef MY_Read(uint8_t reg, uint8_t*data, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, len, 100);
}

static uint8_t MY_Read_REG(uint8_t reg)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 100);
    return data;
}

static HAL_StatusTypeDef MY_Write_REG(uint8_t reg, uint8_t data)
{
    return HAL_I2C_Mem_Write_DMA(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1);
}

void bsp_tas6424_init(void)
{
    /* Set Channels BTL or PBTL mode */
    MY_Write_REG(0x00, 0x00);
    /*
     * High pass filter eneabled
     * Global overtemperature warning set to 110°C
     * Overcurrent is level 2
     * Volume update rate is 1 step / FSYNC
     */
    MY_Write_REG(0x01, 0b01110000);
    /*
     * Serial Audio-Port Control
     * 00: 44.1 kHz * 01: 48 kHz * 10: 96 kHz
     */
    MY_Write_REG(0x03, 0b01000110);
    /* Channel State Control Register */
    MY_Write_REG(0x04, 0b00000101);
    /* Volume ch1 */
    MY_Write_REG(0x05, 0xCF - 40);
    /* Volume ch2 */
    MY_Write_REG(0x06, 0xCF - 40);
    /* Volume ch3 */
    MY_Write_REG(0x07, 0xCF - 40);
    /* Volume ch4 */
    MY_Write_REG(0x08, 0xCF - 40);
}

void bsp_tas6424_DC_diagnostic(void)
{
    /* Automatic diagnostics when leaving Hi-Z and after channel fault */
    MY_Write_REG(0x09, 0x00);
    /* shorted-load threshold: 2Ω */
    MY_Write_REG(0x0a, 0b00110011);
    MY_Write_REG(0x0b, 0b00110011);
    /* All Channel State: DC load diagnostics */
    MY_Write_REG(0x04, 0xff);

    uint32_t data;

    MY_Read(0x0C, (uint8_t*)&data, 3);
}
