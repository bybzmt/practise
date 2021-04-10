#include "base.h"


IWDG_HandleTypeDef IwdgHandle;

USBD_HandleTypeDef USBD_Device;
uint8_t device_mode = MODE_IDLE;

void usb_init(void)
{
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
}

void usb_start(void)
{
    USBD_Start(&USBD_Device);
}

void usb_stop(void)
{
    USBD_Stop(&USBD_Device);
}

uint8_t device_mode;
void device_mode_change(uint8_t mode)
{
    switch (device_mode) {
        case MODE_SPDIF: spdif_stop(); break;
        case MODE_BT: break;
        case MODE_USB: break;
    }

    device_mode = mode;
}

static void test_write()
{
    uint8_t reg = 0;
    uint8_t data = 0;

    HAL_StatusTypeDef flag = HAL_I2C_Mem_Write(&hi2c1, 0b10011000, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("pcm5242 write err1\n");
    }

    flag = HAL_I2C_Mem_Write(&hi2c1, 0b10011010, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("pcm5242 write err2\n");
    }

    flag = HAL_I2C_Mem_Write(&hi2c1, 0b10011100, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("pcm5242 write err3\n");
    }

    flag = HAL_I2C_Mem_Write(&hi2c1, 0b10011110, (uint16_t)reg, 1, &data, 1, 500);
    if (flag != HAL_OK) {
        printf("pcm5242 write err4\n");
    }
}

void UserMain()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    for (uint8_t i=0;;i++) {
        printf("----%d----\n", i);
        test_write();
        vTaskDelay(3000);
    }

    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;
    IwdgHandle.Init.Reload = 40000/128*5;
    HAL_IWDG_Init(&IwdgHandle);

    tas6424_init();
    pcm5242_init();

    usb_init();
    usb_start();

    printf("boot\n");

    for (;;) {
        vTaskDelay(1000);

        if (device_mode == MODE_IDLE) {
            device_mode_change(MODE_SPDIF);
            spdif_start();
        }

        /* tas6424_check(); */
        audio_check();

        if ( audio.state == AUDIO_STATE_ERROR ) {
            NVIC_SystemReset();
        }

        HAL_IWDG_Refresh(&IwdgHandle);
    }
}

