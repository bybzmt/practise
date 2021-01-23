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
    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    /* bsp_ssd1306_init(); */

    /* usb_to_sai_init(); */
    uint8_t buf[1024]={0};

    BSP_AUDIO_Init(0, 0, SAI_AUDIO_FREQUENCY_44K);
    BSP_AUDIO_Play(buf, 1024);

    bsp_tas6424_init();

    printf("run!\n");
    return;

    int num = 0;
    while (1) {
        printf("run:%d\n", num++);
        vTaskDelay(1000);
    }
}

