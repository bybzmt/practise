#include "base.h"

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
    printf("sub stop\n");

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

void UserMain()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    tas6424_init();

    audio.vol = 70;
    audio.mute = false;

    usb_init();
    usb_start();

    for (;;) {
        if (device_mode == MODE_IDLE) {
            device_mode_change(MODE_SPDIF);
            spdif_start();
        } else if (device_mode == MODE_SPDIF) {
            if (audio.enable == false) {
                spdif_stop();
                spdif_start();
            }
        }

        tas6424_check();

        vTaskDelay(2000);
    }

    printf("runing.\n");
}

