#include "base.h"

#define ADDR 0xD4

static bool tas6424_en_flag;
static void tas6424_msp_init(void);
static bool tas6424_checkFatalError(void);
static void tas6424_reporting(void);

static void MY_Read(uint8_t reg, uint8_t*data, uint16_t len)
{
    HAL_StatusTypeDef flag = HAL_I2C_Mem_Read(&hi2c1, ADDR, (uint16_t)reg, 1, data, len, 100);
    if (flag != HAL_OK) {
        printf("tas6424 read err\n");
    }
}

/* static uint8_t MY_Read_REG(uint8_t reg) */
/* { */
    /* uint8_t data = 0; */
    /* MY_Read(reg, &data, 1); */
    /* return data; */
/* } */

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

void tas6424_en(bool flag)
{
    tas6424_en_flag = flag;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, flag);
}

static void tas6424_msp_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* en pin */
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* mute pin */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* fall event */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    /* GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; */
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); */
}

void tas6424_init(void)
{
    tas6424_msp_init();

    tas6424_en(true);

    vTaskDelay(6);

    /* reset device */
    MY_Write_REG(0x00, 0x80);
    vTaskDelay(5);
    MY_Write_REG(0x21, 0x80);
    vTaskDelay(5);

    /* Automatic diagnostics when leaving Hi-Z and after channel fault */
    MY_Write_REG(0x09, 0x00);
    /* shorted-load threshold: 1.0Ω */
    MY_Write_REG(0x0a, 0b00010001);
    MY_Write_REG(0x0b, 0b00010001);
    /* All Channel State: DC load diagnostics */
    MY_Write_REG(0x04, 0xff);

    /* Set Channels BTL or PBTL mode */
    /*
     * High pass filter eneabled
     * Global overtemperature warning set to 120°C
     * Overcurrent is level 2
     * Volume update rate is 1 step /8 FSYNC
     * 7.6-V peak output voltage
     */
    MY_Write_REG(0x01, 0b00111100);

    /* Hi-Z */
    MY_Write_REG(0x04, 0b01010101);
}

void tas6424_mute(bool ok)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, !ok);
}

/* volume: 0.5db/setp 0xff +24db - 0x00 -103db */
void tas6424_vol(uint8_t vol)
{
    uint8_t raw[] = {vol, vol, vol, vol};
    MY_Write(0x5, raw, 4);
}

void tas6424_play(uint32_t AudioFreq)
{
    /* Hi-Z */
    MY_Write_REG(0x04, 0b01010101);

    if (tas6424_checkFatalError()) {
        printf("tas6424 stop\n");
        return;
    }

    /* 44khz TDM */
    uint8_t freq = 0b00010100;
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

static bool tas6424_checkFatalError(void)
{
    uint8_t data[2]={0};

    MY_Read(0x0C, (uint8_t*)&data, 2);

    if ((data[0] & 0b11011101) || (data[1] & 0b11011101)) {
        printf("diagnostic: 0x%X 0x%X\n", data[0], data[1]);
        return true;
    }

    return false;
}

static char *chanel_state[] = {
    "PLAY",
    "Hi-Z",
    "MUTE",
    "DC load diagnostics",
};

static char *c_yes_no[] = {
    "No",
    "Yes",
};

void tas6424_check(void)
{
    if (!tas6424_en_flag) {
        return;
    }

    uint8_t p14 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14);
    uint8_t p15 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);

    if (p14 && p15) {
        return;
    }

    tas6424_reporting();

    /* Hi-Z */
    MY_Write_REG(0x04, 0b01010101);

    /* clear fault and play */
    MY_Write_REG(0x21, 0x80);
    vTaskDelay(5);
    MY_Write_REG(0x04, 0b00000101);
}

