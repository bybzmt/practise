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
    bsp_tas6424_init();

    usb_to_sai_init();

    printf("runing.\n");

    /* while(1) { */
        /* uint8_t f1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14); */
        /* uint8_t f2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15); */
        /* printf("pin: %d %d\n", f1, f2); */
        /* vTaskDelay(1000); */
    /* } */
}

