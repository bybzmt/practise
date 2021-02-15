#include "base.h"

USBD_HandleTypeDef USBD_Device;

void usb_init(void)
{
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

    /* USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops); */
}

void usb_start(void)
{
    USBD_Start(&USBD_Device);
}

void usb_stop(void)
{
    printf("sub stop\n");

    bsp_tas6424_deInit();

    USBD_Stop(&USBD_Device);
}

void UserMain()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    bsp_tas6424_init();


    /* spdif_init(); */

    usb_init();
    usb_start();

    bool spdif_check = false;

    /* for (;;) { */
        /* if (spdif_check != volume_mute) { */
            /* if (volume_mute) { */
                /* spdif_start(); */
            /* } else { */
                /* spdif_stop(); */
            /* } */
            /* spdif_check = volume_mute; */
        /* } */

        /* vTaskDelay(2000); */
    /* } */

    printf("runing.\n");
}

