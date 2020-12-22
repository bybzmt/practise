#include "base.h"

USBD_HandleTypeDef USBD_Device;

void usb_to_sai_init(void)
{
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

    USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);

    USBD_Start(&USBD_Device);
}

void UserMain()
{
    usb_to_sai_init();

    printf("run!\n");

    /* int num = 0; */
    while (1) {
        /* printf("run:%d\n", num++); */
        vTaskDelay(1000);
    }
}

