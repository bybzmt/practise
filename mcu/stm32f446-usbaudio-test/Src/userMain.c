#include "base.h"

USBD_HandleTypeDef USBD_Device;

static void gpio_init()
{
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init_structure;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    //usb上拉电阻
    gpio_init_structure.Pin =  GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    //关
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
    HAL_Delay(100);
}

void UserMain()
{
    gpio_init();

    /* Init Device Library */
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

    /* Add Interface callbacks for AUDIO Class */
    USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);

    /* Start Device Process */
    USBD_Start(&USBD_Device);

    //usb上拉电阻
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);

    printf("run!\n");

    /* int num = 0; */
    while (1) {
        /* printf("run:%d\n", num++); */
        vTaskDelay(1000);
    }
}

