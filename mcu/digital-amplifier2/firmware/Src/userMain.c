#include "base.h"

USBD_HandleTypeDef USBD_Device;

bool usb_used = false;
bool spdif_run = false;

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
        if (!usb_used) {
            if (!spdif_run) {
                spdif_start();
            } else {
                if (audio.enable == false) {
                    spdif_stop();
                    spdif_start();
                }
            }
        }

        tas6424_check();

        vTaskDelay(3000);
    }

    printf("runing.\n");
}