static void tas6424_reporting(void)
{
    uint8_t data[5]={0};

    MY_Read(0x0F, &data[0], 5);

    char *fmt= "CH%d STATE: %s\n";
    printf(fmt, 1, chanel_state[(data[0]>>6) & 3]);
    printf(fmt, 2, chanel_state[(data[0]>>4) & 3]);
    printf(fmt, 3, chanel_state[(data[0]>>2) & 3]);
    printf(fmt, 4, chanel_state[(data[0]) & 3]);

    fmt = "CH%d Overcurrent: %s\n";
    printf(fmt, 1, c_yes_no[(data[1]>>7) & 1]);
    printf(fmt, 2, c_yes_no[(data[1]>>6) & 1]);
    printf(fmt, 3, c_yes_no[(data[1]>>5) & 1]);
    printf(fmt, 4, c_yes_no[(data[1]>>4) & 1]);

    fmt = "CH%d DC fault: %s\n";
    printf(fmt, 1, c_yes_no[(data[1]>>3) & 1]);
    printf(fmt, 2, c_yes_no[(data[1]>>2) & 1]);
    printf(fmt, 3, c_yes_no[(data[1]>>1) & 1]);
    printf(fmt, 4, c_yes_no[(data[1]) & 1]);

    printf("Clock fault: %s\n",       c_yes_no[(data[2]>>4) & 1]);
    printf("PVDD overvoltage: %s\n",  c_yes_no[(data[2]>>3) & 1]);
    printf("VBAT overvoltage: %s\n",  c_yes_no[(data[2]>>2) & 1]);
    printf("PVDD undervoltage: %s\n", c_yes_no[(data[2]>>1) & 1]);
    printf("VBAT undervoltage: %s\n", c_yes_no[(data[2]>>0) & 1]);

    printf("Global overtemperature: %s\n", c_yes_no[(data[3]>>4) & 1]);

    fmt = "CH%d Overtemperature: %s\n";
    printf(fmt, 1, c_yes_no[(data[3]>>3) & 1]);
    printf(fmt, 2, c_yes_no[(data[3]>>2) & 1]);
    printf(fmt, 3, c_yes_no[(data[3]>>1) & 1]);
    printf(fmt, 4, c_yes_no[(data[3]>>0) & 1]);

    printf("VDD POR occurred: %s\n",               c_yes_no[(data[4]>>5) & 1]);
    printf("Global overtemperature warning: %s\n", c_yes_no[(data[4]>>4) & 1]);

    fmt = "CH%d overtemperature warning: %s\n";
    printf(fmt, 1, c_yes_no[(data[4]>>3) & 1]);
    printf(fmt, 2, c_yes_no[(data[4]>>2) & 1]);
    printf(fmt, 3, c_yes_no[(data[4]>>1) & 1]);
    printf(fmt, 4, c_yes_no[(data[4]>>0) & 1]);

    MY_Read(0x0C, (uint8_t*)&data, 2);

    printf("CH1 short-to-GND detected: %s\n",     c_yes_no[(data[0]>>7) & 1]);
    printf("CH1 short-to-power detected: %s\n",   c_yes_no[(data[0]>>6) & 1]);
    printf("CH1 Open load detected: %s\n",        c_yes_no[(data[0]>>5) & 1]);
    printf("CH1 Shorted load detected: %s\n",     c_yes_no[(data[0]>>4) & 1]);

    printf("CH2 short-to-GND detected: %s\n",     c_yes_no[(data[0]>>3) & 1]);
    printf("CH2 short-to-power detected: %s\n",   c_yes_no[(data[0]>>2) & 1]);
    printf("CH2 Open load detected: %s\n",        c_yes_no[(data[0]>>1) & 1]);
    printf("CH2 Shorted load detected: %s\n",     c_yes_no[(data[0]>>0) & 1]);

    printf("CH3 short-to-GND detected: %s\n",     c_yes_no[(data[1]>>7) & 1]);
    printf("CH3 short-to-power detected: %s\n",   c_yes_no[(data[1]>>6) & 1]);
    printf("CH3 Open load detected: %s\n",        c_yes_no[(data[1]>>5) & 1]);
    printf("CH3 Shorted load detected: %s\n",     c_yes_no[(data[1]>>4) & 1]);

    printf("CH4 short-to-GND detected: %s\n",     c_yes_no[(data[1]>>3) & 1]);
    printf("CH4 short-to-power detected: %s\n",   c_yes_no[(data[1]>>2) & 1]);
    printf("CH4 Open load detected: %s\n",        c_yes_no[(data[1]>>1) & 1]);
    printf("CH4 Shorted load detected: %s\n",     c_yes_no[(data[1]>>0) & 1]);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14|GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* tas6424_reporting(); */
}
