#include "base.h"

#define ADDR 0xD4

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
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 100);
    if (flag != HAL_OK) {
        printf("tas6424 readReg err\n");
    }
    return data;
}

static void MY_Write_REG(uint8_t reg, uint8_t data)
{
    /* HAL_StatusTypeDef flag = HAL_I2C_Mem_Write_DMA(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1); */
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, ADDR, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("tas6424 write err\n");
    }
}

static void bsp_tas6424_en(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
    vTaskDelay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
    vTaskDelay(100);
}

void bsp_tas6424_init(void)
{
    bsp_tas6424_en();

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
